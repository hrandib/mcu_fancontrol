/*
 * Copyright (c) 2022 Dmytro Shestakov
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "comm_config.h"
#include "control_struct.h"
#include "controller.h"
#include "crc.h"
#include "device_info.h"
#include "flash.h"
#include "iwdg.h"
#include "scm_utils.h"
#include "sensor_handler.h"
#include "timers.h"
#include "uart.h"

using namespace Mcudrv;

typedef Uarts::UartIrq<UART_TX_RINGBUF_SIZE, UART_RX_RINGBUF_SIZE> Uart;

template void Uart::TxISR();
template void Uart::RxISR();

FORCEINLINE
static void InitUart()
{
    Uart::Init<Uarts::DefaultCfg, UART_BAUDRATE>();
}

static uint8_t WriteControlStruct();
static uint8_t WriteChannelsConfig();
static uint8_t WriteAnalogConfig();

#define PACKET_WAIT_TICKS (MS2ST(1000 * sizeof(ControlStruct) * 2) / (UART_BAUDRATE / 10) + 1)

SCM_TASK(ShellHandler, OS::pr0, 200)
{
    enum { POLL_PERIOD_MS = 16 };

    InitUart();
    while(true) {
        Mcudrv::Iwdg::Refresh();
        uint8_t c;
        while(Uart::Getch(c)) {
            switch(c) {
                // Device info
                case 'i': {
                    Uart::Putbuf(deviceInfo);
                    uint8_t sn = sensorHandler.GetSensorsNumber();
                    Uart::Putch(sn);
                    Uart::Putbuf(sensorHandler.GetIds(), SENSOR_MAX_NUMBER);
                } break;
                // Write config
                case 'w':
                    sleep(PACKET_WAIT_TICKS);
                    Uart::Putch(WriteControlStruct());
                    break;
                // Set active channels
                case 'e':
                    sleep(PACKET_WAIT_TICKS);
                    Uart::Putch(WriteChannelsConfig());
                    break;
                // Set analog channels, just for the report, not used in the business logic
                case 'a':
                    sleep(PACKET_WAIT_TICKS);
                    Uart::Putch(WriteAnalogConfig());
                    break;
                // Read config
                case 'r':
                    Uart::Putbuf(controlStruct);
                    break;
                // Sensor data
                case 't': {
                    int16_t values[8];
                    uint8_t sn = sensorHandler.GetSensorsNumber();
                    sensorMutex.lock();
                    sensorHandler.GetValues(values);
                    sensorMutex.unlock();
                    Uart::Putbuf((const uint8_t*)values, sn * 2);
                } break;
                // debug data
                case 'd': {
                    using namespace T1;
                    sensorMutex.lock();
                    uint8_t buf[4];
                    // PWM values
                    buf[0] = Timer1::ReadCompareByte<Ch1>();
                    buf[1] = Timer1::ReadCompareByte<Ch2>();
                    // Controller data
                    buf[2] = isStopped[0];
                    buf[3] = isStopped[1];
                    Uart::Putbuf(buf);
                    Uart::Putbuf(iVal);
                    sensorMutex.unlock();
                } break;
            }
        }
        sleep(MS2ST(POLL_PERIOD_MS));
    }
}

uint8_t WriteControlStruct()
{
    uint8_t result_status = 0;
    ControlStruct cs[CH_MAX_NUMBER];
    for(uint8_t ch = 0; ch < CH_MAX_NUMBER; ++ch) {
        Crc::Crc8 crc;
        uint8_t* arr = (uint8_t*)&cs[ch];
        uint8_t c;
        crc.Init(CRC_INIT_VAL);
        for(uint8_t bi = 0; bi < sizeof(ControlStruct); ++bi) {
            if(!Uart::Getch(c)) {
                break;
            }
            crc(c);
            arr[bi] = c;
        }
        if(!crc.GetResult()) {
            ++result_status;
        }
        else {
            break;
        }
    }
    if(result_status == CH_MAX_NUMBER) {
        using namespace Mem;
        Unlock<Eeprom>();
        sensorMutex.lock();
        if(IsUnlocked<Eeprom>()) {
            for(uint8_t i = 0; i < CH_MAX_NUMBER; ++i) {
                *const_cast<ControlStruct*>(&controlStruct[i]) = cs[i];
                ++result_status;
            }
        }
        Lock<Eeprom>();
        sensorMutex.unlock();
    }
    return result_status;
}

uint8_t popcount(uint8_t val)
{
    uint8_t result = 0;
    for(uint8_t i = 8; i--;) {
        result += (val & (1 << i)) ? 1 : 0;
    }
    return result;
}

uint8_t WriteChannelsConfig()
{
    using namespace Mem;
    uint8_t ch_mask, crc_val;
    uint8_t result = 0;
    Crc::Crc8 crc;
    crc.Init(CRC_INIT_VAL);
    result += Uart::Getch(ch_mask);
    crc(ch_mask);
    result += Uart::Getch(crc_val);
    crc(crc_val);
    result += !crc.GetResult();
    uint8_t ch_number = popcount(ch_mask);
    if(ch_number && ch_number <= CH_MAX_NUMBER) {
        ++result;
    }
    if(result == 4) {
        MemGuard<> mg;
        *const_cast<uint32_t*>(&deviceInfo.CHANNELS) = uint32_t(ch_number);
        *const_cast<uint32_t*>(&deviceInfo.CH_ENABLE_MASK) = uint32_t(ch_mask);
    }
    return result;
}

uint8_t WriteAnalogConfig()
{
    using namespace Mem;
    uint8_t ch_mask, crc_val;
    uint8_t result = 0;
    Crc::Crc8 crc;
    crc.Init(CRC_INIT_VAL);
    result += Uart::Getch(ch_mask);
    crc(ch_mask);
    result += Uart::Getch(crc_val);
    crc(crc_val);
    result += !crc.GetResult();
    uint8_t ch_number = popcount(ch_mask);
    if(ch_number <= CH_MAX_NUMBER) {
        ++result;
    }
    if(result == 4) {
        MemGuard<> mg;
        *const_cast<uint32_t*>(&deviceInfo.CH_ANALOG_MASK) = uint32_t(ch_mask);
    }
    return result;
}

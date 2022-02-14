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
#include "scm_utils.h"
#include "sensor_handler.h"
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

SCM_TASK(ShellHandler, OS::pr0, 200)
{
    enum { POLL_PERIOD_MS = 16 };

    InitUart();
    while(true) {
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
                    sleep(MS2ST(1000 * sizeof(ControlStruct) * 2) / (UART_BAUDRATE / 10) + 1);
                    Uart::Putch(WriteControlStruct());
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
                    // PWM values
                    Uart::Putch(Timer1::ReadCompareByte<Ch1>());
                    Uart::Putch(Timer1::ReadCompareByte<Ch2>());
                    // Controller data
                    Uart::Putch(isStopped[0]);
                    Uart::Putch(isStopped[1]);
                    Uart::Putbuf(iVal[0]);
                    Uart::Putbuf(iVal[1]);
                    sensorMutex.unlock();
                } break;
            }
        }
        sleep(MS2ST(POLL_PERIOD_MS));
    }
}

static uint8_t WriteControlStruct()
{
    uint8_t result_status = 0;
    ControlStruct cs[CH_NUMBER];
    for(uint8_t ch = 0; ch < CH_NUMBER; ++ch) {
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
    if(result_status == CH_NUMBER) {
        using namespace Mem;
        Unlock<Eeprom>();
        sensorMutex.lock();
        if(IsUnlocked<Eeprom>()) {
            for(uint8_t i = 0; i < CH_NUMBER; ++i) {
                *const_cast<ControlStruct*>(&controlStruct[i]) = cs[i];
                ++result_status;
            }
        }
        Lock<Eeprom>();
        sensorMutex.unlock();
    }
    return result_status;
}

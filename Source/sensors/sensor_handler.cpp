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

#include "sensor_handler.h"
#include "i2c.h"
#include "onewire.h"
#include "string_utils.h"

using namespace Mcudrv;

typedef Twis::SoftTwi<Twis::Standard, Pb4, Pb5> I2c;
typedef Twis::Lm75<I2c> Lm75;

typedef OneWire<Pd2> OneWireBus;
typedef Ds18b20<OneWireBus, SENSOR_MAX_NUMBER> Ds18;

enum { COLUMN_SIZE = 6 };

static uint8_t* FormatTempData(int16_t val, uint8_t* buf)
{
    uint8_t* ptr = io::itoa8(val >> 1, buf);
    *ptr++ = '.';
    *ptr++ = val & 1 ? '5' : '0';
    *ptr = '\0';
    return ptr;
}

static uint8_t* AddSpacing(uint8_t* buf, uint8_t spaces)
{
    while(spaces--) {
        *buf++ = ' ';
    }
    return buf;
}

SensorHandler::SensorHandler() : sensorIds_()
{
    I2c::Init();
    sensorsNumber_ = InitLm75();
    sensorsNumber_ = InitDs18(sensorsNumber_);
}

void SensorHandler::PrintTemp(BaseStream& bs)
{
    uint8_t buf[8];
    for(uint8_t i = 0; i < GetSensorsNumber(); ++i) {
        int16_t rawData = GetTemp(GetId(i));
        uint8_t* pos;
        if(rawData != 0xFFFF) {
            pos = FormatTempData(rawData, buf);
        }
        else {
            pos = buf;
            *pos++ = 'N';
            *pos++ = 'A';
        }
        pos = AddSpacing(pos, COLUMN_SIZE - (pos - buf));
        bs.Write(buf, pos - buf);
    }
    bs.Write("\r\n");
}

void SensorHandler::PrintIds(BaseStream& bs)
{
    uint8_t buf[8];

    bs.Write("Sensors available: ");
    io::utoa8(sensorsNumber_, buf);
    bs.Write(buf, 1);
    bs.Write("\r\n");

    for(uint8_t i = 0; i < GetSensorsNumber(); ++i) {
        uint8_t id = GetId(i);
        uint8_t* pos = buf;
        if(id & 0x80) {
            *pos++ = 'o';
            *pos++ = 'w';
        }
        pos = io::utoa8(id & ~DS18_ID_FLAG, pos, 16);
        pos = AddSpacing(pos, COLUMN_SIZE - (pos - buf));
        bs.Write(buf, pos - buf);
    }
    bs.Write("\r\n");
}

void SensorHandler::Convert()
{
    Ds18::Convert();
}

bool SensorHandler::Ds18sensorsPresent()
{
    return (bool)Ds18::GetSensorsNumber();
}

int16_t SensorHandler::GetTemp(uint8_t id)
{
    // DS18B20
    if(id & DS18_ID_FLAG) {
        return Ds18::Get(id & ~DS18_ID_FLAG) >> 3;
    }
    // LM75
    else {
        return Lm75::Read(id);
    }
}

uint8_t SensorHandler::InitLm75()
{
    uint8_t sensorPos = 0;
    for(uint8_t curSensorId = 0; curSensorId < 8; ++curSensorId) {
        if(Lm75::Detect(curSensorId)) {
            sensorIds_[sensorPos++] = curSensorId;
            if(sensorPos == SENSOR_MAX_NUMBER) {
                break;
            }
        }
    }
    return sensorPos;
}

uint8_t SensorHandler::InitDs18(uint8_t indexOffset)
{
    OneWireBus::Init();
    uint8_t sensorsNumber = Ds18::Init();
    for(uint8_t i = 0; i < sensorsNumber; ++i) {
        sensorIds_[i + indexOffset] = DS18_ID_FLAG | i;
    }
    return sensorsNumber + indexOffset;
}

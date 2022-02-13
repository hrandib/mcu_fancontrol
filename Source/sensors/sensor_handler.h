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

#ifndef SENSOR_HANDLER_H
#define SENSOR_HANDLER_H

#include "base_stream.h"
#include "scmRTOS.h"
#include <stdint.h>

#define SENSOR_MAX_NUMBER 8 // up to 4 LM75 and DS18B20, 8 in total

// Initializes and stores existing sensors
class SensorHandler
{
public:
    enum { CONVERT_TIME_MS = 100 };

    SensorHandler();
    void Init();

    int16_t GetTemp(uint8_t id);
    void PrintTemp(BaseStream& bs);
    void PrintIds(BaseStream& bs);
    void Convert();
    bool Ds18sensorsPresent();
    void GetValues(int16_t* buf);

    const uint8_t* GetIds()
    {
        return sensorIds_;
    }
    uint8_t GetId(uint8_t index)
    {
        return sensorIds_[index];
    }
    uint8_t GetSensorsNumber()
    {
        return sensorsNumber_;
    }
private:
    enum { DS18_ID_FLAG = 0x80 };
    uint8_t sensorsNumber_;
    uint8_t sensorIds_[SENSOR_MAX_NUMBER];

    uint8_t InitLm75();
    uint8_t InitDs18(uint8_t indexOffset);
};

extern SensorHandler sensorHandler;
extern OS::TMutex sensorMutex;

#endif // SENSOR_HANDLER_H

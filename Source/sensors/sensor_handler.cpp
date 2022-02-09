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

class Lm75sensor : public Sensor
{
    Lm75sensor(uint8_t id) : id_(id)
    { }

    // Sensor interface
public:
    uint8_t GetTemp()
    {
        return Lm75::Read(id_);
    }
    uint8_t GetId()
    {
        return id_;
    }
private:
    uint8_t id_;
};

class Ds18sensor : public Sensor
{

    // Sensor interface
public:
    uint8_t GetTemp();
    uint8_t GetId();
};

SensorHandler::SensorHandler(BaseStream& bs) : bs_(bs), lm75sensors_()
{
    I2c::Init();
    sensorsNumber_ = InitLm75();
    sensorsNumber_ += InitDs18();
    uint8_t buf[5];

    bs.Write("Temps: ");
    for(uint8_t i = 0; i < sensorsNumber_; ++i) {
        uint8_t result = Lm75::Read(lm75sensors_[i]);
        uint8_t* ptr = io::utoa8(result / 2, buf);
        bs.Write(buf, ptr - buf);
        bs.Put('.');
        bs.Put(result & 1 ? '5' : '0');
        bs.Put(' ');
    }
    bs.Write("\r\n");
}

int8_t SensorHandler::GetTemp(uint8_t id)
{
    return 0;
}

uint8_t SensorHandler::InitLm75()
{
    uint8_t sensorPos = 0;
    for(uint8_t curSensorId = 0; curSensorId < 8; ++curSensorId) {
        if(Lm75::Detect(curSensorId)) {
            lm75sensors_[sensorPos++] = curSensorId;
            if(sensorPos == SENSOR_MAX_NUMBER) {
                break;
            }
        }
    }
    return sensorPos;
}

uint8_t SensorHandler::InitDs18()
{
    return 0;
}

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

#include "controller.h"
#include "device_info.h"

static void InitFanStopPin();
static int16_t GetMaxTemp(const ControlStruct& cs, SensorHandler& sh);
static uint8_t Algo2PointFunc(int8_t channel, int16_t curTemp, const ControlStruct& cs);
static uint8_t AlgoPiFunc(uint8_t channel, int16_t curTemp, const ControlStruct& cs);
static void Worker(uint8_t channel, const ControlStruct& cs, SensorHandler& sh);

typedef Mcudrv::Pa3 FanStopPin;

bool isStopped[CH_MAX_NUMBER];
int16_t iVal[CH_MAX_NUMBER];

SCM_TASK(ControlLoop, OS::pr1, 150)
{
    uint8_t pollTimeCounter[CH_MAX_NUMBER]; // uninit as intended
    if(controlStruct[1].fanStopHysteresis) {
        InitFanStopPin();
    }
    InitPwm();
    sensorHandler.Init();
    while(true) {
        uint16_t ticks_start = get_tick_count();
        sensorMutex.lock();
        if(sensorHandler.Ds18sensorsPresent()) {
            sensorHandler.Convert();
        }
        sleep(MS2ST(100));
        for(uint8_t i = 0; i < CH_MAX_NUMBER; ++i) {
            if(++pollTimeCounter[i] >= controlStruct[i].pollTimeSecs) {
                Worker(i, controlStruct[i], sensorHandler);
                pollTimeCounter[i] = 0;
            }
        }
        sensorMutex.unlock();
        FanStopPin::SetOrClear(isStopped[1]);
        sleep(MS2ST(1000) - (get_tick_count() - ticks_start));
    }
}

void InitFanStopPin()
{
    using namespace Mcudrv;
    FanStopPin::SetConfig<GpioBase::Out_PushPull>();
    FanStopPin::Set();
}

void Worker(uint8_t channel, const ControlStruct& cs, SensorHandler& sh)
{
    int16_t tCurrent = GetMaxTemp(cs, sh);
    uint8_t pwmVal;
    if(cs.algoType == ControlStruct::ALGO_2POINT) {
        pwmVal = Algo2PointFunc(channel, tCurrent, cs);
    }
    else { // ALGO_PI
        pwmVal = AlgoPiFunc(channel, tCurrent, cs);
    }
    SetPwm(channel, pwmVal);
}

int16_t GetMaxTemp(const ControlStruct& cs, SensorHandler& sh)
{
    int16_t result = INT16_MIN;
    for(uint8_t i = 0; i < cs.sensorsNumber; ++i) {
        int16_t cur = sh.GetTemp(cs.sensorIds[i]);
        if(cur > result) {
            result = cur;
        }
    }
    return result;
}

uint8_t Algo2PointFunc(int8_t channel, int16_t curTemp, const ControlStruct& cs)
{
    uint8_t result = 0;
    const ControlStruct::Point2Algo& algo = cs.algo.p2Options;
    if(curTemp >= algo.tmax * 2) {
        result = cs.pwmMax;
    }
    else if(curTemp <= algo.tmin * 2) {
        if(cs.fanStopHysteresis && ((algo.tmin * 2) - curTemp) > (cs.fanStopHysteresis * 2)) {
            isStopped[channel] = true;
        }
        else if(!isStopped[channel]) {
            result = cs.pwmMin;
        }
    }
    else {
        uint8_t tRange = (algo.tmax - algo.tmin) * 2;
        uint16_t pwmRange = (cs.pwmMax - cs.pwmMin) << 8;
        uint16_t k = pwmRange / tRange;
        uint8_t tNorm = curTemp - (algo.tmin * 2);
        result = ((k * tNorm) >> 8) + cs.pwmMin;
        isStopped[channel] = false;
    }
    return result;
}
uint8_t AlgoPiFunc(uint8_t channel, int16_t curTemp, const ControlStruct& cs)
{
    const ControlStruct::PiAlgo& algo = cs.algo.piOptions;
    uint16_t result = 0;
    int16_t error = curTemp - (algo.t * 2);

    iVal[channel] += (algo.ki * error);
    if(iVal[channel] > (algo.max_i << 8)) {
        iVal[channel] = algo.max_i << 8;
    }
    else if(iVal[channel] < 0) {
        iVal[channel] = 0;
    }
    if(error > 0) {
        result = (((algo.kp * error) >> 5) + (iVal[channel] >> 8));
        if((result + cs.pwmMin) < cs.pwmMax) {
            result += cs.pwmMin;
        }
        else {
            result = cs.pwmMax;
        }
        isStopped[channel] = false;
    }
    else if(cs.fanStopHysteresis && error < (cs.fanStopHysteresis * 2)) {
        isStopped[channel] = true;
    }
    else if(!isStopped[channel]) {
        result = cs.pwmMin + (iVal[channel] >> 8);
    }

    return (uint8_t)result;
}

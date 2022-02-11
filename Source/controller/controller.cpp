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

static int16_t GetMaxTemp(const ControlStruct& cs, SensorHandler& sh);
uint8_t Algo2PointFunc(int16_t curTemp, const ControlStruct& cs);
uint8_t AlgoPiFunc(int16_t curTemp, const ControlStruct& cs);
static void Worker(const ControlStruct& cs, SensorHandler& sh);

SCM_TASK(ControlLoop, OS::pr1, 150)
{
    uint8_t pollTimeCounter[CHANNELS_NUMBER]; // uninit as intended
    InitPwm();
    SensorHandler sensorHandler;
    baseStream->Put('\r');
    sensorHandler.PrintIds(*baseStream);
    sensorHandler.PrintTemp(*baseStream);
    while(true) {
        if(sensorHandler.Ds18sensorsPresent()) {
            sensorHandler.Convert();
        }
        sleep(S2ST(1));
        for(uint8_t i = 0; i < CHANNELS_NUMBER; ++i) {
            if(++pollTimeCounter[i] >= controlStruct[i].pollTimeSecs) {
                Worker(controlStruct[i], sensorHandler);
                pollTimeCounter[i] = 0;
            }
        }
    }
}

static void Worker(const ControlStruct& cs, SensorHandler& sh)
{
    int16_t tCurrent = GetMaxTemp(cs, sh);
    uint8_t pwmVal;
    if(cs.algoType == ControlStruct::ALGO_2POINT) {
        pwmVal = Algo2PointFunc(tCurrent, cs);
    }
    else { // ALGO_PI
        pwmVal = AlgoPiFunc(tCurrent, cs);
    }
    SetPwm(cs.pwmChannel, pwmVal);
    if(cs.pwmChannel) {
        //    baseStream->Write("PWM channel: ");
        //    baseStream->Put('0' + cs.pwmChannel);

        uint8_t buf[4];
        baseStream->Write("Temp: ");
        uint8_t* ptr = io::itoa16(tCurrent / 2, buf);
        baseStream->Write(buf, ptr - buf);

        baseStream->Write("\r\nPWM: ");
        ptr = io::itoa16(pwmVal, buf);
        baseStream->Write(buf, ptr - buf);
        baseStream->Write("\r\n\r\n");
    }
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

uint8_t Algo2PointFunc(int16_t curTemp, const ControlStruct& cs)
{
    uint8_t result;
    const ControlStruct::Point2Algo& algo = cs.algo.p2Options;
    if(curTemp >= algo.tmax * 2) {
        result = cs.pwmMax;
    }
    else if(curTemp <= algo.tmin * 2) {
        result = cs.pwmMin;
    }
    else {
        uint8_t tRange = (algo.tmax - algo.tmin) * 2;
        uint16_t pwmRange = (cs.pwmMax - cs.pwmMin) << 8;
        uint16_t k = pwmRange / tRange;
        uint8_t tNorm = curTemp - (algo.tmin * 2);
        result = ((k * tNorm) >> 8) + cs.pwmMin;
    }
    return result;
}

uint8_t AlgoPiFunc(int16_t curTemp, const ControlStruct& cs)
{
    static int16_t iVal[CHANNELS_NUMBER];
    const ControlStruct::PiAlgo& algo = cs.algo.piOptions;
    uint16_t result;
    int16_t error = curTemp - (algo.t * 2);

    iVal[cs.pwmChannel] += (algo.ki * error);
    if(iVal[cs.pwmChannel] > (algo.max_i << 8)) {
        iVal[cs.pwmChannel] = algo.max_i << 8;
    }
    else if(iVal[cs.pwmChannel] < 0) {
        iVal[cs.pwmChannel] = 0;
    }
    if(error > 0) {
        result = (((algo.kp * error) >> 5) + (iVal[cs.pwmChannel] >> 8));
        if((result + cs.pwmMin) < cs.pwmMax) {
            result += cs.pwmMin;
        }
        else {
            result = cs.pwmMax;
        }
    }
    else {
        result = cs.pwmMin + (iVal[cs.pwmChannel] >> 8);
    }

    uint8_t buf[8];
    baseStream->Write("iVal: ");
    uint8_t* ptr = io::itoa16(iVal[cs.pwmChannel] >> 8, buf);
    baseStream->Write(buf, ptr - buf);

    baseStream->Write("\r\nError: ");
    ptr = io::itoa16(error, buf);
    baseStream->Write(buf, ptr - buf);

    baseStream->Write("\r\npVal: ");
    ptr = io::itoa16(((algo.kp * error) >> 5), buf);
    baseStream->Write(buf, ptr - buf);
    baseStream->Write("\r\n");

    return (uint8_t)result;
}

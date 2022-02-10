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

static void Worker(const ControlStruct& cs, SensorHandler& sh);

SCM_TASK(ControlLoop, OS::pr1, 150)
{
    uint8_t pollTimeCounter[CHANNELS_NUMBER] = { 0 };
    SensorHandler sensorHandler;
    sensorHandler.PrintIds(*baseStream);

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

static

  void
  Worker(const ControlStruct& cs, SensorHandler& sh)
{
    baseStream->Write("Worker: ");
    baseStream->Put('0' + cs.pollTimeSecs);
    baseStream->Write("\r\n");
    if(cs.pollTimeSecs == 4) {
        sh.PrintTemp(*baseStream);
    }
}

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
#include "scm_utils.h"
#include "uart_stream.h"

ControlStruct controlStruct[2];

SCM_TASK(ControlLoop, OS::pr1, 50)
{
    //    Controller c;
    //    Shell shell(uartStream);
    //    sensorHandler.PrintIds();
    //    sensorHandler.Init(&uartStream);
    //    uint8_t pollTimeCounter[3] = { 0 };
    while(true) {
        //        //        if(sensorHandler.IsDs18SensorsPresent()) {
        //        //            sensorHandler.Convert();
        //        //        }
        sleep(MS2ST(5000));
        //        //        for(uint8_t i = 0; i < 2; ++i) {
        //        //            if(++pollTimeCounter[i] >= controlStruct[i].pollTimeSecs) {
        //        //                c.Worker(controlStruct[0]);
        //        //                pollTimeCounter[i] = 0;
        //        //            }
        //        //        }
        //        //        if(++pollTimeCounter[2] >= 3) {
        //        //        sensorHandler.PrintTemp();
        //        //        }
        //        // Guard delay between possible 1-Wire operations
        //        sleep(MS2ST(100));
        baseStream->Write("Hello!\r\n");
    }
}

void Controller::Worker(const ControlStruct& cs)
{ }

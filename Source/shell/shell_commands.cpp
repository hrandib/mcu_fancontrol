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

#include "shell_commands.h"
#include "string_utils.h"
#include "timers.h"
#include <cstdlib>
#include <stddef.h>

using namespace Mcudrv;
using namespace T1;
using namespace io;

SHELL_FUNC(SetPwm)
{
    if(argc == 2) {
        uint8_t chNum = (uint8_t)atoi(argv[0]);
        uint8_t pwmVal = (uint8_t)atoi(argv[1]);
        if(!chNum) {
            Timer1::WriteCompareByte<Ch1>(pwmVal);
        }
        else {
            Timer1::WriteCompareByte<Ch2>(pwmVal);
        }
    }
    // TODO: Print help
}

SHELL_FUNC(GetPwm)
{
    uint8_t buf[8];
    uint8_t* ptr = utoa8(Timer1::GetCompareByte<Ch1>(), buf);
    *ptr = ' ';
    ptr = utoa8(Timer1::GetCompareByte<Ch2>(), ptr + 1);
    *ptr++ = '\r';
    *ptr++ = '\n';
    ios.Write(buf, ptr - buf);
}

// SHELL_FUNC(Sensors)
//{
//    sensorHandler.PrintIds(ios);
//    sensorHandler.PrintTemp(ios);
//}

const Command shell_commands[] = { { "setpwm", SetPwm }, { "getpwm", GetPwm }, { NULL, NULL } };

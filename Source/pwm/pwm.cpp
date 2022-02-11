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

#include "pwm.h"
#include "pinlist.h"
#include "timers.h"

using namespace Mcudrv;
using namespace T1;

void InitPwm()
{
    GpioC::SetConfig<P6 | P7, GpioBase::Out_PushPull_fast>();
    Timer1::Init(1, ARPE | CEN);
    Timer1::WriteAutoReload(PWM_MAXVAL); // PWM 25kHz
    Timer1::SetChannelCfg<Ch1, Output, ChannelCfgOut(Out_PWM_Mode1 | Out_PreloadEnable)>();
    Timer1::ChannelEnable<Ch1>();
    Timer1::SetChannelCfg<Ch2, Output, ChannelCfgOut(Out_PWM_Mode1 | Out_PreloadEnable)>();
    Timer1::ChannelEnable<Ch2>();
    Timer1::WriteCompareByte<Ch1>(PWM_MAXVAL / 2);
    Timer1::WriteCompareByte<Ch2>(PWM_MAXVAL / 2);
}

void SetPwm(uint8_t ch, uint8_t val)
{
    if(!ch) {
        Timer1::WriteCompareByte<Ch1>(val);
    }
    else {
        Timer1::WriteCompareByte<Ch2>(val);
    }
}

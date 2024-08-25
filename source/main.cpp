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

#include "clock.h"
#include "iwdg.h"
#include "pinlist.h"
#include "scm_utils.h"
#include "sensor_handler.h"
#include "timers.h"
#include "utils.h"

static void InitPeripherals()
{
    using namespace Mcudrv;
    using namespace T4;
    // FCPU = 8MHz
    SysClock::SetHsiDivider(SysClock::Div2);
    GpioA::WriteConfig<0xFF, GpioBase::In_Pullup>();
    GpioB::WriteConfig<0xFF, GpioBase::In_Pullup>();
    GpioC::WriteConfig<0xFF, GpioBase::In_Pullup>();
    GpioD::WriteConfig<0xFF, GpioBase::In_Pullup>();
    Timer4::Init(T4::Div_128, Cfg(ARPE | CEN)); // Systick = 244Hz
    Timer4::EnableInterrupt();
}

int main()
{
    // Better to enable in option bytes
    Mcudrv::Iwdg::Enable();
    Mcudrv::Iwdg::SetPeriod(Mcudrv::Iwdg::P_1s);

    InitPeripherals();
    enableInterrupts();
    OS::run();
}

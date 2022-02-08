#include "pinlist.h"
#include "scm_utils.h"
#include "timers.h"
#include "utils.h"

using namespace Mcudrv;
using namespace T1;

static const uint8_t PWM_MAXVAL = 80;
static const uint8_t PWM_MINVAL = 0;
static utils::RangeLinear<int8_t, PWM_MINVAL, PWM_MAXVAL, 20> pwmVal;

static void IncrementHandler(bool isLongPress)
{
    if(isLongPress) {
        pwmVal.SetMax();
    }
    else {
        ++pwmVal;
    }
    Timer1::WriteCompareByte<Ch1>(pwmVal);
    Timer1::WriteCompareByte<Ch2>(pwmVal);
}

static void DecrementHandler(bool isLongPress)
{
    if(isLongPress) {
        pwmVal.SetMin();
    }
    else {
        --pwmVal;
    }
    Timer1::WriteCompareByte<Ch1>(pwmVal);
    Timer1::WriteCompareByte<Ch2>(pwmVal);
}

static void InitPwm()
{
    GpioC::SetConfig<P6 | P7, GpioBase::Out_PushPull_fast>();
    Timer1::Init(1, ARPE | CEN);
    Timer1::WriteAutoReload(PWM_MAXVAL); // PWM 25kHz
    Timer1::SetChannelCfg<Ch1, Output, ChannelCfgOut(Out_PWM_Mode1 | Out_PreloadEnable)>();
    Timer1::ChannelEnable<Ch1>();
    Timer1::SetChannelCfg<Ch2, Output, ChannelCfgOut(Out_PWM_Mode1 | Out_PreloadEnable)>();
    Timer1::ChannelEnable<Ch2>();
    Timer1::WriteCompareByte<Ch1>(PWM_MAXVAL / 4);
    Timer1::WriteCompareByte<Ch2>(PWM_MAXVAL / 4);
}

static void InitPeripherals()
{
    GpioA::WriteConfig<0xFF, GpioBase::In_Pullup>();
    GpioB::WriteConfig<0xFF, GpioBase::In_Pullup>();
    GpioC::WriteConfig<0xFF, GpioBase::In_Pullup>();
    GpioD::WriteConfig<0xFF, GpioBase::In_Pullup>();
    T4::Timer4::Init(T4::Div_32, T4::Cfg(T4::ARPE | T4::CEN)); // Systick = 122Hz
    T4::Timer4::EnableInterrupt();
    InitPwm();
}

int main()
{
    InitPeripherals();
    enableInterrupts();
    OS::run();
}

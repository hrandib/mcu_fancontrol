#include "scm_utils.h"
#include "timers.h"
#include "pinlist.h"

using namespace Mcudrv;

static const uint8_t PWM_MAXVAL = 80;

SCM_TASK(ButtonHandler, OS::pr0, 200)
{
    using namespace T1;
    typedef Pb5 ButtonUp;
    typedef Pb6 ButtonDown;
    bool alreadyPressed = false;
    int8_t pwmVal = 0;
    const int8_t step = 20;
    while(true) {
        sleep(MS2ST(50));
        if(!ButtonDown::IsSet()) {
            if(alreadyPressed) {
                continue;
            }
            pwmVal -= step;
            if(pwmVal < 0) {
                pwmVal = 0;
            }
            Timer1::WriteCompareByte<Ch1>(pwmVal);
            alreadyPressed = true;
        }
        else if(!ButtonUp::IsSet()) {
            if(alreadyPressed) {
                continue;
            }
            if(pwmVal >= PWM_MAXVAL) {
                pwmVal = PWM_MAXVAL;
            }
            Timer1::WriteCompareByte<Ch1>(pwmVal);
            alreadyPressed = true;
        }
        else {
            alreadyPressed = false;
        }
    }
}

static void InitPwm()
{
    using namespace T1;
    GpioC::SetConfig<P6 | P7, GpioBase::Out_PushPull_fast>();
    Timer1::Init(1, ARPE | CEN);
    Timer1::WriteAutoReload(PWM_MAXVAL); // PWM 25kHz
    Timer1::SetChannelCfg<Ch1, Output, static_cast<ChannelCfgOut>(Out_PWM_Mode1 | Out_PreloadEnable)>();
    Timer1::SetChannelCfg<Ch2, Output, static_cast<ChannelCfgOut>(Out_PWM_Mode1 | Out_PreloadEnable)>();
    Timer1::ChannelEnable<Ch1>();
    Timer1::ChannelEnable<Ch2>();

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

int main() {
    InitPeripherals();
    enableInterrupts();
    OS::run();
}

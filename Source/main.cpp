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
}

SCM_TASK(ButtonsHandler, OS::pr0, 200)
{
    enum {
        BUTTONS_NUMBER = 2,
        LONGPRESS_DELAY_MS = 2000,
        POLL_PERIOD_MS = 16,
    };
    typedef Pinlist<Pd5, SequenceOf<BUTTONS_NUMBER> > ButtonPins;
    utils::ButtonsLongPress<BUTTONS_NUMBER, POLL_PERIOD_MS, LONGPRESS_DELAY_MS> buttons;
    buttons[0] = DecrementHandler;
    buttons[1] = IncrementHandler;
    while(true) {
        sleep(MS2ST(POLL_PERIOD_MS));
        buttons.UpdateState(ButtonPins::Read());
    }
}

static void InitPwm()
{
    GpioC::SetConfig<P6 | P7, GpioBase::Out_PushPull_fast>();
    Timer1::Init(1, ARPE | CEN);
    Timer1::WriteAutoReload(PWM_MAXVAL); // PWM 25kHz
    Timer1::SetChannelCfg<Ch1, Output, ChannelCfgOut(Out_PWM_Mode2 | Out_PreloadEnable)>();
    Timer1::ChannelEnable<Ch1>();
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

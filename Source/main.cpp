#include "scmRTOS.h"
#include "gpio.h"
#include "timers.h"
#include "pinlist.h"
#include "uart.h"
#include "i2c.h"

using namespace Mcudrv;
typedef Uarts::UartIrq<> Uart;

typedef Twis::SoftTwi<Twis::Standard, Pb4, Pb5> I2c;
typedef Twis::Lm75<I2c> Lm75;
const uint8_t lm75_devAddr = 7;

template void Uart::TxISR();
template void Uart::RxISR();

typedef OS::process<OS::pr0, 100> TProc1;
template<> void TProc1::exec();

static TProc1 proc1;

template<> void TProc1::exec()
{
    while(true) {
        sleep(200);
        Pd2::Toggle();
        int16_t val = Lm75::Read(lm75_devAddr);
        Uart::Puts(val >> 1);
        Uart::Putch('.');
        Uart::Putch((val & 0x01) ? '5' : '0');
        Uart::Newline();
    }
}

typedef OS::process<OS::pr1, 100> TProc2;
template<> void TProc2::exec();

static TProc2 proc2;

template<> void TProc2::exec()
{
    using namespace T1;
    while(true) {
        uint8_t ch;
        if(Uart::Getch(ch)) {
            switch(ch) {
            case 'a':
                Timer1::GetCompareByte<Ch1>() -= 1;
                break;
            case 's':
                Timer1::GetCompareByte<Ch2>() -= 1;
                break;
            case 'q':
                Timer1::GetCompareByte<Ch1>() += 1;
                break;
            case 'w':
                Timer1::GetCompareByte<Ch2>() += 1;
                break;
            }
            Uart::Puts(Timer1::ReadCompareByte<Ch1>(), 10);
            Uart::Putch(' ');
            Uart::Puts(Timer1::ReadCompareByte<Ch2>(), 10);
            Uart::Newline();
        }
        sleep(1);
    }
}

static void InitPwm()
{
    using namespace T1;
    GpioC::SetConfig<P6 | P7, GpioBase::Out_PushPull_fast>();
    Timer1::Init(1, ARPE | CEN);
    Timer1::WriteAutoReload(80);
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

    T4::Timer4::Init(T4::Div_32, T4::Cfg(T4::ARPE | T4::CEN)); // 122Hz
    T4::Timer4::EnableInterrupt();

    Pd2::SetConfig<GpioBase::Out_PushPull>();

    Uart::Init<Uarts::DefaultCfg, 57600>();
    I2c::Init();
    InitPwm();
}

int main() {
    InitPeripherals();
    enableInterrupts();
    OS::run();
}

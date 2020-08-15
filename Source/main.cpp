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

static TProc1 proc;

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

static void initPeripherals()
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
}

int main() {
    initPeripherals();
    enableInterrupts();
    OS::run();
}

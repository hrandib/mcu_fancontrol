#include "scmRTOS.h"
#include "gpio.h"
#include "timers.h"
#include "pinlist.h"
#include "uart.h"

using namespace Mcudrv;
typedef Uarts::UartIrq<> Uart;

template void Uart::TxISR();
template void Uart::RxISR();


typedef OS::process<OS::pr0, 100> TProc1;
template<> void TProc1::exec();

static TProc1 proc;

template<> void TProc1::exec()
{
    static uint32_t counter;
    while(true) {
        sleep(40);
        Pd2::Toggle();
        Uart::Puts("Hello ");
        Uart::Puts(counter++);
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
}

int main() {
    initPeripherals();
    enableInterrupts();
    OS::run();
}

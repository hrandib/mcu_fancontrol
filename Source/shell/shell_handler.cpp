#include "scm_utils.h"
#include "shell.h"
#include "uart_stream.h"

using namespace Mcudrv;

typedef Uarts::UartIrq<UART_TX_RINGBUF_SIZE, UART_RX_RINGBUF_SIZE> Uart;

template void Uart::TxISR();
template void Uart::RxISR();

FORCEINLINE
static void InitUart()
{
    Uart::Init<Uarts::DefaultCfg, UART_BAUDRATE>();
    Uart::Puts("\r\n[ FanController CLI ]\r\n");
}

SCM_TASK(ShellHandler, OS::pr0, CMD_BUF_SIZE + 100)
{
    enum { POLL_PERIOD_MS = 16 };

    InitUart();
    UartStream<Uart> uartStream;
    Shell shell(uartStream);

    while(true) {
        shell.handle();
        sleep(MS2ST(POLL_PERIOD_MS));
    }
}

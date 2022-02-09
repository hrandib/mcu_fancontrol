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
}

SCM_TASK(ShellHandler, OS::pr0, CMD_BUF_SIZE + 100)
{
    enum { POLL_PERIOD_MS = 3000 };

    InitUart();
    UartStream<Uart> uartStream;
    Shell shell(uartStream);
    SensorHandler sensorHandler(uartStream);
    sensorHandler.PrintIds();
    while(true) {
        //        shell.handle();
        sensorHandler.Convert();
        sleep(MS2ST(POLL_PERIOD_MS));
        sensorHandler.PrintTemp();
        sleep(MS2ST(100));
    }
}

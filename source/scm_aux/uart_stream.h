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

#ifndef BASE_STREAM_UART_H
#define BASE_STREAM_UART_H

#include "base_stream.h"
#include "uart.h"

namespace Mcudrv {

// Use Uarts::Uart... for specialization
template<typename Uart>
class UartStream : public BaseStream
{
public:
    byte_t Get()
    {
        byte_t b;
        while(!Uart::Getch(b))
            ;
        return b;
    }

    void Put(byte_t b)
    {
        Uart::Putch(b);
    }

    length_t Read(byte_t* buf, length_t len)
    {
        byte_t b;
        length_t result_len = len;
        while(result_len-- && Uart::Getch(b)) {
            *buf++ = b;
        }
        return len - result_len - 1;
    }

    length_t Write(const byte_t* buf, length_t len)
    {
        return Uart::Putbuf(buf, len);
    }
};

} // Mcudrv

// extern BaseStream* baseStream;

#endif // BASE_STREAM_UART_H

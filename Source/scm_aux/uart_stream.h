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

#endif // BASE_STREAM_UART_H

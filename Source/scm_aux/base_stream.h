#ifndef BASE_STREAM_H
#define BASE_STREAM_H

#include <stdint.h>

typedef uint8_t byte_t;
typedef uint16_t length_t;

struct BaseStream
{
    virtual byte_t Get() = 0;
    virtual void Put(byte_t) = 0;
    virtual length_t Read(byte_t*, length_t) = 0;
    virtual length_t Write(const byte_t*, length_t) = 0;

    template<typename T>
    length_t Read(T* buf, length_t len)
    {
        static_assert(sizeof(T) == 1, "The type size must be 1");
        return Read(reinterpret_cast<byte_t*>(buf), len);
    }

    template<typename T>
    length_t Write(T* buf, length_t len)
    {
        static_assert(sizeof(T) == 1, "The type size must be 1");
        return Write(reinterpret_cast<const byte_t*>(buf), len);
    }

    template<typename T, uint16_t N>
    length_t Write(T (&buf)[N])
    {
        static_assert(sizeof(T) == 1, "The type size must be 1");
        return Write(buf, N - 1);
    }
};

#endif // BASE_STREAM_H

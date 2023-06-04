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
        return Write(buf, N);
    }
};

#endif // BASE_STREAM_H

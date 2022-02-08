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

#ifndef SCM_UTILS_H
#define SCM_UTILS_H

#include "scmRTOS.h"

#define SCM_TASK(name, prio, stacksize)                \
    typedef OS::process<prio, stacksize> TProc_##name; \
                                                       \
    template<>                                         \
    void TProc_##name::exec();                         \
                                                       \
    static TProc_##name proc_##name;                   \
                                                       \
    template<>                                         \
    void TProc_##name::exec()

#define MS2ST(msecs) (timeout_t(((msecs * scmRTOS_TICK_RATE_HZ) + 999) / 1000))

#define S2ST(secs) (secs * scmRTOS_TICK_RATE_HZ))

#define ST2MS(ticks) (uint16_t(((ticks * 1000UL) + (scmRTOS_TICK_RATE_HZ - 1)) / scmRTOS_TICK_RATE_HZ))

#endif // SCM_UTILS_H

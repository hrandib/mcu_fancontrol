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

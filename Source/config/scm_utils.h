#ifndef SCM_UTILS_H
#define SCM_UTILS_H

#include "scmRTOS.h"

#define SCM_TASK(name, prio, stacksize) \
    typedef OS::process<prio, stacksize> TProc_##name; \
    \
    template<> void TProc_##name::exec(); \
    \
    static TProc_##name proc_##name; \
    \
    template<> void TProc_##name::exec()

#define MS2ST(ms) \
    (timeout_t((uint32_t)ms * scmRTOS_TICK_RATE_HZ / 1000 ? ms * scmRTOS_TICK_RATE_HZ / 1000 : 1))

#define S2ST(secs) \
  (MS2ST(secs * 1000))


#define ST2MS(ticks) \
    (ticks * 1000 / configTICK_RATE_HZ)

#endif // SCM_UTILS_H

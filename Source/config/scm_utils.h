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


#endif // SCM_UTILS_H

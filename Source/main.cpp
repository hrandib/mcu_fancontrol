#include "scmRTOS.h"

typedef OS::process<OS::pr0, 100> TProc1;
template<> void TProc1::exec();

static TProc1 proc;

template<> void TProc1::exec()
{
    while(true) {
        sleep(10);
    }
}

int main() {
    OS::run();
}

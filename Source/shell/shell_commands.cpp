#include "shell_commands.h"
#include "string_utils.h"
#include "timers.h"
#include <cstdlib>
#include <stddef.h>

using namespace Mcudrv;
using namespace T1;
using namespace io;

SHELL_FUNC(SetPwm)
{
    if(argc == 2) {
        uint8_t chNum = (uint8_t)atoi(argv[0]);
        uint8_t pwmVal = (uint8_t)atoi(argv[1]);
        if(!chNum) {
            Timer1::WriteCompareByte<Ch1>(pwmVal);
        }
        else {
            Timer1::WriteCompareByte<Ch2>(pwmVal);
        }
    }
    // TODO: Print help
}

SHELL_FUNC(GetPwm)
{
    uint8_t buf[8];
    uint8_t* ptr = utoa8(Timer1::GetCompareByte<Ch1>(), buf);
    *ptr = ' ';
    ptr = utoa8(Timer1::GetCompareByte<Ch2>(), ptr + 1);
    *ptr++ = '\r';
    *ptr++ = '\n';
    s.Write(buf, ptr - buf);
}

const Command shell_commands[] = { { "setpwm", SetPwm }, { "getpwm", GetPwm }, { NULL, NULL } };

#include "ext.h"


void _PrintfMsgSL(const char* msg, va_list arg);

void __PrintfMsg(const char* msg, ...)
{
    va_list arg;
    va_start(arg, msg);
    _PrintfMsgSL(msg, arg);
    va_end(arg);
}

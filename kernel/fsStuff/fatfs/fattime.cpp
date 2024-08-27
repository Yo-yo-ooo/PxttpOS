#include "ff.h"
#include "../../devices/rtc/rtc.h"

DWORD get_fattime (void){
    return (DWORD)(RTC::Year - 80) << 25 |
           (DWORD)(RTC::Month + 1) << 21 |
           (DWORD)RTC::Day << 16 |
           (DWORD)RTC::Hour << 11 |
           (DWORD)RTC::Minute << 5 |
           (DWORD)RTC::Second >> 1;
}
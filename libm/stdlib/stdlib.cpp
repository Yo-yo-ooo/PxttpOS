#include "./stdlib.h"
#include "../stdio/stdio.h"
#include "../ctype.h"
#include <libm/syscallManager.h>
#include <libm/msgPackets/msgPacket.h>
#include <libm/rnd/rnd.h>
#include <libm/cstrTools.h>
#include <libm/memStuff.h>
#include <libm/stubs.h>
#include "../stdio/stdargs.h"
#include <libm/cstr.h>
#include <libm/heap/heap.h>
#include <libm/list/list_basics.h>
#include <limits.h>
//#include "../stubs.h"


int _atoi(char* pstr)
{
	int Ret_Integer = 0;
	int Integer_sign = 1;
	
	/*
	* 判断指针是否为空
	*/
	if(pstr == NULL)
	{
		//STDIO::printf("Pointer is NULL\n");
		return 0;
	}
	
	/*
	* 跳过前面的空格字符
	*/
	while(isspace(*pstr) == 0)
	{
		pstr++;
	}
	
	/*
	* 判断正负号
	* 如果是正号，指针指向下一个字符
	* 如果是符号，把符号标记为Integer_sign置-1，然后再把指针指向下一个字符
	*/
	if(*pstr == '-')
	{
		Integer_sign = -1;
	}
	if(*pstr == '-' || *pstr == '+')
	{
		pstr++;
	}
	
	/*
	* 把数字字符串逐个转换成整数，并把最后转换好的整数赋给Ret_Integer
	*/
	while(*pstr >= '0' && *pstr <= '9')
	{
		Ret_Integer = Ret_Integer * 10 + *pstr - '0';
		pstr++;
	}
	Ret_Integer = Integer_sign * Ret_Integer;
	
	return Ret_Integer;
}


double _atof(const char *str)
{
	double s=0.0;
 
	double d=10.0;
	int jishu=0;
 
	bool falg=false;
 
	while(*str==' ')
	{
		str++;
	}
 
	if(*str=='-')//记录数字正负
	{
		falg=true;
		str++;
	}
 
	if(!(*str>='0'&&*str<='9'))//如果一开始非数字则退出，返回0.0
		return s;
 
	while(*str>='0'&&*str<='9'&&*str!='.')//计算小数点前整数部分
	{
		s=s*10.0+*str-'0';
		str++;
	}
 
	if(*str=='.')//以后为小树部分
		str++;
 
	while(*str>='0'&&*str<='9')//计算小数部分
	{
		s=s+(*str-'0')/d;
		d*=10.0;
		str++;
	}
 
	if(*str=='e'||*str=='E')//考虑科学计数法
	{
		str++;
		if(*str=='+')
		{
			str++;
			while(*str>='0'&&*str<='9')
			{
				jishu=jishu*10+*str-'0';
				str++;
			}
			while(jishu>0)
			{
				s*=10;
				jishu--;
			}
		}
		if(*str=='-')
		{
			str++;
			while(*str>='0'&&*str<='9')
			{
				jishu=jishu*10+*str-'0';
				str++;
			}
			while(jishu>0)
			{
				s/=10;
				jishu--;
			}
		}
	}
 
    return s*(falg?-1.0:1.0);
}

void *OS_calloc(size_t number, size_t size) {
    size_t i,nb;
    char* p,q;

    nb = number * size;
    if((p = q = _Malloc1(nb)) != NULL){
        for(i = 0; i < nb; i++)
            *p++ = 0;
    }
    return q;
}

void* OS_malloc(size_t size){
    return _Malloc1(size);
}

void OS_free(void* ptr){
    Heap::GlobalHeapManager->_Xfree((void*)ptr,"void _free(void* ptr)","stdlib.cpp",160);
}

void* OS_realloc(void* ptr, size_t size){
    return Heap::GlobalHeapManager->_Xrealloc((void*)ptr, size, __PRETTY_FUNCTION__, __FILE__, __LINE__);
}

#define N 50

long _atol(const char *nptr)
{
    int c;
    long total = 0;
    int sign;
    while(isspace((int)(unsigned char)*nptr))//跳过前面的空格
        ++nptr;
    c = (int)(unsigned char)*nptr++;
    sign = c;
    if(c == '-' || c == '+')
        c = (int)(unsigned char) *nptr++;

    while(isdigit(c))
    {
        total = 10 * total + c- '0';
        c = (int)(unsigned char)*nptr++;
    }
    if(sign == '-')
        return -total;
    else
        return total;
}
#undef N

char *_itoa(int val, char *buf, unsigned radix)
{
    char   *p;
    char   *firstdig;
    char   temp;
    unsigned   digval;
    p = buf;
    if(val <0)
    {
        *p++ = '-';
        val = (unsigned long)(-(long)val);
    }
    firstdig = p;
    do{
        digval = (unsigned)(val % radix);
        val /= radix;

        if  (digval > 9)
            *p++ = (char)(digval - 10 + 'a');
        else
            *p++ = (char)(digval + '0');
    }while(val > 0);

    *p-- = '\0 ';
    do{
        temp = *p;
        *p = *firstdig;
        *firstdig = temp;
        --p;
        ++firstdig;
    }while(firstdig < p);
    return buf;
}

long _strtol(const char *str, char **endptr, int base) {
    int sign = 1;
    // Initialize result
    long result = 0;
    
    // Skip leading whitespace
    while (isspace(*str)) str++;

    // Determine sign
    if (*str == '+') {
        str++;
    } else if (*str == '-') {
        sign = -1;
        str++;
    }

    // Determine base if not specified
    if (base == 0) {
        if (*str == '0') {
            if (tolower(*(str + 1)) == 'x') {
                base = 16;
                str += 2;
            } else {
                base = 8;
                str++;
            }
        } else {
            base = 10;
        }
    }

    // Check for invalid base
    if (base < 2 || base > 36) {
        if (endptr) *endptr = (char *)str;
        return 0;
    }

    

    // Process digits
    while (isxdigit(*str)) {
        int digit;
        if (isdigit(*str)) {
            digit = *str - '0';
        } else {
            digit = toupper(*str) - 'A' + 10;
        }
        if (digit >= base) break;
        if (result > (LONG_MAX - digit) / base) {
            // Overflow
            result = sign == 1 ? LONG_MAX : LONG_MIN;
            //errno = ERANGE;
        } else {
            result = result * base + digit;
        }
        str++;
    }

    // Set endptr to point to the first character that is not part of the number
    if (endptr) *endptr = (char *)str;

    return sign * result;
}
long long _strtoll(const char *nptr, char **endptr, int base) {
    const char *s;
    long long acc, cutoff;
    int c;
    int neg, any, cutlim;

    /*
    * Skip white space and pick up leading +/- sign if any.
    * If base is 0, allow 0x for hex and 0 for octal, else
    * assume decimal; if base is already 16, allow 0x.
    */
    s = nptr;
    do {
        c = (unsigned char)*s++;
    } while (isspace(c));

    if (c == '-') {
        neg = 1;
        c = *s++;
    } else {
        neg = 0;
        if (c == '+')
        c = *s++;
    }

    if ((base == 0 || base == 16) && c == '0' && (*s == 'x' || *s == 'X')) {
        c = s[1];
        s += 2;
        base = 16;
    }

    if (base == 0)
        base = c == '0' ? 8 : 10;

    /*
    * Compute the cutoff value between legal numbers and illegal
    * numbers.  That is the largest legal value, divided by the
    * base.  An input number that is greater than this value, if
    * followed by a legal input character, is too big.  One that
    * is equal to this value may be valid or not; the limit
    * between valid and invalid numbers is then based on the last
    * digit.  For instance, if the range for long long is
    * [-9223372036854775808..9223372036854775807] and the input base
    * is 10, cutoff will be set to 922337203685477580 and cutlim to
    * either 7 (neg==0) or 8 (neg==1), meaning that if we have
    * accumulated a value > 922337203685477580, or equal but the
    * next digit is > 7 (or 8), the number is too big, and we will
    * return a range error.
    *
    * Set any if any 'digits' consumed; make it negative to indicate
    * overflow.
    */

    switch (base) {
    case 4:
        if (neg) {
        cutlim = LLONG_MIN % 4;
        cutoff = LLONG_MIN / 4;
        } else {
        cutlim = LLONG_MAX % 4;
        cutoff = LLONG_MAX / 4;
        }
        break;

    case 8:
        if (neg) {
        cutlim = LLONG_MIN % 8;
        cutoff = LLONG_MIN / 8;
        } else {
        cutlim = LLONG_MAX % 8;
        cutoff = LLONG_MAX / 8;
        }
        break;

    case 10:
        if (neg) {
        cutlim = LLONG_MIN % 10;
        cutoff = LLONG_MIN / 10;
        } else {
        cutlim = LLONG_MAX % 10;
        cutoff = LLONG_MAX / 10;
        }
        break;

    case 16:
        if (neg) {
        cutlim = LLONG_MIN % 16;
        cutoff = LLONG_MIN / 16;
        } else {
        cutlim = LLONG_MAX % 16;
        cutoff = LLONG_MAX / 16;
        }
        break;

    default:
        cutoff = neg ? LLONG_MIN : LLONG_MAX;
        cutlim = cutoff % base;
        cutoff /= base;
        break;
    }

    if (neg) {
        if (cutlim > 0) {
        cutlim -= base;
        cutoff += 1;
        }
        cutlim = -cutlim;
    }

    for (acc = 0, any = 0;; c = (unsigned char)*s++) {
        if (isdigit(c))
        c -= '0';
        else if (isalpha(c))
        c -= isupper(c) ? 'A' - 10 : 'a' - 10;
        else
        break;

        if (c >= base)
        break;

        if (any < 0)
        continue;

        if (neg) {
        if (acc < cutoff || (acc == cutoff && c > cutlim)) {
            any = -1;
            acc = LLONG_MIN;
            //errno = ERANGE;
        } else {
            any = 1;
            acc *= base;
            acc -= c;
        }
        } else {
        if (acc > cutoff || (acc == cutoff && c > cutlim)) {
            any = -1;
            acc = LLONG_MAX;
            //errno = ERANGE;
        } else {
            any = 1;
            acc *= base;
            acc += c;
        }
        }
    }

    if (endptr != 0)
        *endptr = (char *)(any ? s - 1 : nptr);

    return (acc);
}
unsigned long long _strtoull(const char *nptr, char **endptr, int base) {
    const char *s;
    unsigned long long acc, cutoff;
    int c;
    int neg, any, cutlim;

    s = nptr;
    do {
        c = (unsigned char)*s++;
    } while (isspace(c));

    if (c == '-') {
        neg = 1;
        c = *s++;
    } else {
        neg = 0;
        if (c == '+')
        c = *s++;
    }

    if ((base == 0 || base == 16) && c == '0' && (*s == 'x' || *s == 'X')) {
        c = s[1];
        s += 2;
        base = 16;
    }

    if (base == 0)
        base = c == '0' ? 8 : 10;

    switch (base) {
    case 4:
        cutoff = ULLONG_MAX / 4;
        cutlim = ULLONG_MAX % 4;
        break;

    case 8:
        cutoff = ULLONG_MAX / 8;
        cutlim = ULLONG_MAX % 8;
        break;

    case 10:
        cutoff = ULLONG_MAX / 10;
        cutlim = ULLONG_MAX % 10;
        break;

    case 16:
        cutoff = ULLONG_MAX / 16;
        cutlim = ULLONG_MAX % 16;
        break;

    default:
        cutoff = ULLONG_MAX / base;
        cutlim = ULLONG_MAX % base;
        break;
    }

    for (acc = 0, any = 0;; c = (unsigned char)*s++) {
        if (isdigit(c))
        c -= '0';
        else if (isalpha(c))
        c -= isupper(c) ? 'A' - 10 : 'a' - 10;
        else
        break;

        if (c >= base)
        break;

        if (any < 0)
        continue;

        if (acc > cutoff || (acc == cutoff && c > cutlim)) {
        any = -1;
        acc = ULLONG_MAX;
        //errno = ERANGE;
        } else {
        any = 1;
        acc *= (unsigned long long)base;
        acc += c;
        }
    }

    if (neg && any > 0)
        acc = -acc;

    if (endptr != 0)
        *endptr = (char *)(any ? s - 1 : nptr);

    return (acc);
}
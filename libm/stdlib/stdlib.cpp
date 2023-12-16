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


int atoi(char* pstr)
{
	int Ret_Integer = 0;
	int Integer_sign = 1;
	
	/*
	* 判断指针是否为空
	*/
	if(pstr == NULL)
	{
		STDIO::printf("Pointer is NULL\n");
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


double atof(const char *str)
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

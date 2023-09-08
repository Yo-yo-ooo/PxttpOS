#include "test.h"
#include <libm/syscallManager.h>
#include <libm/syscallList.h>
#include <libm/rendering/basicRenderer.h>
#include <libm/rendering/Cols.h>
#include <libm/experimental/RelocatableAllocator.h>
#include <libm/experimental/AutoFree.h>
#include <libm/cstr.h>
#include <libm/cpu.h>

//char buffer[512];

char* Strcat(char* dest, const char* source){
	if (dest == NULL || source == NULL){		//合法性校验
		return dest;
	}
	char* p = dest;			//将目的数组赋给p
	while (*p != '\0'){		//循环看大小
		p++;
	}
	while (*source != '\0'){			//注意指针的用法
		*p = *source;
		p++;			//依次加加进行连接
		source++;
	}
	*p = '\0';
	return dest;
}

int main()
{
    globalCls();
    struct cpuinfo_x86 _ci;
    _ci = GetCI();
    
    globalPrintLn("CPU Basic Information:");
    globalPrintLn(Strcat("x86_CPU Level ",to_string(_ci.cpuid_level)));
    globalPrintLn(Strcat("x86_CPU Cache Bits ",to_string(_ci.x86_cache_bits)));
    globalPrintLn(Strcat("x86_CPU Family ",to_string(_ci.x86)));
    globalPrintLn(Strcat("X86_ModID ",_ci.x86_model_id));
    programWait(3000);
    globalCls();
    globalPrintLn("Hello World");

    return 0;
}

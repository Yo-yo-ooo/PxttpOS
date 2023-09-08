#include "test.h"
#include <libm/syscallManager.h>
#include <libm/rendering/basicRenderer.h>
#include <libm/rendering/Cols.h>
#include <libm/cpu.h>

int main()
{
    //return 0;
    //while (true);


    int argc = getArgC();
    //*((char*)(uint64_t)argc) = 'A';
    char** argv = getArgV();
    ENV_DATA* env = getEnvData();



    //return 0;
    
    main2(argc, argv, env);

    return 0;
}

void Bruhus(char* yes)
{
    for (int i = 0; i < 10; i++)
    {
        yes[i] = 0xFF;
    }
}

char* Strcat(char *dst, const char *src)
{
    //assert(dst != NULL && src != NULL);
    char *temp = dst;
    while (*temp != '\0')
        temp++;
    while ((*temp++ = *src++) != '\0');

    return dst;
}

void main2(int argc, char** argv, ENV_DATA* env)
{
    //globalCls();
    //return;
    struct cpuinfo_x86 _ci;
    _ci = GetCI();
    
    globalPrintLn(Strcat("x86_CPULevel ",_ci.cpuid_level));
    globalPrintLn(Strcat("X86_ModID",_ci.x86_model_id));
    
    return;
    //*((char*)(uint64_t)argc) = 'A';
    //Bruhus((char*)env->globalFrameBuffer->BaseAddress);
    
    //while (true);
    
}

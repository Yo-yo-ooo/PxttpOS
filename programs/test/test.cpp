#include "test.h"
#include <libm/syscallManager.h>
#include <libm/rendering/basicRenderer.h>
#include <libm/rendering/Cols.h>
#include <libm/cpu.h>

int main()
{
    //return 0;
    //while (true);


    proramExit(0);

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
    
    
    return;
    //*((char*)(uint64_t)argc) = 'A';
    //Bruhus((char*)env->globalFrameBuffer->BaseAddress);
    
    //while (true);
    
}

#include "test.h"
#include <libm/syscallManager.h>
#include <libm/syscallList.h>
#include <libm/rendering/basicRenderer.h>
#include <libm/rendering/Cols.h>
#include <libm/experimental/RelocatableAllocator.h>
#include <libm/experimental/AutoFree.h>
#include <libm/cstr.h>
#include <libm/cstrTools.h>

char buffer[512];

int main()
{
    //globalCls();

    int argc = getArgC();
    char **argv = getArgV();
    ENV_DATA *env = getEnvData();

    
    // globalPrintLn("> Goofy ah Scheduler Test:");

    // programWait(2000);
    // globalPrintLn("> USER ELF");
    // for (int i = 0; i < 150; i++)
    //     launchTestElfUser();
    // programWait(1000);

    // //return 0;
    // globalPrintLn("> KERNEL ELF");
    // for (int i = 0; i < 200; i++)
    //     launchTestElfKernel();
    // programWait(1000);
    // globalPrintLn("> Test Done!");
    
    // return 0;

    // for (int i = 0; i < 10;)
    // {
    //     if (serialCanReadChar())
    //     {
    //         char c = serialReadChar();
    //         serialPrintChar(c);
    //         globalPrintChar(c);
    //         i++;
    //     }
    //     else
    //         programWait(500);//programYield();
    // }
    return 0;
}

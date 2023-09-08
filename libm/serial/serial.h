#include <libm/syscallManager.h>

namespace Serial{
    void Print(const char* str){
        serialPrint(str);
    }

    bool CanReadChar(){
        return serialCanReadChar();
    }

    void PrintChar(char chr){
        serialPrintChar(chr);
    }

    void PrintLn(const char* str){
        serialPrintLn(str);
    }

    char ReadChar(){
        serialReadChar();
    }
}

#include "keyscan.h"
#include <libm/rendering/basicRenderer.h>
extern TempRenderer *render;

char Translate(uint8_t scancode, bool uppercase)
{ 
    

    if (scancode >= 58) return 0;
    



    if (uppercase) 
        return ShiftASCIITable[scancode];

    return ASCIITable[scancode];
}

bool IsLeftShiftPressed;
bool IsRightShiftPressed;
void HandleKeyboard(uint8_t Scancode){

    switch (Scancode){
        case LeftShift:
            IsLeftShiftPressed = true;
            return;
        case LeftShift + 0x80:
            IsLeftShiftPressed = false;
            return;
        case RightShift:
            IsRightShiftPressed = true;
            return;
        case RightShift + 0x80:
            IsRightShiftPressed = false;
            return;
        case Enter:
            render->Println("");
            return;
        case Spacebar:
            render->Print(' ');
            return;
        case Backspace:
           render->ClearChar();
           return;
    }

    char ASCII = Translate(Scancode, IsLeftShiftPressed | IsRightShiftPressed);

    if (ASCII != 0){
        render->Print(ASCII);
    }

}
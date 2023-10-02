#include "keyscan.h"
#include <libm/rendering/basicRenderer.h>
extern TempRenderer *render;

char Translate(uint8_t scancode, bool uppercase)
{ 
    if (scancode == ARR_UP) return 0xad;
    if (scancode == ARR_DOWN) return 0xaf;
    if (scancode == ARR_LEFT) return 0xac;
    if (scancode == ARR_RIGHT) return 0xae;


    if (scancode == Control) return 0x80 + 0x1d;
    if (scancode == LeftShift) return 0x80 + 0x36;
    if (scancode == RightShift) return 0x80 + 0x36;
    if (scancode == LeftAlt) return 0x80 + 0x38;
    if (scancode == Backspace) return 127;
    if (scancode == Enter) return '\n';
    if (scancode == Escape) return 27;
    if (scancode == Tab) return 9;


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
            render->putChar(' ');
            return;
        case Backspace:
           render->ClearChar();
           return;
    }

    char ASCII = Translate(Scancode, IsLeftShiftPressed | IsRightShiftPressed);

    if (ASCII != 0){
        render->putChar(ASCII);
    }

}
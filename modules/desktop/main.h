#pragma once
#include <stdint.h>
#include <stddef.h>
#include <libm/syscallManager.h>
#include <libm/env/env.h>
#include <libm/rendering/basicRenderer.h>
#include <libm/rendering/Cols.h>
#include <libm/rnd/rnd.h>
#include <libm/cstr.h>
#include <libm/list/list_window.h>
#include <libm/memStuff.h>
#include <libm/list/list_basic_msg.h>




extern TempRenderer* actualScreenRenderer;
extern Framebuffer* actualScreenFramebuffer;
extern Framebuffer* mainBuffer;
extern PointerBuffer* pointerBuffer;

extern uint32_t defaultBackgroundColor;
extern Framebuffer* backgroundImage;
extern bool drawBackground;

//extern Framebuffer* taskbar;

extern List<Window*>* windows;
extern Window* activeWindow;
extern Window* currentActionWindow;

extern MPoint MousePosition;


void InitStuff();

void PrintFPS(int fps, int frameTime, int breakTime, int totalTime);

int main();

void DrawFrame();
void rein();





/*
    programSetPriority(1);
    while (true)
    {
        if (serialCanReadChar())
        {
            char c = serialReadChar();
            globalPrint("KEY> ");
            globalPrintChar(c);
            globalPrintLn("");

            if (c == '1')
                programYield();
            else if (c == '2')
                programWait(100);
            else if (c == '3')
                programSetPriority(0);
            else if (c == '4')
                programSetPriority(1);
            else if (c == '5')
                programSetPriority(2);
            else if (c == '6')
                programSetPriority(3);
            else if (c == '7')
                programSetPriority(20);
            else if (c == '8')
                programWait(1000);
        }
    }
*/
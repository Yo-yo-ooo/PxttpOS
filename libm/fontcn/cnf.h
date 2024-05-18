#pragma once
#include <stdint.h>
#include <stddef.h>
#include "../rendering/basicRenderer.h"
#include "../gui/guiStuff/components/canvas/canvasComponent.h"
#define PARSE_ERROR 0
extern unsigned char cn_lib[188416];
typedef struct UTFGB2312{
    uint16_t utf16;
    uint16_t GB2312;
};
extern UTFGB2312 utf_gb2312[6963];
int draw_cn(int x, int y, char *str, uint32_t color,GuiComponentStuff::CanvasComponent *canvas);
int UTFdcn(int x, int y,const char *str, uint32_t color,GuiComponentStuff::CanvasComponent *canvas);
uint8_t UTF8ToUnicode(uint8_t *utf8, uint32_t *unicode);
uint8_t UnicodeToUTF16(uint32_t unicode, uint16_t *utf16);

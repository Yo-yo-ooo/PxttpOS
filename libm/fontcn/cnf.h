#pragma once
#include <stdint.h>
#include <stddef.h>
#include "../rendering/basicRenderer.h"
#include "../gui/guiStuff/components/canvas/canvasComponent.h"
#define PARSE_ERROR 0
extern const unsigned char cn_lib[188416];
typedef struct UTFGB2312{
    uint16_t utf16;
    uint16_t GB2312;
};
extern const UTFGB2312 utf_gb2312[6963];
int draw_cn(int x, int y, char *str, uint32_t color,GuiComponentStuff::CanvasComponent *canvas);
int UTF8dcn(int x, int y,uint8_t *str, uint32_t color,GuiComponentStuff::CanvasComponent *canvas);

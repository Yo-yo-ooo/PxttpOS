#pragma once
#include <stdint.h>
#include <stddef.h>
#include "../rendering/basicRenderer.h"
#include "../gui/guiStuff/components/canvas/canvasComponent.h"

extern unsigned char cn_lib[188416];
int draw_cn(int x, int y, char *str, uint32_t color,GuiComponentStuff::CanvasComponent *canvas);

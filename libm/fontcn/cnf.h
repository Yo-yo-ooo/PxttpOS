#pragma once
#include <stdint.h>
#include <stddef.h>
#include "../rendering/basicRenderer.h"

extern unsigned char cn_lib[188416];
class CNR : public TempRenderer{
public:
    int draw_cn(int x, int y, char *str, uint32_t color);
private:
    void DrawPixel(int x, int y, uint32_t col, bool update);
};
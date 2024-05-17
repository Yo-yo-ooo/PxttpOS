#include "cnf.h"

void CNR::DrawPixel(int x, int y, uint32_t col){
        TempRenderer* fb = renderer->componentFrameBuffer;
        if (x >= 0 && x < fb->Width && 
            y >= 0 && y < fb->Height)
            fb->pixels[x + y * fb->Width] = col;
        if (update)
            UpdateCanvas(x, y);
}

int CNR::draw_cn(int x, int y, char *str, uint32_t color)
{
#define PIXEL(x, y) *(((uint32_t*)framebuffer->BaseAddress) + x + (y * framebuffer->Width))
    uint16_t font;
    int i, j, k;
    int offset;
    for (k = 0; str[k] != 0; k += 2)
    {
        offset = ((char)(str[k] - 0xa0 - 1) * 94 +
                  ((char)(str[k + 1] - 0xa0) - 1)) *
                 32;
        for (i = 0; i < 16; i++)
        {
            font = cn_lib[offset + i * 2] << 8 |
                   cn_lib[offset + i * 2 + 1];
            for (j = 0; j < 16; j++)
                if ((font << j) & 0x8000)
                   PIXEL((x + j), (y + i)) = color;
        }
        x += 16;
    }
    return 0;
#undef PIXEL
}
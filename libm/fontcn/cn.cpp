#include "cnf.h"

int draw_cn(int x, int y, char *str, uint32_t color,GuiComponentStuff::CanvasComponent *canvas)
{
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
                   canvas->DrawPixel((x + j), (y + i),color,true);
        }
        x += 16;
    }
    return 0;
}
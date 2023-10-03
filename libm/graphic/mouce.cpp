#include "color.h"


void init_mouse_cursor8(uint32_t *mouse)
/* マウスカーソルを準備（16x16） */
{
    //uint32_t *pixptr = (uint32_t *)render->framebuffer->BaseAddress;
    static char cursor[16][16] = {
        "*...............",
        "**..............",
        "*O*.............",
        "*OO*............",
        "*OOO*...........",
        "*OOOO*..........",
        "*OOOOO*.........",
        "*OOOOOO*........",
        "*OOOOOOO*.......",
        "*OOOO*****......",
        "*OO*O*..........",
        "*O*.*O*.........",
        "**..*O*.........",
        "*....*O*........",
        ".....*O*........",
        "......*........."
    };
    int x, y;

    for (y = 0; y < 16; y++) {
        for (x = 0; x < 16; x++) {
            if (cursor[y][x] == '*') {
                mouse[y * 16 + x]  = COL8_000000;
            }
            if (cursor[y][x] == 'O') {
                mouse[y * 16 + x]  = COL8_FFFFFF;
            }
            if (cursor[y][x] == '.') {
                mouse[y * 16 + x]  = COL8_008484;
            }
        }
    }
    return;
}
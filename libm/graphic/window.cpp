#include "window.h"
#include "../rendering/basicRenderer.h"

#define boxfill8_(vram,xize,c,x0,y0,x1,y1) vram->Clear(x0, y0, x1, y1, c)

WMWindow::WMWindow(TempRenderer *renderer, int xsize, int ysize, char *title)
{
    static char closebtn[14][16] = {
        "OOOOOOOOOOOOOOO@",
        "OQQQQQQQQQQQQQ$@",
        "OQQQQQQQQQQQQQ$@",
        "OQQQ@@QQQQ@@QQ$@",
        "OQQQQ@@QQ@@QQQ$@",
        "OQQQQQ@@@@QQQQ$@",
        "OQQQQQQ@@QQQQQ$@",
        "OQQQQQ@@@@QQQQ$@",
        "OQQQQ@@QQ@@QQQ$@",
        "OQQQ@@QQQQ@@QQ$@",
        "OQQQQQQQQQQQQQ$@",
        "OQQQQQQQQQQQQQ$@",
        "O$$$$$$$$$$$$$$@",
        "@@@@@@@@@@@@@@@@"
    };
    int x, y;
    char c;
    boxfill8_(renderer, xsize, COL8_C6C6C6, 0,         0,         xsize - 1, 0        );
    boxfill8_(renderer, xsize, COL8_FFFFFF, 1,         1,         xsize - 2, 1        );
    boxfill8_(renderer, xsize, COL8_C6C6C6, 0,         0,         0,         ysize - 1);
    boxfill8_(renderer, xsize, COL8_FFFFFF, 1,         1,         1,         ysize - 2);
    boxfill8_(renderer, xsize, COL8_848484, xsize - 2, 1,         xsize - 2, ysize - 2);
    boxfill8_(renderer, xsize, COL8_000000, xsize - 1, 0,         xsize - 1, ysize - 1);
    boxfill8_(renderer, xsize, COL8_C6C6C6, 2,         2,         xsize - 3, ysize - 3);
    boxfill8_(renderer, xsize, COL8_000084, 3,         3,         xsize - 4, 20       );
    boxfill8_(renderer, xsize, COL8_848484, 1,         ysize - 2, xsize - 2, ysize - 2);
    boxfill8_(renderer, xsize, COL8_000000, 0,         ysize - 1, xsize - 1, ysize - 1);
    //putfonts8_asc(buf, xsize, 24, 4, COL8_FFFFFF, title);
    //actualScreenRenderer->Print(title,COL8_FFFFFF);
    unsigned char*s = title;
    int x2 = 24;
    int y2 = 4;
    for (; *s != 0x00; s++) {
		renderer->putChar(*s,x2,y2);
		x2 += 8;
	}
    for (y = 0; y < 14; y++) {
        for (x = 0; x < 16; x++) {
            c = closebtn[y][x];
            if (c == '@') {
                c = COL8_000000;
            } else if (c == '$') {
                c = COL8_848484;
            } else if (c == 'Q') {
                c = COL8_C6C6C6;
            } else {
                c = COL8_FFFFFF;
            }
            //vram[(5 + y) * xsize + (xsize - 21 + x)] = c;
            uint32_t* pixPtr = (uint32_t*)renderer->BaseAddress;
            pixPtr[(5 + y) * xsize + (xsize - 21 + x)] = c;
        }
    }
    return;
}
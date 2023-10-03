#pragma once

#include <libm/rendering/Cols.h>
#include "../rendering/basicRenderer.h"

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef  DWORD  COLORREF;
#define RGB(r,g,b) ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
#define GetRValue(rgb) ((BYTE)(rgb))
#define GetGValue(rgb) ((BYTE)(((WORD)(rgb)) >> 8))
#define GetBValue(rgb) ((BYTE)((rgb)>>16))

#define boxfill8(vram,xize,c,x0,y0,x1,y1) render->Clear(x0, y0, x1, y1, c)

#define COL8_000000		0xFF000000
#define COL8_FF0000		0xFFFF0000
#define COL8_00FF00		0xFF00FF00
#define COL8_FFFF00	    0xFFFFFF00
#define COL8_0000FF		0xFF0000FF
#define COL8_FF00FF		0xFFFF00FF
#define COL8_00FFFF		0xFF00FFFF
#define COL8_FFFFFF		0xFFFFFFFF
#define COL8_C6C6C6		0xFFC6C6C6
#define COL8_840000		0xFF840000
#define COL8_008400	    0xFF008400
#define COL8_848400	    0xFF848400
#define COL8_000084	    0xFF000084
#define COL8_840084		0xFF840084
#define COL8_008484		0xFF008484
#define COL8_848484	    0xFF848484

void init_mouse_cursor8(uint32_t *mouse);
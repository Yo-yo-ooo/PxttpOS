#pragma once



typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef  DWORD  COLORREF;
#define RGB(r,g,b) ((COLORREF)(((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16)))
#define GetRValue(rgb) ((BYTE)(rgb))
#define GetGValue(rgb) ((BYTE)(((WORD)(rgb)) >> 8))
#define GetBValue(rgb) ((BYTE)((rgb)>>16))

#define boxfill8(vram,xize,c,x0,y0,x1,y1) render->Clear(x0, y0, x1, y1, c)
#define COL8_008484 RGB(0,132,132)
#define COL8_000000 RGB(0,0,0)
#define COL8_C6C6C6 RGB(0,153,153)
#define COL8_FFFFFF RGB(255,255,255)
#define COL8_848484 RGB(132,132,132)
#define COL8_000084 RGB(0,0,132)


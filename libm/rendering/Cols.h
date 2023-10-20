#pragma once

#include <stdint.h>

    static struct col
    { 
        static const uint32_t 
        white =  0xffffffff,
        silver = 0xffc0c0c0,
        gray =   0xff808080,
        bgray =  0xffC0C0C0,
        dgray =  0xff404040,
        black =  0xff000000,
        pink =   0xffFF1493,
        green =  0xff008000,
        red =    0xff800000,
        purple = 0xff800080,
        orange = 0xffFF4500,
        cyan =   0xff008080,
        yellow = 0xffFFD700,
        brown =  0xffA52A2A,
        blue =   0xff000080,
        dblue =  0xff000030,
        bred =   0xffFF0000,
        bblue =  0xff0000FF,
        bgreen = 0xff00FF00,
        tblack =  0x00000000,
        lime =    0xffC0FF00;
    } Colors;
    
    static const int colCount = 28;

    static const char* colNames[colCount] =
    {
        "white",
        "silver",
        "gray",
        "bgray",
        "dgray",
        "black",
        "pink",
        "green",
        "red",
        "purple",
        "orange",
        "cyan",
        "yellow",
        "brown",
        "blue",
        "dblue",
        "bred",
        "bblue",
        "bgreen",
        "bright red",
        "bright blue",
        "bright green",
        "dark gray",
        "bright gray",
        "dark blue",
        "transparent black",
        "tblack",
        "lime"
    };

    static uint32_t colValues[colCount] = 
    {
        Colors.white,
        Colors.silver,
        Colors.gray,
        Colors.bgray,
        Colors.dgray,
        Colors.black,
        Colors.pink,
        Colors.green,
        Colors.red,
        Colors.purple,
        Colors.orange,
        Colors.cyan,
        Colors.yellow,
        Colors.brown,
        Colors.blue,
        Colors.dblue,
        Colors.bred,
        Colors.bblue,
        Colors.bgreen,
        Colors.bred,
        Colors.bblue,
        Colors.bgreen,
        Colors.gray,
        Colors.bgray,
        Colors.dblue,
        Colors.tblack,
        Colors.tblack,
        Colors.lime
    };

    
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
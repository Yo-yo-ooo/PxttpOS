#include "cnf.h"
#include <libm/cstrTools.h>
#include <libm/cstr.h>
#include <libm/heap/heap.h>
#include <libm/heap/new.hpp>
#include <libm/heap/new2.h>
#include <libm/syscallManager.h>
#include <libm/stubs.h>
#include <libm/cstring/string.h>
#include "sthutf.h"
using namespace Heap;
static boolean isLegalUTF8(const UTF8* source, int length)
{
    UTF8 a;
    const UTF8* srcptr = NULL;

    if (NULL == source) {
        //printf("ERR, isLegalUTF8: source=%p\n", source);
        return FALSE;
    }
    srcptr = source + length;

    switch (length) {
    default:
        //printf("ERR, isLegalUTF8 1: length=%d\n", length);
        return FALSE;
        /* Everything else falls through when "TRUE"... */
    case 4:
        if ((a = (*--srcptr)) < 0x80 || a > 0xBF) {
            //printf("ERR, isLegalUTF8 2: length=%d, a=%x\n", length, a);
            return FALSE;
        }
    case 3:
        if ((a = (*--srcptr)) < 0x80 || a > 0xBF) {
            //printf("ERR, isLegalUTF8 3: length=%d, a=%x\n", length, a);
            return FALSE;
        }
    case 2:
        if ((a = (*--srcptr)) > 0xBF) {
            //printf("ERR, isLegalUTF8 4: length=%d, a=%x\n", length, a);
            return FALSE;
        }
        switch (*source)
        {
            /* no fall-through in this inner switch */
        case 0xE0:
            if (a < 0xA0) {
                //printf("ERR, isLegalUTF8 1: source=%x, a=%x\n", *source, a);
                return FALSE;
            }
            break;
        case 0xED:
            if (a > 0x9F) {
                //printf("ERR, isLegalUTF8 2: source=%x, a=%x\n", *source, a);
                return FALSE;
            }
            break;
        case 0xF0:
            if (a < 0x90) {
                //printf("ERR, isLegalUTF8 3: source=%x, a=%x\n", *source, a);
                return FALSE;
            }
            break;
        case 0xF4:
            if (a > 0x8F) {
                //printf("ERR, isLegalUTF8 4: source=%x, a=%x\n", *source, a);
                return FALSE;
            }
            break;
        default:
            if (a < 0x80) {
                //printf("ERR, isLegalUTF8 5: source=%x, a=%x\n", *source, a);
                return FALSE;
            }
        }
    case 1:
        if (*source >= 0x80 && *source < 0xC2) {
            //printf("ERR, isLegalUTF8: source=%x\n", *source);
            return FALSE;
        }
    }
    if (*source > 0xF4)
        return FALSE;
    return TRUE;
}

ConversionResult Utf8_To_Utf16(const UTF8* sourceStart, UTF16* targetStart, size_t outLen, ConversionFlags flags)
{
    ConversionResult result = conversionOK;
    const UTF8* source = sourceStart;
    UTF16* target = targetStart;
    UTF16* targetEnd = targetStart + outLen / 2;
    const UTF8* sourceEnd = NULL;

    if ((NULL == source) || (NULL == targetStart)) {
        //printf("ERR, Utf8_To_Utf16: source=%p, targetStart=%p\n", source, targetStart);
        return conversionFailed;
    }
    sourceEnd = StrLen((const char*)sourceStart) + sourceStart;

    while (*source) {
        UTF32 ch = 0;
        unsigned short extraBytesToRead = trailingBytesForUTF8[*source];
        if (source + extraBytesToRead >= sourceEnd) {
            //printf("ERR, Utf8_To_Utf16----sourceExhausted: source=%p, extraBytesToRead=%d, sourceEnd=%p\n", source, extraBytesToRead, sourceEnd);
            result = sourceExhausted;
            break;
        }
        /* Do this check whether lenient or strict */
        if (!isLegalUTF8(source, extraBytesToRead + 1)) {
            //printf("ERR, Utf8_To_Utf16----isLegalUTF8 return FALSE: source=%p, extraBytesToRead=%d\n", source, extraBytesToRead);
            result = sourceIllegal;
            break;
        }
        /*
        * The cases all fall through. See "Note A" below.
        */
        switch (extraBytesToRead) {
        case 5: ch += *source++; ch <<= 6; /* remember, illegal UTF-8 */
        case 4: ch += *source++; ch <<= 6; /* remember, illegal UTF-8 */
        case 3: ch += *source++; ch <<= 6;
        case 2: ch += *source++; ch <<= 6;
        case 1: ch += *source++; ch <<= 6;
        case 0: ch += *source++;
        }
        ch -= offsetsFromUTF8[extraBytesToRead];

        if (target >= targetEnd) {
            source -= (extraBytesToRead + 1); /* Back up source pointer! */
            //printf("ERR, Utf8_To_Utf16----target >= targetEnd: source=%p, extraBytesToRead=%d\n", source, extraBytesToRead);
            result = targetExhausted;
            break;
        }
        if (ch <= UNI_MAX_BMP) {
            /* Target is a character <= 0xFFFF */
            /* UTF-16 surrogate values are illegal in UTF-32 */
            if (ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END) {
                if (flags == strictConversion) {
                    source -= (extraBytesToRead + 1); /* return to the illegal value itself */
                    //printf("ERR, Utf8_To_Utf16----ch >= UNI_SUR_HIGH_START && ch <= UNI_SUR_LOW_END: source=%p, extraBytesToRead=%d\n", source, extraBytesToRead);
                    result = sourceIllegal;
                    break;
                }
                else {
                    *target++ = UNI_REPLACEMENT_CHAR;
                }
            }
            else {
                *target++ = (UTF16)ch; /* normal case */
            }
        }
        else if (ch > UNI_MAX_UTF16) {
            if (flags == strictConversion) {
                result = sourceIllegal;
                source -= (extraBytesToRead + 1); /* return to the start */
                //printf("ERR, Utf8_To_Utf16----ch > UNI_MAX_UTF16: source=%p, extraBytesToRead=%d\n", source, extraBytesToRead);
                break; /* Bail out; shouldn't continue */
            }
            else {
                *target++ = UNI_REPLACEMENT_CHAR;
            }
        }
        else {
            /* target is a character in range 0xFFFF - 0x10FFFF. */
            if (target + 1 >= targetEnd) {
                source -= (extraBytesToRead + 1); /* Back up source pointer! */
                //printf("ERR, Utf8_To_Utf16----target + 1 >= targetEnd: source=%p, extraBytesToRead=%d\n", source, extraBytesToRead);
                result = targetExhausted; break;
            }
            ch -= halfBase;
            *target++ = (UTF16)((ch >> halfShift) + UNI_SUR_HIGH_START);
            *target++ = (UTF16)((ch & halfMask) + UNI_SUR_LOW_START);
        }
    }
    return result;
}


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

int UTF8dcn(int x, int y,uint8_t *str, uint32_t color,GuiComponentStuff::CanvasComponent *canvas){
    
    uint16_t utf16[256] = { 0 };
    register char buf[3] = {0,0,'\0'};
    for(int i = 0;i < StrLen(str);i++){
        Utf8_To_Utf16(str,utf16,sizeof(utf16),strictConversion);
        //serialPrintLn("Here!OK!");
        for(int k = 0;k < 6963;k++){
            if(utf_gb2312[k].utf16 == utf16[i]){
                buf[0] = ((char)((utf_gb2312[k].GB2312 >> 8) & 0xff));
                buf[1] = ((char)(utf_gb2312[k].GB2312 & 0xff));
                draw_cn(x,y,buf,color,canvas);
                x+=16;
            }else{continue;}
        }
    }
    return 0;
}
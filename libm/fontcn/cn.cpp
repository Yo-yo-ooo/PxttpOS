#include "cnf.h"
#include <libm/cstrTools.h>

/*
Unicode符号范围     |        UTF-8编码方式
(十六进制)        |              （二进制）
----------------------+---------------------------------------------
0000 0000-0000 007F | 0xxxxxxx
0000 0080-0000 07FF | 110xxxxx 10xxxxxx
0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
*/
/**
 * @brief utf8编码转unicode字符集(usc4)，最大支持4字节utf8编码，(4字节以上在中日韩文中为生僻字)
 * @param *utf8 utf8变长编码字节集1~4个字节
 * @param *unicode utf8编码转unicode字符集结果，最大4个字节，返回的字节序与utf8编码序一致
 * @return length 0：utf8解码异常，others：本次utf8编码长度
 */
uint8_t UTF8ToUnicode(uint8_t *utf8, uint32_t *unicode) {
    const uint8_t lut_size = 3;
    const uint8_t length_lut[] = {2, 3, 4};
    const uint8_t range_lut[] = {0xE0, 0xF0, 0xF8};
    const uint8_t mask_lut[] = {0x1F, 0x0F, 0x07};

    uint8_t length = 0;
    char b = *(utf8 + 0);
    uint32_t i = 0;

    if(utf8 == NULL) {
        *unicode = 0;
        return PARSE_ERROR;
    }
    // utf8编码兼容ASCII编码,使用0xxxxxx 表示00~7F
    if(b < 0x80) {
        *unicode =  b;
        return 1;
    }
    // utf8不兼容ISO8859-1 ASCII拓展字符集
    // 同时最大支持编码6个字节即1111110X
    if(b < 0xC0 || b > 0xFD) {
        *unicode = 0;
        return PARSE_ERROR;
    }
    for(i = 0; i < lut_size; i++) {
        if(b < range_lut[i]) {
            *unicode = b & mask_lut[i];
            length = length_lut[i];
            break;
        }
    }
    // 超过四字节的utf8编码不进行解析
    if(length == 0) {
        *unicode = 0;
        return PARSE_ERROR;
    }
    // 取后续字节数据
    for(i = 1; i < length; i++ ) {
        b = *(utf8 + i);
        // 多字节utf8编码后续字节范围10xxxxxx~10111111
        if( b < 0x80 || b > 0xBF ) {
            break;
        }
        *unicode <<= 6;
        // 00111111
        *unicode |= (b & 0x3F);
    }
    // 长度校验
    return (i < length) ? PARSE_ERROR : length;
}

/**
 * @brief 4字节unicode(usc4)字符集转utf16编码
 * @param unicode unicode字符值
 * @param *utf16 utf16编码结果
 * @return utf16长度，(2字节)单位
 */
uint8_t UnicodeToUTF16(uint32_t unicode, uint16_t *utf16) {
    // Unicode范围 U+000~U+FFFF
    // utf16编码方式：2 Byte存储，编码后等于Unicode值
    if(unicode <= 0xFFFF) {
		if(utf16 != NULL) {
			*utf16 = (unicode & 0xFFFF);
		}
		return 1;
	}else if( unicode <= 0xEFFFF ) {
		if( utf16 != NULL ) {
		    // 高10位
		    *(utf16 + 0) = 0xD800 + (unicode >> 10) - 0x40;
            // 低10位
		    *(utf16 + 1) = 0xDC00 + (unicode &0x03FF);
		}
		return 2;
	}

    return 0;
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

int UTFdcn(int x, int y,const char *str, uint32_t color,GuiComponentStuff::CanvasComponent *canvas){

    uint32_t buffer;
    uint8_t utf8 = 0,buf = 0;
    uint16_t utf16[2] = {0};
    uint8_t bytes[2];
    for(int i = 0;i < StrLen(str);i++){
        utf8 = str[i];
        buf = UTF8ToUnicode(utf8, &buffer);
        buf = UnicodeToUTF16(buffer, utf16);
        if(utf16[1] < (uint16_t)0x5BC5){
            for(int j = 0;j < 3481;j++){
                if(utf_gb2312[j].utf16 == utf16[1]){
#define pack utf_gb2312[j].GB2312
                    bytes[0] = (uint8_t)((pack >> 24) & 0xFF);
                    bytes[1] = (uint8_t)((pack >> 16) & 0xFF);
                    draw_cn(x, y, bytes, color, canvas);
#undef pack
                }
            }
        }else if(utf16[1] > (uint16_t)0x5BC5){
            for(int k = 3481;k < 6963;k++){
                if(utf_gb2312[k].utf16 == utf16[1]){
#define pack utf_gb2312[k].GB2312
                    bytes[0] = (uint8_t)((pack >> 24) & 0xFF);
                    bytes[1] = (uint8_t)((pack >> 16) & 0xFF);
                    draw_cn(x, y, bytes, color, canvas);
#undef pack
                }
            }
        }else{
            draw_cn(x, y, "\0xD2FA", color, canvas);
        }
    }
    return 0;
}
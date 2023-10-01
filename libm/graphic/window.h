#include <stddef.h>
#include <stdint.h>
#include "color.h"
#include "../rendering/basicRenderer.h"

class WMWindow
{
private:
    /* data */
    unsigned char* buf;//无用的
public:
    WMWindow(TempRenderer *renderer, int xsize, int ysize, char *title);
};


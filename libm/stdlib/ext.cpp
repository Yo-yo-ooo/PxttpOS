#include "ext.h"
double copysign(double x, double y) {
    union {
        double f;
        uint64_t i;
    } ux = {x}, uy = {y};
    ux.i &= -1ULL / 2;
    ux.i |= uy.i & 1ULL << 63;
    return ux.f;
}
float copysignf(float x, float y) {
    union {
        float f;
        uint32_t i;
    } ux = {x}, uy = {y};
    ux.i &= 0x7fffffff;
    ux.i |= uy.i & 0x80000000;
    return ux.f;
}
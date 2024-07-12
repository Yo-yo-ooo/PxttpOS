#pragma once
#include <stdint.h>

#include "extmath.h"
double copysign(double x, double y);
float copysignf(float x, float y);

void _printf(const char* str, ...);
void _initStdio(int needLoggerWindow);
#pragma once

#include <limits.h>

#define E 2.71828182845904523536028747135266250        /* e */
#define LN1_25 0.22314355131420976485756568763463292   /* ln(2) */
#define LN2 0.69314718055994528622676398299518041      /* ln(2) */
#define LN10 2.30258509299404590109361379290930927     /* ln(10) */
#define PI 3.14159265358979323846264338327950288       /* pi */
#define PI_2 1.57079632679489661923132169163975144     /* pi/2 */
#define PI_4 0.78539816339744830961566084581987572     /* pi/4 */
#define ATAN0_5 0.46364760900080614902663000975735485  /* arctg(0.5) */
#define ATAN1 0.78539816339744827899949086713604629    /* arctg(1) */
#define ATAN1_5 0.98279372324732905408239957978366874  /* arctg(1.5) */
#define INF 1.0 / 0.0
#define NAN 0.0 / 0.0

typedef long double (*core_func)(double x);
typedef int (*taylor_deriv)(int i);

long int abs(int x);
long double acos(double x);
long double asin(double x);
long double atan(double x);
long double ceil(double x);
long double cos(double x);
long double exp(double x);
long double fabs(double x);
long double floor(double x);
long double fmod(double x, double y);
long double log(double x);
long double pow(double base, double exp);
long double sin(double x);
long double sqrt(double x);
long double tan(double x);
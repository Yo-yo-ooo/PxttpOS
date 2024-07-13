#include "extmath.h"
#include "../cstring/string.h"
#include "../math.h"

int __rem_pio2_large(double* x, double* y, int e0, int nx, int prec);
int __rem_pio2f(float x, double *y);
float __sindf(double x);
float __cosdf(double x);

#define ln(x)       Ln(x)
#define modf(x,y)   Modf(x,y)
#define floor(x)    Floor(x)
#define scalbn(x,y)   Scalbn(x,y)
#define cosf(x)     Cosf(x)
#define sqrt(x)     Sqrt(x)
#define atan2(x,y)    Atan2(x,y)
#define sinf(x)     Sinf(x)

#define EPS (2.22044604925031308085e-16)
#define accuracy 0.000001
#define pai 3.1415926

int
__fpclassifyf(float f)
{
	struct ieee_single *p = (struct ieee_single *)&f;

	if (p->sng_exp == 0) {
		if (p->sng_frac == 0)
			return FP_ZERO;
		else
			return FP_SUBNORMAL;
	}

	if (p->sng_exp == SNG_EXP_INFNAN) {
		if (p->sng_frac == 0)
			return FP_INFINITE;
		else
			return FP_NAN;
	}

	return FP_NORMAL;
}

int
__fpclassify(double d)
{
	struct ieee_double *p = (struct ieee_double *)&d;

	if (p->dbl_exp == 0) {
		if (p->dbl_frach == 0 && p->dbl_fracl == 0)
			return FP_ZERO;
		else
			return FP_SUBNORMAL;
	}

	if (p->dbl_exp == DBL_EXP_INFNAN) {
		if (p->dbl_frach == 0 && p->dbl_fracl == 0)
			return FP_INFINITE;
		else
			return FP_NAN;
	}

	return FP_NORMAL;
}

static const double s1pio2 = 1 * M_PI_2, /* 0x3FF921FB, 0x54442D18 */
    s2pio2 = 2 * M_PI_2,                 /* 0x400921FB, 0x54442D18 */
    s3pio2 = 3 * M_PI_2,                 /* 0x4012D97C, 0x7F3321D2 */
    s4pio2 = 4 * M_PI_2;                 /* 0x401921FB, 0x54442D18 */
//#define EPS (2.22044604925031308085e-16)
float intpower(float a, int n) {  //非负整数次方
    if (n == 0) {
        return 1;
    } else {
        int i;
        float s = 1;
        for (i = 0; i < n; i++) {
        s = s * a;
        }
        return s;
    }
}

float Ln(float x) {  //自然对数
    float s = 0;
    int E = 50;  //精度
    if (x < 1) {
        int i, sygn = 1;
        for (i = 1; i < E + 1; i++) {
        s = s + sygn * intpower(x - 1, i) / i;
        sygn = -sygn;
        }
    } else {
        x = 1 / x;
        int i, sygn = 1;
        for (i = 1; i < E + 1; i++) {
        s = s + sygn * intpower(x - 1, i) / i;
        sygn = -sygn;
        }
        s = -s;
    }

    return s;
}
int fact(int n) {
    int s = 1;
    if (n == 0) {
        return 1;
    } else {
        int i;
        for (i = 1; i <= n; i++) {
        s = s * i;
        }
        return s;
    }
}
static const double toint   = 1.5/EPS;
static const double pio4    = 0x1.921fb6p-1;
static const double invpio2 = 6.36619772367581382433e-01; /* 0x3FE45F30, 0x6DC9C883 */
static const double pio2_1  = 1.57079631090164184570e+00; /* 0x3FF921FB, 0x50000000 */
static const double pio2_1t = 1.58932547735281966916e-08; /* 0x3E5110b4, 0x611A6263 */

typedef double double_t;

double Sqrt(double x) {
    double res;
    __asm__("fsqrt" : "=t"(res) : "0"(x));
    return res;
}

double Fabs(double x)
{
	int high;
	GET_HIGH_WORD(high,x);
	SET_HIGH_WORD(x,high&0x7fffffff);
    return x;
}
#define fabs(x) Fabs(x)

double Modf(double x, double *iptr) {
    union {
        double f;
        uint64_t i;
    } u = {x};
    uint64_t mask;
    int e = (int)(u.i >> 52 & 0x7ff) - 0x3ff;

    /* no fractional part */
    if (e >= 52) {
        *iptr = x;
        if (e == 0x400 && u.i << 12 != 0) /* nan */
        return x;
        u.i &= 1ULL << 63;
        return u.f;
    }

    /* no integral part*/
    if (e < 0) {
        u.i &= 1ULL << 63;
        *iptr = u.f;
        return x;
    }

    mask = -1ULL >> 12 >> e;
    if ((u.i & mask) == 0) {
        *iptr = x;
        u.i &= 1ULL << 63;
        return u.f;
    }
    u.i &= ~mask;
    *iptr = u.f;
    return x - u.f;
}
double Floor(double x) {
    int flag = 1;
    if (fabs(x) == x) {
        flag = 0;
    }
    if (flag) {
        double r = (x); // 如果小于，则返回x-1的整数部分
        double s;
        modf(r, &s);
        return s - 1;
    } else {
        double r = (x); // 如果大于等于，则返回x的整数部分
        double s;
        modf(r, &s);
        return s;
    }
}
double Scalbn(double x, int n) {
    union {
        double f;
        uint64_t i;
    } u;
    double_t y = x;

    if (n > 1023) {
        y *= 0x1p1023;
        n -= 1023;
        if (n > 1023) {
        y *= 0x1p1023;
        n -= 1023;
        if (n > 1023)
            n = 1023;
        }
    } else if (n < -1022) {
        y *= 0x1p-1022;
        n += 1022;
        if (n < -1022) {
        y *= 0x1p-1022;
        n += 1022;
        if (n < -1022)
            n = -1022;
        }
    }
    u.i = (uint64_t)(0x3ff + n) << 52;
    x = y * u.f;
    return x;
}

static const int init_jk[] = {2,3,4,6}; /* initial value for jk */

static const int32_t ipio2[] = {
    0xA2F983, 0x6E4E44, 0x1529FC, 0x2757D1, 0xF534DD, 0xC0DB62, 0x95993C,
    0x439041, 0xFE5163, 0xABDEBB, 0xC561B7, 0x246E3A, 0x424DD2, 0xE00649,
    0x2EEA09, 0xD1921C, 0xFE1DEB, 0x1CB129, 0xA73EE8, 0x8235F5, 0x2EBB44,
    0x84E99C, 0x7026B4, 0x5F7E41, 0x3991D6, 0x398353, 0x39F49C, 0x845F8B,
    0xBDF928, 0x3B1FF8, 0x97FFDE, 0x05980F, 0xEF2F11, 0x8B5A0A, 0x6D1F6D,
    0x367ECF, 0x27CB09, 0xB74F46, 0x3F669E, 0x5FEA2D, 0x7527BA, 0xC7EBE5,
    0xF17B3D, 0x0739F7, 0x8A5292, 0xEA6BFB, 0x5FB11F, 0x8D5D08, 0x560330,
    0x46FC7B, 0x6BABF0, 0xCFBC20, 0x9AF436, 0x1DA9E3, 0x91615E, 0xE61B08,
    0x659985, 0x5F14A0, 0x68408D, 0xFFD880, 0x4D7327, 0x310606, 0x1556CA,
    0x73A8C9, 0x60E27B, 0xC08C6B,

#if 0
0x47C419, 0xC367CD, 0xDCE809, 0x2A8359, 0xC4768B, 0x961CA6,
0xDDAF44, 0xD15719, 0x053EA5, 0xFF0705, 0x3F7E33, 0xE832C2,
0xDE4F98, 0x327DBB, 0xC33D26, 0xEF6B1E, 0x5EF89F, 0x3A1F35,
0xCAF27F, 0x1D87F1, 0x21907C, 0x7C246A, 0xFA6ED5, 0x772D30,
0x433B15, 0xC614B5, 0x9D19C3, 0xC2C4AD, 0x414D2C, 0x5D000C,
0x467D86, 0x2D71E3, 0x9AC69B, 0x006233, 0x7CD2B4, 0x97A7B4,
0xD55537, 0xF63ED7, 0x1810A3, 0xFC764D, 0x2A9D64, 0xABD770,
0xF87C63, 0x57B07A, 0xE71517, 0x5649C0, 0xD9D63B, 0x3884A7,
0xCB2324, 0x778AD6, 0x23545A, 0xB91F00, 0x1B0AF1, 0xDFCE19,
0xFF319F, 0x6A1E66, 0x615799, 0x47FBAC, 0xD87F7E, 0xB76522,
0x89E832, 0x60BFE6, 0xCDC4EF, 0x09366C, 0xD43F5D, 0xD7DE16,
0xDE3B58, 0x929BDE, 0x2822D2, 0xE88628, 0x4D58E2, 0x32CAC6,
0x16E308, 0xCB7DE0, 0x50C017, 0xA71DF3, 0x5BE018, 0x34132E,
0x621283, 0x014883, 0x5B8EF5, 0x7FB0AD, 0xF2E91E, 0x434A48,
0xD36710, 0xD8DDAA, 0x425FAE, 0xCE616A, 0xA4280A, 0xB499D3,
0xF2A606, 0x7F775C, 0x83C2A3, 0x883C61, 0x78738A, 0x5A8CAF,
0xBDD76F, 0x63A62D, 0xCBBFF4, 0xEF818D, 0x67C126, 0x45CA55,
0x36D9CA, 0xD2A828, 0x8D61C2, 0x77C912, 0x142604, 0x9B4612,
0xC459C4, 0x44C5C8, 0x91B24D, 0xF31700, 0xAD43D4, 0xE54929,
0x10D5FD, 0xFCBE00, 0xCC941E, 0xEECE70, 0xF53E13, 0x80F1EC,
0xC3E7B3, 0x28F8C7, 0x940593, 0x3E71C1, 0xB3092E, 0xF3450B,
0x9C1288, 0x7B20AB, 0x9FB52E, 0xC29247, 0x2F327B, 0x6D550C,
0x90A772, 0x1FE76B, 0x96CB31, 0x4A1679, 0xE27941, 0x89DFF4,
0x9794E8, 0x84E6E2, 0x973199, 0x6BED88, 0x365F5F, 0x0EFDBB,
0xB49A48, 0x6CA467, 0x427271, 0x325D8D, 0xB8159F, 0x09E5BC,
0x25318D, 0x3974F7, 0x1C0530, 0x010C0D, 0x68084B, 0x58EE2C,
0x90AA47, 0x02E774, 0x24D6BD, 0xA67DF7, 0x72486E, 0xEF169F,
0xA6948E, 0xF691B4, 0x5153D1, 0xF20ACF, 0x339820, 0x7E4BF5,
0x6863B2, 0x5F3EDD, 0x035D40, 0x7F8985, 0x295255, 0xC06437,
0x10D86D, 0x324832, 0x754C5B, 0xD4714E, 0x6E5445, 0xC1090B,
0x69F52A, 0xD56614, 0x9D0727, 0x50045D, 0xDB3BB4, 0xC576EA,
0x17F987, 0x7D6B49, 0xBA271D, 0x296996, 0xACCCC6, 0x5414AD,
0x6AE290, 0x89D988, 0x50722C, 0xBEA404, 0x940777, 0x7030F3,
0x27FC00, 0xA871EA, 0x49C266, 0x3DE064, 0x83DD97, 0x973FA3,
0xFD9443, 0x8C860D, 0xDE4131, 0x9D3992, 0x8C70DD, 0xE7B717,
0x3BDF08, 0x2B3715, 0xA0805C, 0x93805A, 0x921110, 0xD8E80F,
0xAF806C, 0x4BFFDB, 0x0F9038, 0x761859, 0x15A562, 0xBBCB61,
0xB989C7, 0xBD4010, 0x04F2D2, 0x277549, 0xF6B6EB, 0xBB22DB,
0xAA140A, 0x2F2689, 0x768364, 0x333B09, 0x1A940E, 0xAA3A51,
0xC2A31D, 0xAEEDAF, 0x12265C, 0x4DC26D, 0x9C7A2D, 0x9756C0,
0x833F03, 0xF6F009, 0x8C402B, 0x99316D, 0x07B439, 0x15200C,
0x5BC3D8, 0xC492F5, 0x4BADC6, 0xA5CA4E, 0xCD37A7, 0x36A9E6,
0x9492AB, 0x6842DD, 0xDE6319, 0xEF8C76, 0x528B68, 0x37DBFC,
0xABA1AE, 0x3115DF, 0xA1AE00, 0xDAFB0C, 0x664D64, 0xB705ED,
0x306529, 0xBF5657, 0x3AFF47, 0xB9F96A, 0xF3BE75, 0xDF9328,
0x3080AB, 0xF68C66, 0x15CB04, 0x0622FA, 0x1DE4D9, 0xA4B33D,
0x8F1B57, 0x09CD36, 0xE9424E, 0xA4BE13, 0xB52333, 0x1AAAF0,
0xA8654F, 0xA5C1D2, 0x0F3F0B, 0xCD785B, 0x76F923, 0x048B7B,
0x721789, 0x53A6C6, 0xE26E6F, 0x00EBEF, 0x584A9B, 0xB7DAC4,
0xBA66AA, 0xCFCF76, 0x1D02D1, 0x2DF1B1, 0xC1998C, 0x77ADC3,
0xDA4886, 0xA05DF7, 0xF480C6, 0x2FF0AC, 0x9AECDD, 0xBC5C3F,
0x6DDED0, 0x1FC790, 0xB6DB2A, 0x3A25A3, 0x9AAF00, 0x9353AD,
0x0457B6, 0xB42D29, 0x7E804B, 0xA707DA, 0x0EAA76, 0xA1597B,
0x2A1216, 0x2DB7DC, 0xFDE5FA, 0xFEDB89, 0xFDBE89, 0x6C76E4,
0xFCA906, 0x70803E, 0x156E85, 0xFF87FD, 0x073E28, 0x336761,
0x86182A, 0xEABD4D, 0xAFE7B3, 0x6E6D8F, 0x396795, 0x5BBF31,
0x48D784, 0x16DF30, 0x432DC7, 0x356125, 0xCE70C9, 0xB8CB30,
0xFD6CBF, 0xA200A4, 0xE46C05, 0xA0DD5A, 0x476F21, 0xD21262,
0x845CB9, 0x496170, 0xE0566B, 0x015299, 0x375550, 0xB7D51E,
0xC4F133, 0x5F6E13, 0xE4305D, 0xA92E85, 0xC3B21D, 0x3632A1,
0xA4B708, 0xD4B1EA, 0x21F716, 0xE4698F, 0x77FF27, 0x80030C,
0x2D408D, 0xA0CD4F, 0x99A520, 0xD3A2B3, 0x0A5D2F, 0x42F9B4,
0xCBDA11, 0xD0BE7D, 0xC1DB9B, 0xBD17AB, 0x81A2CA, 0x5C6A08,
0x17552E, 0x550027, 0xF0147F, 0x8607E1, 0x640B14, 0x8D4196,
0xDEBE87, 0x2AFDDA, 0xB6256B, 0x34897B, 0xFEF305, 0x9EBFB9,
0x4F6A68, 0xA82A4A, 0x5AC44F, 0xBCF82D, 0x985AD7, 0x95C7F4,
0x8D4D0D, 0xA63A20, 0x5F57A4, 0xB13F14, 0x953880, 0x0120CC,
0x86DD71, 0xB6DEC9, 0xF560BF, 0x11654D, 0x6B0701, 0xACB08C,
0xD0C0B2, 0x485551, 0x0EFB1E, 0xC37295, 0x3B06A3, 0x3540C0,
0x7BDC06, 0xCC45E0, 0xFA294E, 0xC8CAD6, 0x41F3E8, 0xDE647C,
0xD8649B, 0x31BED9, 0xC397A4, 0xD45877, 0xC5E369, 0x13DAF0,
0x3C3ABA, 0x461846, 0x5F7555, 0xF5BDD2, 0xC6926E, 0x5D2EAC,
0xED440E, 0x423E1C, 0x87C461, 0xE9FD29, 0xF3D6E7, 0xCA7C22,
0x35916F, 0xC5E008, 0x8DD7FF, 0xE26A6E, 0xC6FDB0, 0xC10893,
0x745D7C, 0xB2AD6B, 0x9D6ECD, 0x7B723E, 0x6A11C6, 0xA9CFF7,
0xDF7329, 0xBAC9B5, 0x5100B7, 0x0DB2E2, 0x24BA74, 0x607DE5,
0x8AD874, 0x2C150D, 0x0C1881, 0x94667E, 0x162901, 0x767A9F,
0xBEFDFD, 0xEF4556, 0x367ED9, 0x13D9EC, 0xB9BA8B, 0xFC97C4,
0x27A831, 0xC36EF1, 0x36C594, 0x56A8D8, 0xB5A8B4, 0x0ECCCF,
0x2D8912, 0x34576F, 0x89562C, 0xE3CE99, 0xB920D6, 0xAA5E6B,
0x9C2A3E, 0xCC5F11, 0x4A0BFD, 0xFBF4E1, 0x6D3B8E, 0x2C86E2,
0x84D4E9, 0xA9B4FC, 0xD1EEEF, 0xC9352E, 0x61392F, 0x442138,
0xC8D91B, 0x0AFC81, 0x6A4AFB, 0xD81C2F, 0x84B453, 0x8C994E,
0xCC2254, 0xDC552A, 0xD6C6C0, 0x96190B, 0xB8701A, 0x649569,
0x605A26, 0xEE523F, 0x0F117F, 0x11B5F4, 0xF5CBFC, 0x2DBC34,
0xEEBC34, 0xCC5DE8, 0x605EDD, 0x9B8E67, 0xEF3392, 0xB817C9,
0x9B5861, 0xBC57E1, 0xC68351, 0x103ED8, 0x4871DD, 0xDD1C2D,
0xA118AF, 0x462C21, 0xD7F359, 0x987AD9, 0xC0549E, 0xFA864F,
0xFC0656, 0xAE79E5, 0x362289, 0x22AD38, 0xDC9367, 0xAAE855,
0x382682, 0x9BE7CA, 0xA40D51, 0xB13399, 0x0ED7A9, 0x480569,
0xF0B265, 0xA7887F, 0x974C88, 0x36D1F9, 0xB39221, 0x4A827B,
0x21CF98, 0xDC9F40, 0x5547DC, 0x3A74E1, 0x42EB67, 0xDF9DFE,
0x5FD45E, 0xA4677B, 0x7AACBA, 0xA2F655, 0x23882B, 0x55BA41,
0x086E59, 0x862A21, 0x834739, 0xE6E389, 0xD49EE5, 0x40FB49,
0xE956FF, 0xCA0F1C, 0x8A59C5, 0x2BFA94, 0xC5C1D3, 0xCFC50F,
0xAE5ADB, 0x86C547, 0x624385, 0x3B8621, 0x94792C, 0x876110,
0x7B4C2A, 0x1A2C80, 0x12BF43, 0x902688, 0x893C78, 0xE4C4A8,
0x7BDBE5, 0xC23AC4, 0xEAF426, 0x8A67F7, 0xBF920D, 0x2BA365,
0xB1933D, 0x0B7CBD, 0xDC51A4, 0x63DD27, 0xDDE169, 0x19949A,
0x9529A8, 0x28CE68, 0xB4ED09, 0x209F44, 0xCA984E, 0x638270,
0x237C7E, 0x32B90F, 0x8EF5A7, 0xE75614, 0x08F121, 0x2A9DB5,
0x4D7E6F, 0x5119A5, 0xABF9B5, 0xD6DF82, 0x61DD96, 0x023616,
0x9F3AC4, 0xA1A283, 0x6DED72, 0x7A8D39, 0xA9B882, 0x5C326B,
0x5B2746, 0xED3400, 0x7700D2, 0x55F4FC, 0x4D5901, 0x8071E0,
#endif
};

static const double PIo2[] = {
    1.57079625129699707031e+00, /* 0x3FF921FB, 0x40000000 */
    7.54978941586159635335e-08, /* 0x3E74442D, 0x00000000 */
    5.39030252995776476554e-15, /* 0x3CF84698, 0x80000000 */
    3.28200341580791294123e-22, /* 0x3B78CC51, 0x60000000 */
    1.27065575308067607349e-29, /* 0x39F01B83, 0x80000000 */
    1.22933308981111328932e-36, /* 0x387A2520, 0x40000000 */
    2.73370053816464559624e-44, /* 0x36E38222, 0x80000000 */
    2.16741683877804819444e-51, /* 0x3569F31D, 0x00000000 */
};
int __rem_pio2_large(double* x, double* y, int e0, int nx, int prec) {
    int32_t jz, jx, jv, jp, jk, carry, n, iq[20], i, j, k, m, q0, ih;
    double z, fw, f[20], fq[20], q[20];

    /* initialize jk*/
    jk = init_jk[prec];
    jp = jk;

    /* determine jx,jv,q0, note that 3>q0 */
    jx = nx - 1;
    jv = (e0 - 3) / 24;
    if (jv < 0)
        jv = 0;
    q0 = e0 - 24 * (jv + 1);

    /* set up f[0] to f[jx+jk] where f[jx+jk] = ipio2[jv+jk] */
    j = jv - jx;
    m = jx + jk;
    for (i = 0; i <= m; i++, j++)
        f[i] = j < 0 ? 0.0 : (double)ipio2[j];

    /* compute q[0],q[1],...q[jk] */
    for (i = 0; i <= jk; i++) {
        for (j = 0, fw = 0.0; j <= jx; j++)
        fw += x[j] * f[jx + i - j];
        q[i] = fw;
    }

    jz = jk;
recompute:
    /* distill q[] into iq[] reversingly */
    for (i = 0, j = jz, z = q[jz]; j > 0; i++, j--) {
        fw = (double)(int32_t)(0x1p-24 * z);
        iq[i] = (int32_t)(z - 0x1p24 * fw);
        z = q[j - 1] + fw;
    }

    /* compute n */
    z = scalbn(z, q0);           /* actual value of z */
    z -= 8.0 * floor(z * 0.125); /* trim off integer >= 8 */
    n = (int32_t)z;
    z -= (double)n;
    ih = 0;
    if (q0 > 0) { /* need iq[jz-1] to determine n */
        i = iq[jz - 1] >> (24 - q0);
        n += i;
        iq[jz - 1] -= i << (24 - q0);
        ih = iq[jz - 1] >> (23 - q0);
    } else if (q0 == 0)
        ih = iq[jz - 1] >> 23;
    else if (z >= 0.5)
        ih = 2;

    if (ih > 0) { /* q > 0.5 */
        n += 1;
        carry = 0;
        for (i = 0; i < jz; i++) { /* compute 1-q */
        j = iq[i];
        if (carry == 0) {
            if (j != 0) {
            carry = 1;
            iq[i] = 0x1000000 - j;
            }
        } else
            iq[i] = 0xffffff - j;
        }
        if (q0 > 0) { /* rare case: chance is 1 in 12 */
        switch (q0) {
            case 1:
            iq[jz - 1] &= 0x7fffff;
            break;
            case 2:
            iq[jz - 1] &= 0x3fffff;
            break;
        }
        }
        if (ih == 2) {
        z = 1.0 - z;
        if (carry != 0)
            z -= scalbn(1.0, q0);
        }
    }

    /* check if recomputation is needed */
    if (z == 0.0) {
        j = 0;
        for (i = jz - 1; i >= jk; i--)
        j |= iq[i];
        if (j == 0) { /* need recomputation */
        for (k = 1; iq[jk - k] == 0; k++)
            ; /* k = no. of terms needed */

        for (i = jz + 1; i <= jz + k; i++) { /* add q[jz+1] to q[jz+k] */
            f[jx + i] = (double)ipio2[jv + i];
            for (j = 0, fw = 0.0; j <= jx; j++)
            fw += x[j] * f[jx + i - j];
            q[i] = fw;
        }
        jz += k;
        goto recompute;
        }
    }

    /* chop off zero terms */
    if (z == 0.0) {
        jz -= 1;
        q0 -= 24;
        while (iq[jz] == 0) {
        jz--;
        q0 -= 24;
        }
    } else { /* break z into 24-bit if necessary */
        z = scalbn(z, -q0);
        if (z >= 0x1p24) {
        fw = (double)(int32_t)(0x1p-24 * z);
        iq[jz] = (int32_t)(z - 0x1p24 * fw);
        jz += 1;
        q0 += 24;
        iq[jz] = (int32_t)fw;
        } else
        iq[jz] = (int32_t)z;
    }

    /* convert integer "bit" chunk to floating-point value */
    fw = scalbn(1.0, q0);
    for (i = jz; i >= 0; i--) {
        q[i] = fw * (double)iq[i];
        fw *= 0x1p-24;
    }

    /* compute PIo2[0,...,jp]*q[jz,...,0] */
    for (i = jz; i >= 0; i--) {
        for (fw = 0.0, k = 0; k <= jp && k <= jz - i; k++)
        fw += PIo2[k] * q[i + k];
        fq[jz - i] = fw;
    }

    /* compress fq[] into y[] */
    switch (prec) {
        case 0:
        fw = 0.0;
        for (i = jz; i >= 0; i--)
            fw += fq[i];
        y[0] = ih == 0 ? fw : -fw;
        break;
        case 1:
        case 2:
        fw = 0.0;
        for (i = jz; i >= 0; i--)
            fw += fq[i];
        fw = (double)fw;
        y[0] = ih == 0 ? fw : -fw;
        fw = fq[0] - fw;
        for (i = 1; i <= jz; i++)
            fw += fq[i];
        y[1] = ih == 0 ? fw : -fw;
        break;
        case 3: /* painful */
        for (i = jz; i > 0; i--) {
            fw = fq[i - 1] + fq[i];
            fq[i] += fq[i - 1] - fw;
            fq[i - 1] = fw;
        }
        for (i = jz; i > 1; i--) {
            fw = fq[i - 1] + fq[i];
            fq[i] += fq[i - 1] - fw;
            fq[i - 1] = fw;
        }
        for (fw = 0.0, i = jz; i >= 2; i--)
            fw += fq[i];
        if (ih == 0) {
            y[0] = fq[0];
            y[1] = fq[1];
            y[2] = fw;
        } else {
            y[0] = -fq[0];
            y[1] = -fq[1];
            y[2] = -fw;
        }
    }
    return n & 7;
}

int __rem_pio2f(float x, double *y)
{
	union {float f; uint32_t i;} u = {x};
	double tx[1],ty[1];
	double_t fn;
	uint32_t ix;
	int n, sign, e0;

	ix = u.i & 0x7fffffff;
	/* 25+53 bit pi is good enough for medium size */
	if (ix < 0x4dc90fdb) {  /* |x| ~< 2^28*(pi/2), medium size */
		/* Use a specialized rint() to get fn. */
		fn = (double_t)x*invpio2 + toint - toint;
		n  = (int32_t)fn;
		*y = x - fn*pio2_1 - fn*pio2_1t;
		/* Matters with directed rounding. */
		if (predict_false(*y < -pio4)) {
			n--;
			fn--;
			*y = x - fn*pio2_1 - fn*pio2_1t;
		} else if (predict_false(*y > pio4)) {
			n++;
			fn++;
			*y = x - fn*pio2_1 - fn*pio2_1t;
		}
		return n;
	}
	if(ix>=0x7f800000) {  /* x is inf or NaN */
		*y = x-x;
		return 0;
	}
	/* scale x into [2^23, 2^24-1] */
	sign = u.i>>31;
	e0 = (ix>>23) - (0x7f+23);  /* e0 = ilogb(|x|)-23, positive */
	u.i = ix - (e0<<23);
	tx[0] = u.f;
	n  =  __rem_pio2_large(tx,ty,e0,1,0);
	if (sign) {
		*y = -ty[0];
		return -n;
	}
	*y = ty[0];
	return n;
}


static const double c1pio2 = 1 * M_PI_2, /* 0x3FF921FB, 0x54442D18 */
    c2pio2 = 2 * M_PI_2,                 /* 0x400921FB, 0x54442D18 */
    c3pio2 = 3 * M_PI_2,                 /* 0x4012D97C, 0x7F3321D2 */
    c4pio2 = 4 * M_PI_2;                 /* 0x401921FB, 0x54442D18 */

static const double S1 = -0x15555554cbac77.0p-55, /* -0.166666666416265235595 */
    S2 = 0x111110896efbb2.0p-59,  /*  0.0083333293858894631756 */
    S3 = -0x1a00f9e2cae774.0p-65, /* -0.000198393348360966317347 */
    S4 = 0x16cd878c3b46a7.0p-71;  /*  0.0000027183114939898219064 */

float __sindf(double x) {
  double_t r, s, w, z;

  /* Try to optimize for parallel evaluation as in __tandf.c. */
  z = x * x;
  w = z * z;
  r = S3 + z * S4;
  s = z * x;
  return (x + s * (S1 + z * S2)) + s * w * r;
}
static const double C0 = -0x1ffffffd0c5e81.0p-54, /* -0.499999997251031003120 */
    C1 = 0x155553e1053a42.0p-57,  /*  0.0416666233237390631894 */
    C2 = -0x16c087e80f1e27.0p-62, /* -0.00138867637746099294692 */
    C3 = 0x199342e0ee5069.0p-68;  /*  0.0000243904487962774090654 */

float __cosdf(double x) {
    double_t r, w, z;

    /* Try to optimize for parallel evaluation as in __tandf.c. */
    z = x * x;
    w = z * z;
    r = C2 + z * C3;
    return ((1.0 + z * C0) + w * C1) + (w * z) * r;
}
float Cosf(float x) {
    double y;
    uint32_t ix;
    unsigned n, sign;

    GET_FLOAT_WORD(ix, x);
    sign = ix >> 31;
    ix &= 0x7fffffff;

    if (ix <= 0x3f490fda) {  /* |x| ~<= pi/4 */
        if (ix < 0x39800000) { /* |x| < 2**-12 */
        /* raise inexact if x != 0 */
        FORCE_EVAL(x + 0x1p120f);
        return 1.0f;
        }
        return __cosdf(x);
    }
    if (ix <= 0x407b53d1) { /* |x| ~<= 5*pi/4 */
        if (ix > 0x4016cbe3)  /* |x|  ~> 3*pi/4 */
        return -__cosdf(sign ? x + c2pio2 : x - c2pio2);
        else {
        if (sign)
            return __sindf(x + c1pio2);
        else
            return __sindf(c1pio2 - x);
        }
    }
    if (ix <= 0x40e231d5) { /* |x| ~<= 9*pi/4 */
        if (ix > 0x40afeddf)  /* |x| ~> 7*pi/4 */
        return __cosdf(sign ? x + c4pio2 : x - c4pio2);
        else {
        if (sign)
            return __sindf(-x - c3pio2);
        else
            return __sindf(x - c3pio2);
        }
    }

    /* cos(Inf or NaN) is NaN */
    if (ix >= 0x7f800000)
        return x - x;

    /* general argument reduction needed */
    n = __rem_pio2f(x, &y);
    switch (n & 3) {
    case 0:
        return __cosdf(y);
    case 1:
        return __sindf(-y);
    case 2:
        return -__cosdf(y);
    default:
        return __sindf(y);
    }
}

static const double pi = 3.1415926535897931160E+00, /* 0x400921FB, 0x54442D18 */
    pi_lo = 1.2246467991473531772E-16;              /* 0x3CA1A626, 0x33145C07 */

double Atan2(double y, double x) {
    double z;
    uint32_t m, lx, ly, ix, iy;

    if (isnan(x) || isnan(y))
        return x + y;
    EXTRACT_WORDS(ix, lx, x);
    EXTRACT_WORDS(iy, ly, y);
    if (((ix - 0x3ff00000) | lx) == 0) /* x = 1.0 */
        return Atan(y);
    m = ((iy >> 31) & 1) | ((ix >> 30) & 2); /* 2*sign(x)+sign(y) */
    ix = ix & 0x7fffffff;
    iy = iy & 0x7fffffff;

    /* when y = 0 */
    if ((iy | ly) == 0) {
        switch (m) {
        case 0:
        case 1:
        return y; /* atan(+-0,+anything)=+-0 */
        case 2:
        return pi; /* atan(+0,-anything) = pi */
        case 3:
        return -pi; /* atan(-0,-anything) =-pi */
        }
    }
    /* when x = 0 */
    if ((ix | lx) == 0)
        return m & 1 ? -pi / 2 : pi / 2;
    /* when x is INF */
    if (ix == 0x7ff00000) {
        if (iy == 0x7ff00000) {
        switch (m) {
        case 0:
            return pi / 4; /* atan(+INF,+INF) */
        case 1:
            return -pi / 4; /* atan(-INF,+INF) */
        case 2:
            return 3 * pi / 4; /* atan(+INF,-INF) */
        case 3:
            return -3 * pi / 4; /* atan(-INF,-INF) */
        }
        } else {
        switch (m) {
        case 0:
            return 0.0; /* atan(+...,+INF) */
        case 1:
            return -0.0; /* atan(-...,+INF) */
        case 2:
            return pi; /* atan(+...,-INF) */
        case 3:
            return -pi; /* atan(-...,-INF) */
        }
        }
    }
    /* |y/x| > 0x1p64 */
    if (ix + (64 << 20) < iy || iy == 0x7ff00000)
        return m & 1 ? -pi / 2 : pi / 2;

    /* z = atan(|y/x|) without spurious underflow */
    if ((m & 2) && iy + (64 << 20) < ix) /* |y/x| < 0x1p-64, x<0 */
        z = 0;
    else
        z = Atan(fabs(y / x));
    switch (m) {
    case 0:
        return z; /* atan(+,+) */
    case 1:
        return -z; /* atan(-,+) */
    case 2:
        return pi - (z - pi_lo); /* atan(+,-) */
    default:                   /* case 3 */
        return (z - pi_lo) - pi; /* atan(-,-) */
    }
}

float Sinf(float x) {
    double y;
    uint32_t ix;
    int n, sign;

    GET_FLOAT_WORD(ix, x);
    sign = ix >> 31;
    ix &= 0x7fffffff;

    if (ix <= 0x3f490fda) {  /* |x| ~<= pi/4 */
        if (ix < 0x39800000) { /* |x| < 2**-12 */
        /* raise inexact if x!=0 and underflow if subnormal */
        FORCE_EVAL(ix < 0x00800000 ? x / 0x1p120f : x + 0x1p120f);
        return x;
        }
        return __sindf(x);
    }
    if (ix <= 0x407b53d1) {   /* |x| ~<= 5*pi/4 */
        if (ix <= 0x4016cbe3) { /* |x| ~<= 3pi/4 */
        if (sign)
            return -__cosdf(x + s1pio2);
        else
            return __cosdf(x - s1pio2);
        }
        return __sindf(sign ? -(x + s2pio2) : -(x - s2pio2));
    }
    if (ix <= 0x40e231d5) {   /* |x| ~<= 9*pi/4 */
        if (ix <= 0x40afeddf) { /* |x| ~<= 7*pi/4 */
        if (sign)
            return __cosdf(x + s3pio2);
        else
            return -__cosdf(x - s3pio2);
        }
        return __sindf(sign ? x + s4pio2 : x - s4pio2);
    }

    /* sin(Inf or NaN) is NaN */
    if (ix >= 0x7f800000)
        return x - x;

    /* general argument reduction needed */
    n = __rem_pio2f(x, &y);
    switch (n & 3) {
    case 0:
        return __sindf(y);
    case 1:
        return __cosdf(y);
    case 2:
        return __sindf(-y);
    default:
        return -__cosdf(y);
    }
}


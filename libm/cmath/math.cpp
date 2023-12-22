#include "math.h"

long int abs(int x) {
    return x >= 0 ? x : -x;
}

long double fabs(double x) {
    return x >= 0.0 ? x : -x;
}

long double floor(double x) {
    long double result = 0;
    if (x == INF) {
        result = INF;
    } else if (x == -INF) {
        result = -INF;
    } else if (x != x) {
        result = NAN;
    } else if (x >= LLONG_MAX || x <= LLONG_MIN) {
        result = x;
    } else {
        long long number = (long long)x;
        result = (long double)number;
        if (x < 0 && result != x) {
        result -= 1;
        }
    }
    return result;
}

long double ceil(double x) {
    return (floor(x) == x) ? x : floor(x) + 1;
}

long double fmod(double x, double y) {
    long double result = 0;
    if (y == 0) {
        result = NAN;
    } else if (x == INF || x == -INF) {
        result = NAN;
    } else if (x != x || y != y) {
        result = NAN;
    } else if (y == INF || y == -INF) {
        result = x;
    } else if (x == +0.0) {
        result = x;
    } else {
        if ((x / y) < 0) {
        result = x - y * (ceil(x / y));
        } else {
        result = x - y * (floor(x / y));
        }
    }
    return result;
}

long double fmax(long double x, long double y) {
    return (x > y) ? x : y;
}

long double tayl(taylor_deriv deriv_func, double x, int max_deg) {
    long double over_fact = 1.0, deg_x = 1, res = 0;
    for (int i = 0; i < max_deg; i++) {
        static long double deriv = 0;
        deriv = (double)deriv_func(i);
        over_fact /= fmax(1.0, (long double)i);
        res += deriv * deg_x * over_fact;
        deg_x *= x;
    }
    return res;
}

double trig_shift(double x) {
    double p_x = ((long double)x / (2 * PI));
    return x - 2 * PI * floor(p_x);
}

int sin_deriv(int i) {
    return (i % 2 == 0) ? 0 : (i % 4 == 1) ? 1 : -1;
}

long double sin(double x) {
    return tayl(sin_deriv, trig_shift(x), 30);
}

int cos_deriv(int i) {
    return (i % 2 == 1) ? 0 : (i % 4 == 0) ? 1 : -1;
}

long double cos(double x) {
    return tayl(cos_deriv, trig_shift(x), 30);
}

long double tan(double x) {
    return sin(x) / cos(x);
}

int exp_deriv(int i) {
    int res = 0;
    if (i >= 0)
        res = 1;
    return res;
}

long double simple_exp(double base, long long exp) {
    double res = 1.0;
    for (int i = 0; i < abs(exp); i++) {
        res = res * base;
    }
    if (exp < 0) {
        res = 1 / res;
    }
    return (long double)res;
}

long double exp(double x) {
    long double result = 0;
    long long i = (long long)x;
    if (x == INF) {
        result = INF;
    } else if (x == -INF) {
        result = 0;
    } else {
        result = simple_exp(E, i) * tayl(exp_deriv, fmod(x, 1.0), 20);
    }
    return result;
}

long double sqrt(double x) {
    long double res = 0;
    if (x >= 0.0) {
        res = pow(x, 0.5);
    } else {
        res = NAN;
    }
    return res;
}

long double log(double x) {
    long double res = 0;
    if (x != x || x < 0) {
        res = NAN;
    } else if (x == INF) {
        res = INF;
    } else if (x == +0.0) {
        res = -INF;
    } else {
        int deg10 = 0;
        while (x >= 10.0) {
            deg10++;
            x *= 0.1;
        }
        int deg2 = 0;
        while (x >= 2.0) {
            deg2++;
            x *= 0.5;
        }
        while (x <= 0.5) {
            deg2--;
            x *= 2;
        }
        int deg1_25 = 0;
        while (x >= 1.25) {
            deg1_25++;
            x *= 0.8;
        }
        while (x <= 0.8) {
            deg1_25--;
            x *= 1.25;
        }
        long double deg_x = 1.0, tayl_sign = -1.0;
        for (double i = 1.0; i < 30.0; i++) {
            deg_x *= x - 1.0;
            tayl_sign *= -1.0;
            res += tayl_sign * deg_x / i;
        }
        res += deg10 * LN10 + deg2 * LN2 + deg1_25 * LN1_25;
    }
    return res;
}

long double asin(double x) {
    long double res = 0;
    if (x == 1.0) {
        res = PI_2;
    } else if (x == -1.0) {
        res = -PI_2;
    } else if (x > -1.0 && x < 1.0) {
        res = atan(x / sqrt(1 - x * x));
    } else {
        res = NAN;
    }
    return res;
}

long double acos(double x) {
    long double res = 0;
    if (x >= -1.0 && x <= 1.0) {
        res = PI_2 - asin(x);
    } else {
        res = NAN;
    }
    return res;
}

long double atan(double x) {
    long double res = 0.0;
    if (x != x) {
        res = NAN;
    } else if (x == INF) {
        res = PI_2;
    } else if (x == -INF) {
        res = -PI_2;
    } else if (x < 0) {
        res = -atan(-x);
    } else if (x < 0.4375) {
        res = x;
        long double step = x, mult = x * x;
        for (double i = 1.0; i < 15.0; i++) {
            step *= -1.0 * mult;
            res += step / (2.0 * i + 1.0);
        }
    } else if (x < 0.6875) {
        res = ATAN0_5 + atan((x - 0.5) / (1 + x * 0.5));
    } else if (x < 1.1875) {
        res = ATAN1 + atan((x - 1) / (1 + x));
    } else if (x < 2.4375) {
        res = ATAN1_5 + atan((x - 1.5) / (1 + x * 1.5));
    } else {
        res = PI_2 + atan((-1) / x);
    }
    return res;
  }


long double pow(double base, double exp2) {
    long double res = 0.0;
    if (1.0 / base == INF && exp2 < 0.0 && fmod(exp2, 2.0) == -1.0) {
        res = INF;
    } else if (1.0 / base == -INF && exp2 < 0.0 && fmod(exp2, 2.0) == -1.0) {
        res = -INF;
    } else if ((1.0 / base == -INF || 1.0 / base == INF) && exp2 < 0.0) {
        res = INF;
    } else if (1.0 / base == INF && exp2 > 0.0 && fmod(exp2, 2.0) == 1.0) {
        res = +0.0;
    } else if (1.0 / base == -INF && exp2 > 0.0 && fmod(exp2, 2.0) == 1.0) {
        res = -0.0;
    } else if ((1.0 / base == -INF || 1.0 / base == INF) && exp2 > 0.0) {
        res = +0.0;
    } else if (base == -1.0 && (exp2 == INF || exp2 == -INF)) {
        res = 1.0;
    } else if (base == 1.0 || exp2 == -0.0) {
        res = 1.0;
    } else if (base < 0 && base != -INF && exp2 != INF
                && exp2 != -INF && fmod(exp2, 1.0) != 0.0) {
        res = NAN;
    } else if (exp2 == -INF && base > -1.0 && base < 1.0) {
        res = INF;
    } else if (exp2 == -INF && (base < -1.0 || base > 1.0)) {
        res = +0.0;
    } else if (exp2 == INF && base > -1.0 && base < 1.0) {
        res = +0.0;
    } else if (exp2 == INF && (base < -1.0 || base > 1.0)) {
        res = INF;
    } else if (base == -INF && exp2 < 0.0 && fmod(exp2, 2.0) == -1.0) {
        res = -0.0;
    } else if (base == -INF && exp2 < 0.0) {
        res = +0.0;
    } else if (base == -INF && exp2 > 0.0 && fmod(exp2, 2.0) == 1.0) {
        res = -INF;
    } else if (base == -INF && exp2 > 0.0) {
        res = INF;
    } else if (base == INF && exp2 < 0.0) {
        res = +0.0;
    } else if (base == INF && exp2 > 0.0) {
        res = INF;
    } else if (base != base || exp2 != exp2) {
        res = NAN;
    } else {
        long double ldbase = (long double)base;
        long long i = (long long)exp2;
        res = exp(log(fabs(base)) * fmod(exp2, 1.0));
        res *= simple_exp(ldbase, i);
    }
    return res;
}
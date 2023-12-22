#include "stdlib.h"
#include "../ctype.h"
#include <stddef.h>
#include <stdint.h>

double strtod(const char *str, char **endptr) {

    double ret = 0;
    int i = 0;
    int negative = 0;
    int digits = 0;

    /* skip white spaces: */
    while (isspace(str[i]))
        i++;

    /* sign character: */
    if (str[i] == '+') {
        negative = 0;
        i++;
    } else if (str[i] == '-') {
        negative = 1;
        i++;
    }

    /* before float point: */
    while(str[i] >= '0' && str[i] <= '9') {
        ret = ret*10 + (str[i]-'0');
        digits++;
        i++;
    }

    /* float point? */
    if (str[i] == '.') {
        double tmp = 1;
        i++;
        while(str[i] >= '0' && str[i] <= '9') {
            tmp *= 10;
            ret = ret + (str[i]-'0')/tmp;
            digits++;
            i++;
        }
    }

    /* valid till now? */
    if (!digits) {
        /* no digits have been read! */
        return 0;
    }

    /* evaluate sign: */
    if (negative)
        ret = -ret;

    /* TODO: exponent? */

    if (endptr) {
        *endptr = (char *) &str[i];
    }

    return ret;

}

/* 交换A,B的值 */
static inline void swap(int* a, int* b){
  (*a) = (*a) ^ (*b);
  (*b) = (*a) ^ (*b);
  (*a) = (*a) ^ (*b);
}

/* 比较方法 */
static inline int compare(int a, int b, int (*cmp) (int a, int b)) {
  return cmp ? cmp(a, b) : a < b;
}


void Qsort(int list[], int len, int (*cmp) (int a, int b)) {
    if (len <= 2){
        if (len == 2 && compare(list[1], list[0], cmp))
        swap(&list[0], &list[1]);
        return;
    }
    int left = 1, right = len - 1;
    while (left != right) {
        if (compare(list[0], list[right], cmp)){
        right--;
        continue;
        }
        if (compare(list[left], list[0], cmp)) {
        left++;
        continue;
        }
        swap(&list[left], &list[right]);
    }
    swap(&list[0], &list[left]);
    return Qsort(list, left + 1, cmp), Qsort(list + left, len - left, cmp);
}
#include "stdlib.h"
#include "../ctype.h"
#include <stddef.h>
#include <stdint.h>


static void swap(void *va, void *vb, size_t s)
{
	char t, *a = (char*)va, *b = (char*)vb;
	while(s--) t = a[s], a[s] = b[s], b[s] = t;
}

static void isort(char *base, char *hi, size_t size, int (*compar)(const void *, const void*))
{
	char *p, *q;
	for (p = base; p <= hi; p+=size)
	{
		for (q = p; q > base && compar(q - size,q) > 0; q-=size)
			swap(q-size, q, size);
	}
}

#define CUTOFF 5 /* Leave smaller (sub)lists unsorted. */
#define AVG(hi,lo,num,den,size) ( lo + ((num*(hi-lo)) / (den*size)) * size )
static void dsort(char *lo, char *hi, size_t size, int (*compar)(const void *, const void*))
{
	if (hi - (CUTOFF * size) < lo) return;

	swap(lo, AVG(hi,lo,1,3,size), size);
	swap(hi, AVG(hi,lo,2,3,size), size);

	if (compar(hi,lo) < 0) swap(lo, hi, size);

	char *lt = lo + size;
	char *gt = hi - size;
	char *i  = lo + size;

	while (i <= gt)
	{
		if (compar(i, lo) < 0)
		{
			swap(lt, i, size);
			lt += size;
			i += size;
		}
		else if (compar(hi, i) < 0)
		{
			swap(gt, i, size);
			gt -= size;
		}
		else
		{
			i += size;
		}
	}
	lt -= size;
	gt += size;
	swap(lo, lt, size);
	swap(hi, gt, size);

	dsort(lo, lt - size, size, compar);
	if (compar(lt, gt) < 0) dsort(lt + size, gt - size, size, compar);
	dsort(gt + size, hi, size, compar);
}

void qsort(void *base, size_t nitems, size_t size, int (*compar)(const void *, const void*))
{
	dsort(base, (char*)base + (nitems * size) - size, size, compar);
	isort(base, (char*)base + (nitems * size) - size, size, compar);
}
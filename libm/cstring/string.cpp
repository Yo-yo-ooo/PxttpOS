#include "string.h"

void * memcpy(void *s1, const void *s2, register size_t n)
{
    register char *p1 = s1;
    register const char *p2 = s2;


    if (n) {
        n++;
        while (--n > 0) {
            *p1++ = *p2++;
        }
    }
    return s1;
}

void * memmove(void *s1, const void *s2, register size_t n)
{
    register char *p1 = s1;
    register const char *p2 = s2;
    if (n>0) {
        if (p2 <= p1 && p2 + n > p1) {
            /* overlap, copy backwards */
            p1 += n;
            p2 += n;
            n++;
            while (--n > 0) {
                *--p1 = *--p2;
            }
        } else {
            n++;
            while (--n > 0) {
                *p1++ = *p2++;
            }
        }
    }
    return s1;
}

void *memchr(const void *s, register int c, register size_t n)
{
    register const unsigned char *s1 = s;
    c = (unsigned char) c;
    if (n) {
        n++;
        while (--n > 0) {
            if (*s1++ != c) continue;
            return (void *) --s1;
        }
    }
    return NULL;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
    register const unsigned char *p1 = s1, *p2 = s2;
    if (n) {
        n++;
        while (--n > 0) {
            if (*p1++ == *p2++) continue;
            return *--p1 - *--p2;
        }
    }
    return 0;
}

void *memset(void *s, register int c, register size_t n)
{
    register char *s1 = s;

    if (n>0) {
        n++;
        while (--n > 0) {
            *s1++ = c;
        }
    }
    return s;
}

char *strcat(char *ret, register const char *s2)
{
    register char *s1 = ret;
    while (*s1++ != '\0')
        /* EMPTY */ ;
    s1--;
    while (*s1++ = *s2++)
        /* EMPTY */ ;
    return ret;
}

char *strncat(char *ret, register const char *s2, size_t n)
{
    register char *s1 = ret;

    if (n > 0) {
        while (*s1++)
            /* EMPTY */ ;
        s1--;
        while (*s1++ = *s2++)  {
            if (--n > 0) continue;
            *s1 = '\0';
            break;
        }
        return ret;
    } else return s1;
}

char *strchr(register const char *s, register int c)
{
    c = (char) c;

    while (c != *s) {
        if (*s++ == '\0') return NULL;
    }
    return (char *)s;
}

char *strrchr(register const char *s, int c)
{
    register const char *result = NULL;

    c = (char) c;

    do {
        if (c == *s)
            result = s;
    } while (*s++ != '\0');

    return (char *)result;
}

int strcmp(register const char *s1, register const char *s2)
{
    while (*s1 == *s2++) {
        if (*s1++ == '\0') {
            return 0;
        }
    }
    if (*s1 == '\0') return -1;
    if (*--s2 == '\0') return 1;
    return (unsigned char) *s1 - (unsigned char) *s2;
}

int strncmp(register const char *s1, register const char *s2, register size_t n)
{
    if (n) {
        do {
            if (*s1 != *s2++)
                break;
            if (*s1++ == '\0')
                return 0;
        } while (--n > 0);
        if (n > 0) {
            if (*s1 == '\0') return -1;
            if (*--s2 == '\0') return 1;
            return (unsigned char) *s1 - (unsigned char) *s2;
        }
    }
    return 0;
}

int strcoll(register const char *s1, register const char *s2)
{
    while (*s1 == *s2++) {
        if (*s1++ == '\0') {
            return 0;
        }
    }
    return *s1 - *--s2;
}

char *strcpy(char *ret, register const char *s2)
{
    register char *s1 = ret;

    while (*s1++ = *s2++)
        /* EMPTY */ ;

    return ret;
}

char *strncpy(char *ret, register const char *s2, register size_t n)
{
    register char *s1 = ret;

    if (n>0) {
        while((*s1++ = *s2++) && --n > 0)
            /* EMPTY */ ;
        if ((*--s2 == '\0') && --n > 0) {
            do {
                *s1++ = '\0';
            } while(--n > 0);
        }
    }
    return ret;
}

size_t strlen(const char *org)
{
    register const char *s = org;

    while (*s++)
        /* EMPTY */ ;

    return --s - org;
}

char *strerror(register int errnum)
{
    return "unknown error";
}

size_t strspn(const char *string, const char *in)
{
    register const char *s1, *s2;

    for (s1 = string; *s1; s1++) {
        for (s2 = in; *s2 && *s2 != *s1; s2++)
            /* EMPTY */ ;
        if (*s2 == '\0')
            break;
    }
    return s1 - string;
}

size_t strcspn(const char *string, const char *notin)
{
    register const char *s1, *s2;

    for (s1 = string; *s1; s1++) {
        for(s2 = notin; *s2 != *s1 && *s2; s2++)
            /* EMPTY */ ;
        if (*s2)
            break;
    }
    return s1 - string;
}

char *strpbrk(register const char *string, register const char *brk)
{
    register const char *s1;

    while (*string) {
        for (s1 = brk; *s1 && *s1 != *string; s1++)
            /* EMPTY */ ;
        if (*s1)
            return (char *)string;
        string++;
    }
    return (char *)NULL;
}

char *strstr(register const char *s, register const char *wanted)
{
    register const size_t len = strlen(wanted);

    if (len == 0) return (char *)s;
    while (*s != *wanted || strncmp(s, wanted, len))
        if (*s++ == '\0')
            return (char *)NULL;
    return (char *)s;
}

char *strtok(register char *string, const char *separators)
{
    register char *s1, *s2;
    static char *savestring;

    if (string == NULL) {
        string = savestring;
        if (string == NULL) return (char *)NULL;
    }

    s1 = string + strspn(string, separators);
    if (*s1 == '\0') {
        savestring = NULL;
        return (char *)NULL;
    }

    s2 = strpbrk(s1, separators);
    if (s2 != NULL)
        *s2++ = '\0';
    savestring = s2;
    return s1;
}

size_t strxfrm(register char *s1, register const char *save, register size_t n)
{
    register const char *s2 = save;

    while (*s2) {
        if (n > 1) {
            n--;
            *s1++ = *s2++;
        } else
            s2++;
    }
    if (n > 0)
        *s1++ = '\0';
    return s2 - save;
}
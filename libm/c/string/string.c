#include "string.h"


char * strcpy (char * dst, const char * src)
{
    char * cp = dst;
    while( *cp++ = *src++ != '\0')
        ;               /* Copy src over dst */
    return( dst );
}

/*​strncpy函数:从src复制n个字符到dest中，若n<strlen(src),则dest不以'\0'结尾，
  若n>strlen(src),则多余的字节被填充'\0',算法如下
Purpose:
       Copies count characters from the source string to the
       destination.  If count is less than the length of source,
       NO NULL CHARACTER is put onto the end of the copied string.
       If count is greater than the length of sources, dest is padded
       with null characters to length count.
*/
char * strncpy (char *dest, const char *source, unsigned count)
{
    char *start = dest;
 
    while (count && (*dest++ = *source++ != '\0'))
        count--;
 
    if (count)
        while (--count)
            *dest++ = '\0';
 
    return(start);
}

/*
char *strcat(dst, src) - concatenate (append) one string to another
Purpose:
       Concatenates src onto the end of dest.  Assumes enough
       space in dest.
*/
 
char * strcat (char * dst, const char * src)
{
    char * cp = dst;
 
    while( *cp )
        ++cp;           /* Find end of dst */
    while( *cp++ = *src++ )
       ;               /* Copy src to end of dst */
    return( dst );
}

/*
char *strncat(front, back, count) - append count chars of back onto front
Purpose:
       Appends at most count characters of the string back onto the
       end of front, and ALWAYS terminates with a null character.
       If count is greater than the length of back, the length of back
       is used instead.  (Unlike strncpy, this routine does not pad out
       to count characters).
*/
 
char *strncat (char *front, const char *back, unsigned count)
{
    char *start = front;
 
    while (*front++)
        ;
    front--;
 
    while (count--)
        if ((*front++ = *back++) == '\0')
            return(start);
    *front = '\0';
    return(start);
}

/*
strcmp - compare two strings, returning less than, equal to, or greater than
Purpose:
       Compares two string, determining their lexical order.  Unsigned
       comparison is used.return < 0, 0, or >0, indicating whether the 
       first string is Less than, Equal to, or Greater than the second string.
*/
int strcmp ( const char *s1, const char *s2)
{
    for(;(*s1 == *s2) && (*s1 != '\0');s1++,s2++)
        ;
    return *s1-*s2;
}

/*int strncmp(first, last, count) - compare first count chars of strings
Purpose:
       Compares two strings for lexical order.  The comparison stops
       after: (1) a difference between the strings is found, (2) the end
       of the strings is reached, or (3) count characters have been compared.
*/
int strncmp(const char *s1, const char *s2, size_t n)
{
    
    for(;(*s1 != '\0') && (n > 0) && (*s1 == *s2);s1++,s2++,n--)
        ;
    if(n == 0)
        return 0;
    else
        return *s1 - *s2;
}


/*
char *strchr(string, chr) - search a string for a character
Purpose:
       Searches a string for a given character, which may be the
       null character '\0'.
*/
char *strchr (const char *string, int chr)
{
    while (*string && *string != chr)
        string++;
    if (*string == chr)
        return(string);
    return((char *)0);
}

/*
       returns a pointer to the last occurrence of ch in the given
       string. returns NULL if ch does not occurr in the string
*/
 
char *strrchr (const char *string, int ch)
{
    char *start = string;
    while (*string++)
        ;
    while (--string != start && *string != ch)
        ;
    if (*string == ch)
        return(string);
    return(NULL);
}

//若c在s中，同strchr,否则返回指向字符串末尾的‘\0’的指针，而不是NULL
char *strchrnul (const char *string, int chr)
{
    while (*string && *string != chr)
        string++;
    return(string); 
}


/***
*char *strstr(string1, string2) - search for string2 in string1
*
*Purpose:
*       finds the first occurrence of string2 in string1
*
*Entry:
*       char *string1 - string to search in
*       char *string2 - string to search for
*
*Exit:
*       returns a pointer to the first occurrence of string2 in
*       string1, or NULL if string2 does not occur in string1
*******************************************************************************/
char * strstr (const char * str1,const char * str2)
{
        char *cp = (char *) str1;
        char *s1, *s2;
 
        if ( *str2 == '\0' )
            return((char *)str1);
 
        while (*cp)
        {
                s1 = cp;
                s2 = (char *) str2;
 
                while ( *s1 && *s2 && (*s1 == *s2) )
                        s1++, s2++;
 
                if (*s2 == '\0')
                        return(cp);
 
                cp++;
        }
 
        return(NULL);
}

void* memcpy(void* memTo, void* memFrom, size_t size)
{
	//assert(memTo != NULL && memFrom != NULL);
	char* temFrom = (char*)memFrom;
	char* temTo = (char*)memTo;
	while(size-- > 0)
		*temTo++ = *temFrom++;
	return memTo;
}



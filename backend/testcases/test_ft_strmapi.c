/* Test harness for ft_strmapi */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libft.h"

static char to_upper_indexed(unsigned int i, char c)
{
    (void)i;
    if (c >= 'a' && c <= 'z')
        return c - 32;
    return c;
}

static char add_index(unsigned int i, char c)
{
    return c + i;
}

static char identity(unsigned int i, char c)
{
    (void)i;
    return c;
}

int main(void)
{
    /* Test with toupper-like function */
    {
        char *r = ft_strmapi("hello", to_upper_indexed);
        if (r && strcmp(r, "HELLO") == 0)
            printf("PASS to_upper HELLO %s\n", r);
        else
            printf("FAIL to_upper HELLO %s\n", r ? r : "NULL");
        free(r);
    }
    /* Test with index-adding function: 'a'+0, 'a'+1, 'a'+2 = a, b, c */
    {
        char *r = ft_strmapi("aaaa", add_index);
        if (r && strcmp(r, "abcd") == 0)
            printf("PASS add_index abcd %s\n", r);
        else
            printf("FAIL add_index abcd %s\n", r ? r : "NULL");
        free(r);
    }
    /* Test identity */
    {
        char *r = ft_strmapi("hello", identity);
        if (r && strcmp(r, "hello") == 0)
            printf("PASS identity hello %s\n", r);
        else
            printf("FAIL identity hello %s\n", r ? r : "NULL");
        free(r);
    }
    /* Test empty string */
    {
        char *r = ft_strmapi("", to_upper_indexed);
        if (r && strcmp(r, "") == 0)
            printf("PASS empty_string _ _\n");
        else
            printf("FAIL empty_string empty %s\n", r ? r : "NULL");
        free(r);
    }
    /* Test single char */
    {
        char *r = ft_strmapi("a", to_upper_indexed);
        if (r && strcmp(r, "A") == 0)
            printf("PASS single_char A %s\n", r);
        else
            printf("FAIL single_char A %s\n", r ? r : "NULL");
        free(r);
    }
    /* Verify original is not modified */
    {
        char src[] = "hello";
        char *r = ft_strmapi(src, to_upper_indexed);
        if (strcmp(src, "hello") == 0)
            printf("PASS original_unmodified hello %s\n", src);
        else
            printf("FAIL original_unmodified hello %s\n", src);
        free(r);
    }
    return 0;
}

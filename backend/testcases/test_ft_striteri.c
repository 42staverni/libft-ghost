/* Test harness for ft_striteri */
#include <stdio.h>
#include <string.h>
#include "libft.h"

static void to_upper_iter(unsigned int i, char *c)
{
    (void)i;
    if (*c >= 'a' && *c <= 'z')
        *c -= 32;
}

static void add_index_iter(unsigned int i, char *c)
{
    *c = *c + i;
}

static void noop(unsigned int i, char *c)
{
    (void)i;
    (void)c;
}

int main(void)
{
    /* Test toupper in-place */
    {
        char s[] = "hello";
        ft_striteri(s, to_upper_iter);
        if (strcmp(s, "HELLO") == 0)
            printf("PASS to_upper HELLO %s\n", s);
        else
            printf("FAIL to_upper HELLO %s\n", s);
    }
    /* Test add_index: 'a'+0, 'a'+1, 'a'+2, 'a'+3 = a, b, c, d */
    {
        char s[] = "aaaa";
        ft_striteri(s, add_index_iter);
        if (strcmp(s, "abcd") == 0)
            printf("PASS add_index abcd %s\n", s);
        else
            printf("FAIL add_index abcd %s\n", s);
    }
    /* Test empty string */
    {
        char s[] = "";
        ft_striteri(s, to_upper_iter);
        if (strcmp(s, "") == 0)
            printf("PASS empty_string _ _\n");
        else
            printf("FAIL empty_string empty %s\n", s);
    }
    /* Test noop */
    {
        char s[] = "hello";
        ft_striteri(s, noop);
        if (strcmp(s, "hello") == 0)
            printf("PASS noop hello %s\n", s);
        else
            printf("FAIL noop hello %s\n", s);
    }
    /* Test single char */
    {
        char s[] = "a";
        ft_striteri(s, to_upper_iter);
        if (strcmp(s, "A") == 0)
            printf("PASS single_char A %s\n", s);
        else
            printf("FAIL single_char A %s\n", s);
    }
    return 0;
}

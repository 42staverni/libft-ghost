/* Test harness for ft_strdup */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libft.h"

static void test(const char *name, char *src)
{
    char *r = ft_strdup(src);
    if (!r)
    {
        printf("FAIL %s non_null NULL allocation_failed\n", name);
        return;
    }
    if (strcmp(r, src) == 0 && r != src)
        printf("PASS %s %zu %zu\n", name, strlen(src), strlen(r));
    else
        printf("FAIL %s %s %s\n", name, src, r);
    free(r);
}

int main(void)
{
    test("hello", "hello");
    test("empty", "");
    test("single_char", "a");
    test("with_spaces", "hello world");
    test("long_string", "abcdefghijklmnopqrstuvwxyz0123456789");
    test("special_chars", "!@#$%^&*()");
    test("with_tabs", "hello\tworld\n");
    test("numbers", "1234567890");
    return 0;
}

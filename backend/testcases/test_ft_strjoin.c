/* Test harness for ft_strjoin */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libft.h"

static void test(const char *name, char const *s1, char const *s2,
    const char *expected)
{
    char *r = ft_strjoin(s1, s2);
    if (!r)
    {
        printf("FAIL %s %s NULL allocation_failed\n", name, expected);
        return;
    }
    if (strcmp(r, expected) == 0)
        printf("PASS %s %s %s\n", name, expected, r);
    else
        printf("FAIL %s %s %s\n", name, expected, r);
    free(r);
}

int main(void)
{
    test("basic", "hello", " world", "hello world");
    test("empty_s1", "", "world", "world");
    test("empty_s2", "hello", "", "hello");
    test("both_empty", "", "", "");
    test("single_chars", "a", "b", "ab");
    test("long_strings", "abcdefghij", "klmnopqrst", "abcdefghijklmnopqrst");
    test("with_spaces", "hello ", " world", "hello  world");
    test("special_chars", "!@#", "$%^", "!@#$%^");
    test("numbers", "123", "456", "123456");
    return 0;
}

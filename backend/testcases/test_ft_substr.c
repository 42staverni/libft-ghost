/* Test harness for ft_substr */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libft.h"

static void test(const char *name, char const *s, unsigned int start,
    size_t len, const char *expected)
{
    char *r = ft_substr(s, start, len);
    if (!r)
    {
        if (expected == NULL)
            printf("PASS %s NULL NULL\n", name);
        else
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
    test("basic", "hello world", 6, 5, "world");
    test("from_start", "hello", 0, 3, "hel");
    test("full_string", "hello", 0, 5, "hello");
    test("single_char", "hello", 1, 1, "e");
    test("empty_len", "hello", 0, 0, "");
    test("start_beyond", "hello", 10, 5, "");
    test("len_beyond", "hello", 3, 100, "lo");
    test("empty_string", "", 0, 5, "");
    test("start_at_end", "hello", 5, 5, "");
    test("last_char", "hello", 4, 1, "o");
    test("start_at_null", "hello", 5, 0, "");
    return 0;
}

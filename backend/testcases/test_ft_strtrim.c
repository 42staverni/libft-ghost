/* Test harness for ft_strtrim */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libft.h"

static void test(const char *name, char const *s1, char const *set,
    const char *expected)
{
    char *r = ft_strtrim(s1, set);
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
    test("basic_spaces", "  hello  ", " ", "hello");
    test("trim_chars", "xxhelloxx", "x", "hello");
    test("trim_set", "abchelloabc", "abc", "hello");
    test("no_trim", "hello", "x", "hello");
    test("all_trimmed", "xxx", "x", "");
    test("empty_string", "", "x", "");
    test("empty_set", "hello", "", "hello");
    test("both_empty", "", "", "");
    test("trim_front_only", "xxhello", "x", "hello");
    test("trim_back_only", "helloxx", "x", "hello");
    test("mixed_set", " \thello \t", " \t", "hello");
    test("single_char_remain", "aba", "a", "b");
    test("no_middle_trim", "axbxc", "abc", "xbx");
    test("single_char_str", "a", "a", "");
    test("single_char_no_trim", "a", "b", "a");
    return 0;
}

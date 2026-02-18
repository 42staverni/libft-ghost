/* Test harness for ft_strnstr */
#include <stdio.h>
#include <string.h>
#include <bsd/string.h>
#include "libft.h"

static void test(const char *name, const char *haystack,
    const char *needle, size_t len)
{
    char *r1 = strnstr(haystack, needle, len);
    char *r2 = ft_strnstr(haystack, needle, len);
    if (r1 == NULL && r2 == NULL)
        printf("PASS %s NULL NULL\n", name);
    else if (r1 != NULL && r2 != NULL
        && (r1 - haystack) == (r2 - haystack))
        printf("PASS %s %ld %ld\n", name,
            r1 - haystack, r2 - haystack);
    else
        printf("FAIL %s %ld %ld\n", name,
            r1 ? r1 - haystack : -1,
            r2 ? r2 - haystack : -1);
}

int main(void)
{
    test("basic", "hello world", "world", 11);
    test("at_start", "hello world", "hello", 11);
    test("not_found", "hello world", "xyz", 11);
    test("truncated", "hello world", "world", 8);
    test("exact_boundary", "hello world", "world", 11);
    test("empty_needle", "hello", "", 5);
    test("empty_haystack", "", "hello", 5);
    test("both_empty", "", "", 5);
    test("len_zero", "hello", "hello", 0);
    test("needle_longer", "hi", "hello", 5);
    test("partial_match", "aaab", "aab", 4);
    test("needle_at_end", "abcdef", "def", 6);
    test("len_exact", "abc", "abc", 3);
    test("len_one_short", "abc", "abc", 2);
    test("repeated_pattern", "aaabaaab", "aaab", 8);
    return 0;
}

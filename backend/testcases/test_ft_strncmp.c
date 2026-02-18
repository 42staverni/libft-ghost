/* Test harness for ft_strncmp */
#include <stdio.h>
#include <string.h>
#include "libft.h"

static int sign(int x) { return (x > 0) - (x < 0); }

static void test(const char *name, const char *s1, const char *s2, size_t n)
{
    int r1 = strncmp(s1, s2, n);
    int r2 = ft_strncmp(s1, s2, n);
    if (sign(r1) == sign(r2))
        printf("PASS %s %d %d\n", name, sign(r1), sign(r2));
    else
        printf("FAIL %s %d %d\n", name, sign(r1), sign(r2));
}

int main(void)
{
    test("equal", "hello", "hello", 5);
    test("equal_n0", "hello", "world", 0);
    test("diff_first", "abc", "bbc", 3);
    test("diff_last", "abc", "abd", 3);
    test("s1_shorter", "ab", "abc", 3);
    test("s2_shorter", "abc", "ab", 3);
    test("partial_equal", "abcdef", "abcxyz", 3);
    test("partial_diff", "abcdef", "abcxyz", 4);
    test("empty_both", "", "", 5);
    test("empty_s1", "", "abc", 3);
    test("empty_s2", "abc", "", 3);
    test("one_char_eq", "a", "a", 1);
    test("one_char_diff", "a", "b", 1);
    test("n_larger", "abc", "abc", 100);
    test("unsigned_compare", "\200", "\0", 1);
    return 0;
}

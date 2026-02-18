/* Test harness for ft_memcmp */
#include <stdio.h>
#include <string.h>
#include "libft.h"

static int sign(int x) { return (x > 0) - (x < 0); }

static void test(const char *name, const void *s1, const void *s2, size_t n)
{
    int r1 = memcmp(s1, s2, n);
    int r2 = ft_memcmp(s1, s2, n);
    if (sign(r1) == sign(r2))
        printf("PASS %s %d %d\n", name, sign(r1), sign(r2));
    else
        printf("FAIL %s %d %d\n", name, sign(r1), sign(r2));
}

int main(void)
{
    test("equal", "hello", "hello", 5);
    test("diff_first", "abc", "bbc", 3);
    test("diff_last", "abc", "abd", 3);
    test("n_zero", "abc", "xyz", 0);
    test("partial_equal", "abcdef", "abcxyz", 3);
    test("partial_diff", "abcdef", "abcxyz", 4);
    test("with_null", "abc\0x", "abc\0y", 5);
    test("unsigned_compare", "\xff", "\x00", 1);
    test("both_empty", "", "", 0);
    test("longer_equal", "abcdefghij", "abcdefghij", 10);
    return 0;
}

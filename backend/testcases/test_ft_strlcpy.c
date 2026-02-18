/* Test harness for ft_strlcpy */
#include <stdio.h>
#include <string.h>
#include <bsd/string.h>
#include "libft.h"

static void test(const char *name, const char *src, size_t dstsize)
{
    char buf1[64];
    char buf2[64];
    size_t r1, r2;

    memset(buf1, 'X', sizeof(buf1));
    memset(buf2, 'X', sizeof(buf2));
    r1 = strlcpy(buf1, src, dstsize);
    r2 = ft_strlcpy(buf2, src, dstsize);
    if (r1 == r2 && memcmp(buf1, buf2, sizeof(buf1)) == 0)
        printf("PASS %s %zu %zu\n", name, r1, r2);
    else
        printf("FAIL %s %zu %zu\n", name, r1, r2);
}

int main(void)
{
    test("basic", "hello", 10);
    test("exact_fit", "hello", 6);
    test("truncate", "hello", 3);
    test("one_byte_dst", "hello", 1);
    test("zero_dstsize", "hello", 0);
    test("empty_src", "", 10);
    test("empty_src_zero_dst", "", 0);
    test("long_src_small_dst", "abcdefghijklmnop", 5);
    test("single_char", "a", 2);
    test("single_char_no_room", "a", 1);
    return 0;
}

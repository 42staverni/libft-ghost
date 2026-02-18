/* Test harness for ft_bzero */
#include <stdio.h>
#include <string.h>
#include "libft.h"

static void test(const char *name, size_t n)
{
    char buf1[64];
    char buf2[64];

    memset(buf1, 'A', sizeof(buf1));
    memset(buf2, 'A', sizeof(buf2));
    bzero(buf1, n);
    ft_bzero(buf2, n);
    if (memcmp(buf1, buf2, sizeof(buf1)) == 0)
        printf("PASS %s %zu %zu\n", name, n, n);
    else
        printf("FAIL %s %zu %zu\n", name, n, n);
}

int main(void)
{
    test("zero_bytes", 0);
    test("one_byte", 1);
    test("five_bytes", 5);
    test("ten_bytes", 10);
    test("full_64", 64);
    return 0;
}

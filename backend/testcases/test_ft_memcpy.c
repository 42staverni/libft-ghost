/* Test harness for ft_memcpy */
#include <stdio.h>
#include <string.h>
#include "libft.h"

static void test(const char *name, const void *src, size_t n)
{
    char buf1[64];
    char buf2[64];
    void *ret;

    memset(buf1, 0, sizeof(buf1));
    memset(buf2, 0, sizeof(buf2));
    memcpy(buf1, src, n);
    ret = ft_memcpy(buf2, src, n);
    if (memcmp(buf1, buf2, sizeof(buf1)) == 0 && ret == buf2)
        printf("PASS %s %zu %zu\n", name, n, n);
    else
        printf("FAIL %s %zu %zu\n", name, n, n);
}

int main(void)
{
    test("zero_bytes", "hello", 0);
    test("one_byte", "A", 1);
    test("hello", "hello", 5);
    test("with_nulls", "ab\0cd", 5);
    test("binary_data", "\x01\x02\x03\x04\x05", 5);
    test("long_string", "abcdefghijklmnopqrstuvwxyz0123456789", 36);

    /* NULL src and dst with n=0 should not crash */
    {
        void *ret = ft_memcpy(NULL, NULL, 0);
        /* Behavior with NULL and n=0 is implementation-defined;
           just check it didn't crash */
        (void)ret;
        printf("PASS null_with_zero 0 0\n");
    }
    return 0;
}

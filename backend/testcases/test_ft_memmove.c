/* Test harness for ft_memmove */
#include <stdio.h>
#include <string.h>
#include "libft.h"

static void test_basic(const char *name, const char *src, size_t n)
{
    char buf1[64];
    char buf2[64];
    void *ret;

    memset(buf1, 0, sizeof(buf1));
    memset(buf2, 0, sizeof(buf2));
    memmove(buf1, src, n);
    ret = ft_memmove(buf2, src, n);
    if (memcmp(buf1, buf2, sizeof(buf1)) == 0 && ret == buf2)
        printf("PASS %s %zu %zu\n", name, n, n);
    else
        printf("FAIL %s %zu %zu\n", name, n, n);
}

int main(void)
{
    test_basic("zero_bytes", "hello", 0);
    test_basic("one_byte", "A", 1);
    test_basic("hello", "hello", 5);
    test_basic("with_nulls", "ab\0cd", 5);

    /* Overlap: dst before src */
    {
        char buf1[32] = "0123456789";
        char buf2[32] = "0123456789";
        memmove(buf1, buf1 + 2, 5);
        ft_memmove(buf2, buf2 + 2, 5);
        if (memcmp(buf1, buf2, 32) == 0)
            printf("PASS overlap_dst_before_src 1 1\n");
        else
            printf("FAIL overlap_dst_before_src 1 0\n");
    }

    /* Overlap: dst after src */
    {
        char buf1[32] = "0123456789";
        char buf2[32] = "0123456789";
        memmove(buf1 + 2, buf1, 5);
        ft_memmove(buf2 + 2, buf2, 5);
        if (memcmp(buf1, buf2, 32) == 0)
            printf("PASS overlap_dst_after_src 1 1\n");
        else
            printf("FAIL overlap_dst_after_src 1 0\n");
    }

    /* Same pointer */
    {
        char buf1[16] = "hello";
        char buf2[16] = "hello";
        memmove(buf1, buf1, 5);
        ft_memmove(buf2, buf2, 5);
        if (memcmp(buf1, buf2, 16) == 0)
            printf("PASS same_pointer 1 1\n");
        else
            printf("FAIL same_pointer 1 0\n");
    }

    /* NULL with n=0 */
    {
        ft_memmove(NULL, NULL, 0);
        printf("PASS null_with_zero 0 0\n");
    }
    return 0;
}

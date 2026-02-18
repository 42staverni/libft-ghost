/* Test harness for ft_memset */
#include <stdio.h>
#include <string.h>
#include "libft.h"

static void test(const char *name, size_t len, int c, int fill_len)
{
    char buf1[64];
    char buf2[64];
    void *ret;

    memset(buf1, 'A', sizeof(buf1));
    memset(buf2, 'A', sizeof(buf2));
    memset(buf1, c, fill_len);
    ret = ft_memset(buf2, c, fill_len);
    if (memcmp(buf1, buf2, len) == 0 && ret == buf2)
        printf("PASS %s %d %d\n", name, fill_len, fill_len);
    else
        printf("FAIL %s %d %d\n", name, fill_len, fill_len);
}

int main(void)
{
    test("fill_zero_bytes", 64, 'X', 0);
    test("fill_one_byte", 64, 'X', 1);
    test("fill_five_bytes", 64, 'B', 5);
    test("fill_ten_bytes", 64, 0, 10);
    test("fill_with_null", 64, '\0', 8);
    test("fill_with_255", 64, 255, 4);
    test("fill_full", 64, 'Z', 64);

    /* Return value test */
    {
        char buf[16];
        void *ret = ft_memset(buf, 'A', 16);
        if (ret == buf)
            printf("PASS return_value 1 1\n");
        else
            printf("FAIL return_value 1 0 should_return_pointer_to_buf\n");
    }
    return 0;
}

/* Test harness for ft_isascii */
#include <stdio.h>
#include <ctype.h>
#include "libft.h"

#define TEST(name, input) do { \
    int r = ft_isascii(input); \
    int e = isascii(input) ? 1 : 0; \
    int rn = r ? 1 : 0; \
    if (rn == e) \
        printf("PASS %s %d %d\n", name, e, rn); \
    else \
        printf("FAIL %s %d %d\n", name, e, rn); \
} while(0)

int main(void)
{
    TEST("null_char", 0);
    TEST("char_a", 'a');
    TEST("char_Z", 'Z');
    TEST("digit_0", '0');
    TEST("space", ' ');
    TEST("del_127", 127);
    TEST("value_128", 128);
    TEST("value_255", 255);
    TEST("negative_1", -1);
    TEST("value_1", 1);
    TEST("value_126", 126);
    return 0;
}

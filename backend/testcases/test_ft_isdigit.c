/* Test harness for ft_isdigit */
#include <stdio.h>
#include <ctype.h>
#include "libft.h"

#define TEST(name, input) do { \
    int r = ft_isdigit(input); \
    int e = isdigit(input) ? 1 : 0; \
    int rn = r ? 1 : 0; \
    if (rn == e) \
        printf("PASS %s %d %d\n", name, e, rn); \
    else \
        printf("FAIL %s %d %d\n", name, e, rn); \
} while(0)

int main(void)
{
    TEST("digit_0", '0');
    TEST("digit_1", '1');
    TEST("digit_5", '5');
    TEST("digit_9", '9');
    TEST("char_a", 'a');
    TEST("char_z", 'z');
    TEST("char_A", 'A');
    TEST("space", ' ');
    TEST("minus", '-');
    TEST("plus", '+');
    TEST("dot", '.');
    TEST("null_char", '\0');
    TEST("slash_before_0", '/');
    TEST("colon_after_9", ':');
    TEST("del_127", 127);
    return 0;
}

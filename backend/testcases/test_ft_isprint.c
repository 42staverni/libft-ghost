/* Test harness for ft_isprint */
#include <stdio.h>
#include <ctype.h>
#include "libft.h"

#define TEST(name, input) do { \
    int r = ft_isprint(input); \
    int e = isprint(input) ? 1 : 0; \
    int rn = r ? 1 : 0; \
    if (rn == e) \
        printf("PASS %s %d %d\n", name, e, rn); \
    else \
        printf("FAIL %s %d %d\n", name, e, rn); \
} while(0)

int main(void)
{
    TEST("space", ' ');
    TEST("tilde", '~');
    TEST("char_a", 'a');
    TEST("digit_0", '0');
    TEST("exclaim", '!');
    TEST("null_char", '\0');
    TEST("newline", '\n');
    TEST("tab", '\t');
    TEST("del_127", 127);
    TEST("value_31", 31);
    TEST("value_32", 32);
    TEST("value_126", 126);
    TEST("value_128", 128);
    return 0;
}

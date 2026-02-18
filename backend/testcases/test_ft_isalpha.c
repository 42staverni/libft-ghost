/* Test harness for ft_isalpha */
#include <stdio.h>
#include <ctype.h>
#include "libft.h"

#define TEST(name, input) do { \
    int r = ft_isalpha(input); \
    int e = isalpha(input) ? 1 : 0; \
    int rn = r ? 1 : 0; \
    if (rn == e) \
        printf("PASS %s %d %d\n", name, e, rn); \
    else \
        printf("FAIL %s %d %d\n", name, e, rn); \
} while(0)

int main(void)
{
    TEST("lowercase_a", 'a');
    TEST("lowercase_z", 'z');
    TEST("lowercase_m", 'm');
    TEST("uppercase_A", 'A');
    TEST("uppercase_Z", 'Z');
    TEST("uppercase_M", 'M');
    TEST("digit_0", '0');
    TEST("digit_9", '9');
    TEST("space", ' ');
    TEST("newline", '\n');
    TEST("null_char", '\0');
    TEST("at_sign", '@');
    TEST("bracket", '[');
    TEST("backtick", '`');
    TEST("brace", '{');
    TEST("del_127", 127);
    TEST("negative_1", -1);
    TEST("value_200", 200);
    return 0;
}

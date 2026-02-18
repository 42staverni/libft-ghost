/* Test harness for ft_isalnum */
#include <stdio.h>
#include <ctype.h>
#include "libft.h"

#define TEST(name, input) do { \
    int r = ft_isalnum(input); \
    int e = isalnum(input) ? 1 : 0; \
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
    TEST("uppercase_A", 'A');
    TEST("uppercase_Z", 'Z');
    TEST("digit_0", '0');
    TEST("digit_9", '9');
    TEST("space", ' ');
    TEST("exclaim", '!');
    TEST("at_sign", '@');
    TEST("bracket", '[');
    TEST("backtick", '`');
    TEST("brace", '{');
    TEST("null_char", '\0');
    TEST("newline", '\n');
    TEST("tab", '\t');
    TEST("del_127", 127);
    return 0;
}

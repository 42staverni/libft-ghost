/* Test harness for ft_tolower */
#include <stdio.h>
#include <ctype.h>
#include "libft.h"

#define TEST(name, input) do { \
    int r = ft_tolower(input); \
    int e = tolower(input); \
    if (r == e) \
        printf("PASS %s %d %d\n", name, e, r); \
    else \
        printf("FAIL %s %d %d\n", name, e, r); \
} while(0)

int main(void)
{
    TEST("uppercase_A", 'A');
    TEST("uppercase_Z", 'Z');
    TEST("uppercase_M", 'M');
    TEST("lowercase_a", 'a');
    TEST("lowercase_z", 'z');
    TEST("digit_0", '0');
    TEST("space", ' ');
    TEST("newline", '\n');
    TEST("null_char", '\0');
    TEST("at_sign", '@');
    TEST("bracket", '[');
    TEST("backtick", '`');
    TEST("brace", '{');
    return 0;
}

/* Test harness for ft_atoi */
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "libft.h"

#define TEST(name, input, expected_val) do { \
    int result = ft_atoi(input); \
    int expected = expected_val; \
    if (result == expected) \
        printf("PASS %s %d %d\n", name, expected, result); \
    else \
        printf("FAIL %s %d %d\n", name, expected, result); \
} while(0)

int main(void)
{
    TEST("zero", "0", 0);
    TEST("positive", "42", 42);
    TEST("negative", "-42", -42);
    TEST("plus_sign", "+42", 42);
    TEST("leading_spaces", "   42", 42);
    TEST("leading_tabs", "\t\n\v\f\r 42", 42);
    TEST("int_max", "2147483647", INT_MAX);
    TEST("int_min", "-2147483648", INT_MIN);
    TEST("mixed_chars", "123abc", 123);
    TEST("only_sign", "+", 0);
    TEST("empty", "", 0);
    TEST("double_sign", "++42", 0);
    TEST("minus_plus", "-+42", 0);
    TEST("spaces_between", "12 34", 12);
    TEST("leading_zeros", "0042", 42);
    TEST("negative_zero", "-0", 0);
    return 0;
}

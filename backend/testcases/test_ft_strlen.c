/* Test harness for ft_strlen */
/* Output format: PASS|FAIL test_name expected got [info] */
#include <stdio.h>
#include <string.h>
#include "libft.h"

#define TEST(name, input, expected_val) do { \
    size_t result = ft_strlen(input); \
    size_t expected = expected_val; \
    if (result == expected) \
        printf("PASS %s %zu %zu\n", name, expected, result); \
    else \
        printf("FAIL %s %zu %zu\n", name, expected, result); \
} while(0)

int main(void)
{
    TEST("empty_string", "", 0);
    TEST("hello", "hello", 5);
    TEST("single_char", "a", 1);
    TEST("with_spaces", "hello world", 11);
    TEST("long_string",
        "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ", 52);
    TEST("with_null_inside", "abc\0def", 3);
    TEST("numbers", "12345", 5);
    TEST("special_chars", "!@#$%^&*()", 10);
    TEST("newline", "hello\n", 6);
    TEST("tabs", "\t\t\t", 3);
    return 0;
}

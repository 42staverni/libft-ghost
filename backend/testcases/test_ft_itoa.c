/* Test harness for ft_itoa */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "libft.h"

static void test(const char *name, int n, const char *expected)
{
    char *r = ft_itoa(n);
    if (!r)
    {
        printf("FAIL %s %s NULL allocation_failed\n", name, expected);
        return;
    }
    if (strcmp(r, expected) == 0)
        printf("PASS %s %s %s\n", name, expected, r);
    else
        printf("FAIL %s %s %s\n", name, expected, r);
    free(r);
}

int main(void)
{
    test("zero", 0, "0");
    test("positive", 42, "42");
    test("negative", -42, "-42");
    test("one", 1, "1");
    test("minus_one", -1, "-1");
    test("hundred", 100, "100");
    test("large", 123456, "123456");
    test("int_max", INT_MAX, "2147483647");
    test("int_min", INT_MIN, "-2147483648");
    test("negative_100", -100, "-100");
    test("ten", 10, "10");
    test("nine", 9, "9");
    test("minus_nine", -9, "-9");
    test("thousand", 1000, "1000");
    return 0;
}

/* Test harness for ft_memchr */
#include <stdio.h>
#include <string.h>
#include "libft.h"

static void test(const char *name, const void *s, int c, size_t n)
{
    const void *r1 = memchr(s, c, n);
    const void *r2 = ft_memchr(s, c, n);
    if (r1 == NULL && r2 == NULL)
        printf("PASS %s NULL NULL\n", name);
    else if (r1 != NULL && r2 != NULL
        && ((const char *)r1 - (const char *)s)
            == ((const char *)r2 - (const char *)s))
        printf("PASS %s %ld %ld\n", name,
            (const char *)r1 - (const char *)s,
            (const char *)r2 - (const char *)s);
    else
        printf("FAIL %s %ld %ld\n", name,
            r1 ? (const char *)r1 - (const char *)s : -1,
            r2 ? (const char *)r2 - (const char *)s : -1);
}

int main(void)
{
    test("find_h", "hello", 'h', 5);
    test("find_o", "hello", 'o', 5);
    test("find_l", "hello", 'l', 5);
    test("not_found", "hello", 'x', 5);
    test("n_zero", "hello", 'h', 0);
    test("beyond_null", "hel\0lo", 'l', 6);
    test("find_null_byte", "hel\0lo", '\0', 6);
    test("binary_data", "\x01\x02\x03\x04", '\x03', 4);
    test("first_byte", "abc", 'a', 3);
    test("last_byte", "abc", 'c', 3);
    test("n_shorter", "hello", 'o', 2);
    return 0;
}

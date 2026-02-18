/* Test harness for ft_strrchr */
#include <stdio.h>
#include <string.h>
#include "libft.h"

static void test(const char *name, const char *s, int c)
{
    char *r1 = strrchr(s, c);
    char *r2 = ft_strrchr(s, c);
    if (r1 == NULL && r2 == NULL)
        printf("PASS %s NULL NULL\n", name);
    else if (r1 != NULL && r2 != NULL && (r2 - s) == (r1 - s) && *r2 == *r1)
        printf("PASS %s %ld %ld\n", name, r1 - s, r2 - s);
    else
        printf("FAIL %s %ld %ld\n", name,
            r1 ? r1 - s : -1, r2 ? r2 - s : -1);
}

int main(void)
{
    test("find_h", "hello", 'h');
    test("find_o", "hello", 'o');
    test("find_l_last", "hello", 'l');
    test("not_found", "hello", 'x');
    test("find_null_term", "hello", '\0');
    test("empty_string", "", 'a');
    test("empty_null_term", "", '\0');
    test("first_char", "abc", 'a');
    test("last_char", "abc", 'c');
    test("repeated_last", "abcabc", 'a');
    test("all_same", "aaaa", 'a');
    test("high_value", "hello", 'h' + 256);
    return 0;
}

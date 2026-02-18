/* Test harness for ft_strchr */
#include <stdio.h>
#include <string.h>
#include "libft.h"

static void test(const char *name, const char *s, int c)
{
    char *r1 = strchr(s, c);
    char *r2 = ft_strchr(s, c);
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
    test("find_l_first", "hello", 'l');
    test("not_found", "hello", 'x');
    test("find_null_term", "hello", '\0');
    test("empty_string", "", 'a');
    test("empty_null_term", "", '\0');
    test("first_char", "abc", 'a');
    test("last_char", "abc", 'c');
    test("repeated", "abcabc", 'b');
    test("special_char", "hello\tworld", '\t');
    /* Test with value > 127 cast to char */
    test("high_value", "hello", 'h' + 256);
    return 0;
}

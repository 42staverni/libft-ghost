/* Test harness for ft_strlcat */
#include <stdio.h>
#include <string.h>
#include <bsd/string.h>
#include "libft.h"

static void test(const char *name, const char *initial_dst,
    const char *src, size_t size)
{
    char buf1[64];
    char buf2[64];
    size_t r1, r2;

    memset(buf1, 0, sizeof(buf1));
    memset(buf2, 0, sizeof(buf2));
    strcpy(buf1, initial_dst);
    strcpy(buf2, initial_dst);
    r1 = strlcat(buf1, src, size);
    r2 = ft_strlcat(buf2, src, size);
    if (r1 == r2 && memcmp(buf1, buf2, sizeof(buf1)) == 0)
        printf("PASS %s %zu %zu\n", name, r1, r2);
    else
        printf("FAIL %s %zu %zu\n", name, r1, r2);
}

int main(void)
{
    test("basic", "hello", " world", 20);
    test("exact_fit", "hello", " world", 12);
    test("truncate", "hello", " world", 8);
    test("no_room", "hello", " world", 5);
    test("size_zero", "hello", " world", 0);
    test("empty_dst", "", "hello", 10);
    test("empty_src", "hello", "", 10);
    test("both_empty", "", "", 10);
    test("size_less_than_dst", "hello", " world", 3);
    test("size_equals_dst_len", "hello", " world", 5);
    test("one_byte_append", "hello", "!", 7);
    return 0;
}

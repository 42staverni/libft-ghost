/* Test harness for ft_calloc */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "libft.h"

static void test_content(const char *name, size_t count, size_t size)
{
    void *r = ft_calloc(count, size);
    if (!r && count * size > 0)
    {
        printf("FAIL %s non_null NULL allocation_failed\n", name);
        return;
    }
    if (count == 0 || size == 0)
    {
        /* calloc(0, x) or calloc(x, 0) may return NULL or valid ptr */
        printf("PASS %s 0 0\n", name);
        free(r);
        return;
    }
    /* Verify zeroed */
    char *p = (char *)r;
    int all_zero = 1;
    for (size_t i = 0; i < count * size; i++)
    {
        if (p[i] != 0)
        {
            all_zero = 0;
            break;
        }
    }
    if (all_zero)
        printf("PASS %s %zu %zu\n", name, count * size, count * size);
    else
        printf("FAIL %s zeroed not_zeroed memory_not_initialized\n", name);
    free(r);
}

int main(void)
{
    test_content("one_int", 1, sizeof(int));
    test_content("ten_chars", 10, 1);
    test_content("five_ints", 5, sizeof(int));
    test_content("hundred_bytes", 100, 1);
    test_content("zero_count", 0, 10);
    test_content("zero_size", 10, 0);
    test_content("one_byte", 1, 1);
    test_content("large_alloc", 1000, sizeof(long));

    /* Overflow protection: SIZE_MAX * 2 should fail or return NULL */
    {
        void *r = ft_calloc(SIZE_MAX, 2);
        if (r == NULL)
            printf("PASS overflow_protection NULL NULL\n");
        else
        {
            printf("FAIL overflow_protection NULL non_null should_return_null\n");
            free(r);
        }
    }
    return 0;
}

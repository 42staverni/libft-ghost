/* Test harness for ft_split */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libft.h"

static void free_split(char **arr)
{
    if (!arr)
        return;
    for (int i = 0; arr[i]; i++)
        free(arr[i]);
    free(arr);
}

static void test_split(const char *name, const char *input, char delim,
    const char **expected, int expected_count)
{
    char **result = ft_split(input, delim);
    if (!result) {
        if (expected_count == 0) {
            printf("PASS %s %d 0\n", name, expected_count);
            return;
        }
        printf("FAIL %s expected_array NULL ft_split_returned_NULL\n", name);
        return;
    }

    int count = 0;
    while (result[count])
        count++;

    if (count != expected_count) {
        printf("FAIL %s count_%d count_%d wrong_number_of_elements\n",
            name, expected_count, count);
        free_split(result);
        return;
    }

    for (int i = 0; i < count; i++) {
        if (strcmp(result[i], expected[i]) != 0) {
            printf("FAIL %s %s %s element_%d_mismatch\n",
                name, expected[i], result[i], i);
            free_split(result);
            return;
        }
    }

    if (result[count] != NULL) {
        printf("FAIL %s NULL non_null array_not_null_terminated\n", name);
        free_split(result);
        return;
    }

    printf("PASS %s %d %d\n", name, expected_count, count);
    free_split(result);
}

int main(void)
{
    {
        const char *exp[] = {"hello", "world"};
        test_split("basic", "hello world", ' ', exp, 2);
    }
    {
        const char *exp[] = {"hello", "world"};
        test_split("multi_delim", "  hello  world  ", ' ', exp, 2);
    }
    {
        test_split("empty_string", "", ' ', NULL, 0);
    }
    {
        test_split("only_delim", "   ", ' ', NULL, 0);
    }
    {
        const char *exp[] = {"hello"};
        test_split("no_delim_found", "hello", ' ', exp, 1);
    }
    {
        const char *exp[] = {"a", "b", "c"};
        test_split("single_chars", "a,b,c", ',', exp, 3);
    }
    {
        const char *exp[] = {"hello"};
        test_split("single_word", "hello", ',', exp, 1);
    }
    {
        const char *exp[] = {"split", "this", "for", "me"};
        test_split("longer", "split this for me", ' ', exp, 4);
    }
    return 0;
}

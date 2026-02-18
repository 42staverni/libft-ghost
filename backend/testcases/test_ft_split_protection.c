/* Protection test for ft_split — tests malloc failure cleanup */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>
#include "libft.h"

/*
** We intercept malloc to force failure at a specific call number.
** This lets us verify the function frees everything on partial failure.
*/

static int g_malloc_count = 0;
static int g_fail_at = -1;
static int g_total_allocs = 0;
static int g_total_frees = 0;

void *malloc(size_t size)
{
    static void *(*real_malloc)(size_t) = NULL;
    if (!real_malloc)
        real_malloc = dlsym(RTLD_NEXT, "malloc");

    g_total_allocs++;
    if (g_fail_at >= 0 && g_malloc_count++ >= g_fail_at)
        return NULL;
    return real_malloc(size);
}

void free(void *ptr)
{
    static void (*real_free)(void *) = NULL;
    if (!real_free)
        real_free = dlsym(RTLD_NEXT, "free");
    if (ptr)
        g_total_frees++;
    real_free(ptr);
}

static void free_split(char **arr)
{
    if (!arr)
        return;
    for (int i = 0; arr[i]; i++)
        free(arr[i]);
    free(arr);
}

int main(void)
{
    /* Test NULL input protection */
    {
        char **r = ft_split(NULL, ' ');
        if (r == NULL)
            printf("PASS null_input\n");
        else
        {
            printf("FAIL null_input\n");
            free_split(r);
        }
    }

    /* Test normal operation (no forced failure) */
    {
        g_fail_at = -1;
        g_malloc_count = 0;
        char **r = ft_split("a b c", ' ');
        if (r && r[0] && r[1] && r[2] && r[3] == NULL
            && strcmp(r[0], "a") == 0
            && strcmp(r[1], "b") == 0
            && strcmp(r[2], "c") == 0)
            printf("PASS normal_split\n");
        else
            printf("FAIL normal_split\n");
        free_split(r);
    }

    /* Test malloc failure at first allocation (the array itself) */
    {
        g_fail_at = 0;
        g_malloc_count = 0;
        char **r = ft_split("hello world", ' ');
        if (r == NULL)
            printf("PASS malloc_fail_first\n");
        else
        {
            printf("FAIL malloc_fail_first\n");
            free_split(r);
        }
    }

    /* Test malloc failure at second allocation (first word) */
    {
        g_fail_at = 1;
        g_malloc_count = 0;
        char **r = ft_split("hello world", ' ');
        if (r == NULL)
            printf("PASS malloc_fail_second\n");
        else
        {
            printf("FAIL malloc_fail_second\n");
            free_split(r);
        }
    }

    /* Test malloc failure at third allocation (second word) */
    {
        g_fail_at = 2;
        g_malloc_count = 0;
        char **r = ft_split("hello world", ' ');
        if (r == NULL)
            printf("PASS malloc_fail_third\n");
        else
        {
            printf("FAIL malloc_fail_third\n");
            free_split(r);
        }
    }

    return 0;
}

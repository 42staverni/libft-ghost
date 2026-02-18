/* Test harness for ft_lstiter */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libft.h"

static int g_iter_count = 0;

static void count_iter(void *content)
{
    (void)content;
    g_iter_count++;
}

static void to_upper_content(void *content)
{
    char *s = (char *)content;
    while (*s)
    {
        if (*s >= 'a' && *s <= 'z')
            *s -= 32;
        s++;
    }
}

int main(void)
{
    /* Count iterations */
    {
        g_iter_count = 0;
        t_list *n1 = ft_lstnew("a");
        t_list *n2 = ft_lstnew("b");
        t_list *n3 = ft_lstnew("c");
        n1->next = n2;
        n2->next = n3;
        ft_lstiter(n1, count_iter);
        if (g_iter_count == 3)
            printf("PASS count_three 3 %d\n", g_iter_count);
        else
            printf("FAIL count_three 3 %d\n", g_iter_count);
        free(n1);
        free(n2);
        free(n3);
    }
    /* Modify content */
    {
        char s1[] = "hello";
        char s2[] = "world";
        t_list *n1 = ft_lstnew(s1);
        t_list *n2 = ft_lstnew(s2);
        n1->next = n2;
        ft_lstiter(n1, to_upper_content);
        if (strcmp(s1, "HELLO") == 0 && strcmp(s2, "WORLD") == 0)
            printf("PASS modify_content 1 1\n");
        else
            printf("FAIL modify_content 1 0\n");
        free(n1);
        free(n2);
    }
    /* Single element */
    {
        g_iter_count = 0;
        t_list *lst = ft_lstnew("x");
        ft_lstiter(lst, count_iter);
        if (g_iter_count == 1)
            printf("PASS single 1 %d\n", g_iter_count);
        else
            printf("FAIL single 1 %d\n", g_iter_count);
        free(lst);
    }
    /* NULL list should not crash */
    {
        ft_lstiter(NULL, count_iter);
        printf("PASS null_list 0 0\n");
    }
    return 0;
}

/* Test harness for ft_lstclear */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libft.h"

static int g_del_count = 0;

static void del_func(void *content)
{
    free(content);
    g_del_count++;
}

int main(void)
{
    /* Clear list of 3 */
    {
        g_del_count = 0;
        t_list *n1 = ft_lstnew(strdup("a"));
        t_list *n2 = ft_lstnew(strdup("b"));
        t_list *n3 = ft_lstnew(strdup("c"));
        n1->next = n2;
        n2->next = n3;
        t_list *lst = n1;
        ft_lstclear(&lst, del_func);
        if (lst == NULL && g_del_count == 3)
            printf("PASS clear_three 3 %d\n", g_del_count);
        else
            printf("FAIL clear_three 3 %d\n", g_del_count);
    }
    /* Clear single element */
    {
        g_del_count = 0;
        t_list *lst = ft_lstnew(strdup("only"));
        ft_lstclear(&lst, del_func);
        if (lst == NULL && g_del_count == 1)
            printf("PASS clear_one 1 %d\n", g_del_count);
        else
            printf("FAIL clear_one 1 %d\n", g_del_count);
    }
    /* Clear NULL list */
    {
        g_del_count = 0;
        t_list *lst = NULL;
        ft_lstclear(&lst, del_func);
        if (lst == NULL && g_del_count == 0)
            printf("PASS clear_null 0 %d\n", g_del_count);
        else
            printf("FAIL clear_null 0 %d\n", g_del_count);
    }
    /* Pointer set to NULL */
    {
        g_del_count = 0;
        t_list *lst = ft_lstnew(strdup("x"));
        ft_lstclear(&lst, del_func);
        if (lst == NULL)
            printf("PASS pointer_nulled 1 1\n");
        else
            printf("FAIL pointer_nulled 1 0\n");
    }
    return 0;
}

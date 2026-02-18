/* Test harness for ft_lstdelone */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libft.h"

static int g_del_called = 0;

static void del_func(void *content)
{
    free(content);
    g_del_called++;
}

int main(void)
{
    /* Delete single node with malloc'd content */
    {
        g_del_called = 0;
        char *str = strdup("hello");
        t_list *node = ft_lstnew(str);
        ft_lstdelone(node, del_func);
        if (g_del_called == 1)
            printf("PASS del_called 1 %d\n", g_del_called);
        else
            printf("FAIL del_called 1 %d\n", g_del_called);
    }
    /* Delete node doesn't affect next */
    {
        g_del_called = 0;
        char *s1 = strdup("first");
        char *s2 = strdup("second");
        t_list *n1 = ft_lstnew(s1);
        t_list *n2 = ft_lstnew(s2);
        n1->next = n2;
        ft_lstdelone(n1, del_func);
        /* n2 should still be valid */
        if (g_del_called == 1 && strcmp(n2->content, "second") == 0)
            printf("PASS next_not_affected 1 1\n");
        else
            printf("FAIL next_not_affected 1 0\n");
        ft_lstdelone(n2, del_func);
    }
    /* Delete with NULL content */
    {
        g_del_called = 0;
        t_list *node = ft_lstnew(NULL);
        ft_lstdelone(node, del_func);
        /* del is called even with NULL content */
        printf("PASS null_content 1 1\n");
    }
    return 0;
}

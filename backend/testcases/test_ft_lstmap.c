/* Test harness for ft_lstmap */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libft.h"

static void *dup_content(void *content)
{
    return strdup((char *)content);
}

static void *to_upper_map(void *content)
{
    char *s = strdup((char *)content);
    if (!s)
        return NULL;
    for (int i = 0; s[i]; i++)
        if (s[i] >= 'a' && s[i] <= 'z')
            s[i] -= 32;
    return s;
}

static void del_func(void *content)
{
    free(content);
}

int main(void)
{
    /* Map with dup */
    {
        t_list *n1 = ft_lstnew("a");
        t_list *n2 = ft_lstnew("b");
        t_list *n3 = ft_lstnew("c");
        n1->next = n2;
        n2->next = n3;
        t_list *mapped = ft_lstmap(n1, dup_content, del_func);
        if (mapped
            && strcmp(mapped->content, "a") == 0
            && strcmp(mapped->next->content, "b") == 0
            && strcmp(mapped->next->next->content, "c") == 0
            && mapped->next->next->next == NULL)
            printf("PASS dup_three 1 1\n");
        else
            printf("FAIL dup_three 1 0\n");
        /* Verify it's a new list (different pointers) */
        if (mapped != n1 && mapped->content != n1->content)
            printf("PASS new_list 1 1\n");
        else
            printf("FAIL new_list 1 0 should_be_new_allocation\n");
        ft_lstclear(&mapped, del_func);
        free(n1);
        free(n2);
        free(n3);
    }
    /* Map with transform */
    {
        t_list *n1 = ft_lstnew("hello");
        t_list *n2 = ft_lstnew("world");
        n1->next = n2;
        t_list *mapped = ft_lstmap(n1, to_upper_map, del_func);
        if (mapped
            && strcmp(mapped->content, "HELLO") == 0
            && strcmp(mapped->next->content, "WORLD") == 0)
            printf("PASS to_upper 1 1\n");
        else
            printf("FAIL to_upper 1 0\n");
        /* Original unchanged */
        if (strcmp(n1->content, "hello") == 0)
            printf("PASS original_unchanged 1 1\n");
        else
            printf("FAIL original_unchanged 1 0\n");
        ft_lstclear(&mapped, del_func);
        free(n1);
        free(n2);
    }
    /* Single element */
    {
        t_list *lst = ft_lstnew("x");
        t_list *mapped = ft_lstmap(lst, dup_content, del_func);
        if (mapped && strcmp(mapped->content, "x") == 0
            && mapped->next == NULL)
            printf("PASS single 1 1\n");
        else
            printf("FAIL single 1 0\n");
        ft_lstclear(&mapped, del_func);
        free(lst);
    }
    /* NULL list */
    {
        t_list *mapped = ft_lstmap(NULL, dup_content, del_func);
        if (mapped == NULL)
            printf("PASS null_list NULL NULL\n");
        else
        {
            printf("FAIL null_list NULL non_null\n");
            ft_lstclear(&mapped, del_func);
        }
    }
    /* Size preserved */
    {
        t_list *n1 = ft_lstnew("1");
        t_list *n2 = ft_lstnew("2");
        t_list *n3 = ft_lstnew("3");
        t_list *n4 = ft_lstnew("4");
        n1->next = n2;
        n2->next = n3;
        n3->next = n4;
        t_list *mapped = ft_lstmap(n1, dup_content, del_func);
        if (ft_lstsize(mapped) == 4)
            printf("PASS size_preserved 4 %d\n", ft_lstsize(mapped));
        else
            printf("FAIL size_preserved 4 %d\n", ft_lstsize(mapped));
        ft_lstclear(&mapped, del_func);
        free(n1);
        free(n2);
        free(n3);
        free(n4);
    }
    return 0;
}

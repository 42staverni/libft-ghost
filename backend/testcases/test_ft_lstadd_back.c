/* Test harness for ft_lstadd_back */
#include <stdio.h>
#include <stdlib.h>
#include "libft.h"

int main(void)
{
    /* Add to empty list */
    {
        t_list *lst = NULL;
        t_list *new = ft_lstnew("first");
        ft_lstadd_back(&lst, new);
        if (lst == new && lst->next == NULL)
            printf("PASS add_to_empty 1 1\n");
        else
            printf("FAIL add_to_empty 1 0\n");
        free(new);
    }
    /* Add to non-empty list */
    {
        t_list *first = ft_lstnew("first");
        t_list *lst = first;
        t_list *second = ft_lstnew("second");
        ft_lstadd_back(&lst, second);
        if (lst == first && first->next == second && second->next == NULL)
            printf("PASS add_to_nonempty 1 1\n");
        else
            printf("FAIL add_to_nonempty 1 0\n");
        free(first);
        free(second);
    }
    /* Add multiple to back */
    {
        t_list *lst = NULL;
        t_list *n1 = ft_lstnew("a");
        t_list *n2 = ft_lstnew("b");
        t_list *n3 = ft_lstnew("c");
        ft_lstadd_back(&lst, n1);
        ft_lstadd_back(&lst, n2);
        ft_lstadd_back(&lst, n3);
        if (lst == n1 && n1->next == n2 && n2->next == n3
            && n3->next == NULL)
            printf("PASS add_multiple 1 1\n");
        else
            printf("FAIL add_multiple 1 0\n");
        free(n1);
        free(n2);
        free(n3);
    }
    /* Head pointer unchanged */
    {
        t_list *n1 = ft_lstnew("a");
        t_list *lst = n1;
        t_list *n2 = ft_lstnew("b");
        ft_lstadd_back(&lst, n2);
        if (lst == n1)
            printf("PASS head_unchanged 1 1\n");
        else
            printf("FAIL head_unchanged 1 0\n");
        free(n1);
        free(n2);
    }
    return 0;
}

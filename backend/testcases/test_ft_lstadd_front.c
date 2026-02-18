/* Test harness for ft_lstadd_front */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libft.h"

int main(void)
{
    /* Add to empty list */
    {
        t_list *lst = NULL;
        t_list *new = ft_lstnew("first");
        ft_lstadd_front(&lst, new);
        if (lst == new && lst->content && strcmp(lst->content, "first") == 0
            && lst->next == NULL)
            printf("PASS add_to_empty 1 1\n");
        else
            printf("FAIL add_to_empty 1 0\n");
        free(new);
    }
    /* Add to non-empty list */
    {
        t_list *second = ft_lstnew("second");
        t_list *lst = second;
        t_list *first = ft_lstnew("first");
        ft_lstadd_front(&lst, first);
        if (lst == first && lst->next == second)
            printf("PASS add_to_nonempty 1 1\n");
        else
            printf("FAIL add_to_nonempty 1 0\n");
        free(first);
        free(second);
    }
    /* Add multiple */
    {
        t_list *lst = NULL;
        t_list *n1 = ft_lstnew("c");
        t_list *n2 = ft_lstnew("b");
        t_list *n3 = ft_lstnew("a");
        ft_lstadd_front(&lst, n1);
        ft_lstadd_front(&lst, n2);
        ft_lstadd_front(&lst, n3);
        if (lst == n3 && lst->next == n2 && lst->next->next == n1
            && lst->next->next->next == NULL)
            printf("PASS add_multiple 1 1\n");
        else
            printf("FAIL add_multiple 1 0\n");
        free(n1);
        free(n2);
        free(n3);
    }
    return 0;
}

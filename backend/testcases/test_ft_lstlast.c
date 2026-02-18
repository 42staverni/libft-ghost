/* Test harness for ft_lstlast */
#include <stdio.h>
#include <stdlib.h>
#include "libft.h"

int main(void)
{
    /* NULL list */
    {
        t_list *r = ft_lstlast(NULL);
        if (r == NULL)
            printf("PASS null_list NULL NULL\n");
        else
            printf("FAIL null_list NULL non_null\n");
    }
    /* One element */
    {
        t_list *lst = ft_lstnew("only");
        t_list *r = ft_lstlast(lst);
        if (r == lst)
            printf("PASS one_element 1 1\n");
        else
            printf("FAIL one_element 1 0\n");
        free(lst);
    }
    /* Three elements — should return last */
    {
        t_list *n1 = ft_lstnew("a");
        t_list *n2 = ft_lstnew("b");
        t_list *n3 = ft_lstnew("c");
        n1->next = n2;
        n2->next = n3;
        t_list *r = ft_lstlast(n1);
        if (r == n3)
            printf("PASS three_elements 1 1\n");
        else
            printf("FAIL three_elements 1 0\n");
        free(n1);
        free(n2);
        free(n3);
    }
    /* Last element next is NULL */
    {
        t_list *n1 = ft_lstnew("a");
        t_list *n2 = ft_lstnew("b");
        n1->next = n2;
        t_list *r = ft_lstlast(n1);
        if (r->next == NULL)
            printf("PASS last_next_null 1 1\n");
        else
            printf("FAIL last_next_null 1 0\n");
        free(n1);
        free(n2);
    }
    return 0;
}

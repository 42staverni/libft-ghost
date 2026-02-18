/* Test harness for ft_lstsize */
#include <stdio.h>
#include <stdlib.h>
#include "libft.h"

int main(void)
{
    /* Empty list */
    {
        int r = ft_lstsize(NULL);
        if (r == 0)
            printf("PASS empty_list 0 %d\n", r);
        else
            printf("FAIL empty_list 0 %d\n", r);
    }
    /* One element */
    {
        t_list *lst = ft_lstnew("a");
        int r = ft_lstsize(lst);
        if (r == 1)
            printf("PASS one_element 1 %d\n", r);
        else
            printf("FAIL one_element 1 %d\n", r);
        free(lst);
    }
    /* Three elements */
    {
        t_list *n1 = ft_lstnew("a");
        t_list *n2 = ft_lstnew("b");
        t_list *n3 = ft_lstnew("c");
        n1->next = n2;
        n2->next = n3;
        int r = ft_lstsize(n1);
        if (r == 3)
            printf("PASS three_elements 3 %d\n", r);
        else
            printf("FAIL three_elements 3 %d\n", r);
        free(n1);
        free(n2);
        free(n3);
    }
    /* Five elements */
    {
        t_list *nodes[5];
        for (int i = 0; i < 5; i++)
            nodes[i] = ft_lstnew("x");
        for (int i = 0; i < 4; i++)
            nodes[i]->next = nodes[i + 1];
        int r = ft_lstsize(nodes[0]);
        if (r == 5)
            printf("PASS five_elements 5 %d\n", r);
        else
            printf("FAIL five_elements 5 %d\n", r);
        for (int i = 0; i < 5; i++)
            free(nodes[i]);
    }
    return 0;
}

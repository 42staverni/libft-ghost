/* Test harness for ft_lstnew */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libft.h"

int main(void)
{
    /* Test with string content */
    {
        char *str = "hello";
        t_list *node = ft_lstnew(str);
        if (node && node->content == str && node->next == NULL)
            printf("PASS string_content 1 1\n");
        else
            printf("FAIL string_content 1 0\n");
        free(node);
    }
    /* Test with int content */
    {
        int val = 42;
        t_list *node = ft_lstnew(&val);
        if (node && node->content == &val && node->next == NULL)
            printf("PASS int_content 1 1\n");
        else
            printf("FAIL int_content 1 0\n");
        free(node);
    }
    /* Test with NULL content */
    {
        t_list *node = ft_lstnew(NULL);
        if (node && node->content == NULL && node->next == NULL)
            printf("PASS null_content 1 1\n");
        else
            printf("FAIL null_content 1 0\n");
        free(node);
    }
    /* Test next is NULL */
    {
        t_list *node = ft_lstnew("test");
        if (node && node->next == NULL)
            printf("PASS next_is_null 1 1\n");
        else
            printf("FAIL next_is_null 1 0\n");
        free(node);
    }
    return 0;
}

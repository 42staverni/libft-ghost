/* Protection test for ft_strlen — tests NULL and edge cases */
#include <stdio.h>
#include <signal.h>
#include <setjmp.h>
#include "libft.h"

static jmp_buf jump_buffer;
static void segfault_handler(int sig) {
    (void)sig;
    longjmp(jump_buffer, 1);
}

int main(void)
{
    /* Test NULL protection */
    signal(SIGSEGV, segfault_handler);
    if (setjmp(jump_buffer) == 0) {
        ft_strlen(NULL);
        printf("PASS null_input\n");
    } else {
        printf("FAIL null_input\n");
    }

    /* Test empty string */
    if (ft_strlen("") == 0)
        printf("PASS empty_string\n");
    else
        printf("FAIL empty_string\n");

    return 0;
}

/* Test harness for ft_putchar_fd */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "libft.h"

static void test(const char *name, char c)
{
    int pipefd[2];
    char buf[2] = {0};

    if (pipe(pipefd) == -1)
    {
        printf("FAIL %s pipe pipe_failed\n", name);
        return;
    }
    ft_putchar_fd(c, pipefd[1]);
    close(pipefd[1]);
    read(pipefd[0], buf, 1);
    close(pipefd[0]);
    if (buf[0] == c)
        printf("PASS %s %c %c\n", name, c, buf[0]);
    else
        printf("FAIL %s %c %c\n", name, c, buf[0]);
}

int main(void)
{
    test("char_a", 'a');
    test("char_Z", 'Z');
    test("digit_0", '0');
    test("space", ' ');
    test("newline", '\n');
    test("null_char", '\0');
    test("tilde", '~');
    test("tab", '\t');
    return 0;
}

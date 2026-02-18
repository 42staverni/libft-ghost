/* Test harness for ft_putstr_fd */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "libft.h"

static void test(const char *name, char *s, const char *expected)
{
    int pipefd[2];
    char buf[256] = {0};

    if (pipe(pipefd) == -1)
    {
        printf("FAIL %s pipe pipe_failed\n", name);
        return;
    }
    ft_putstr_fd(s, pipefd[1]);
    close(pipefd[1]);
    read(pipefd[0], buf, sizeof(buf) - 1);
    close(pipefd[0]);
    if (strcmp(buf, expected) == 0)
        printf("PASS %s %zu %zu\n", name, strlen(expected), strlen(buf));
    else
        printf("FAIL %s %s %s\n", name, expected, buf);
}

int main(void)
{
    test("hello", "hello", "hello");
    test("empty", "", "");
    test("with_spaces", "hello world", "hello world");
    test("single_char", "a", "a");
    test("numbers", "12345", "12345");
    test("special", "!@#$%", "!@#$%");
    test("newline", "hello\n", "hello\n");
    return 0;
}

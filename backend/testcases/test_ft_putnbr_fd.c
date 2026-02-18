/* Test harness for ft_putnbr_fd */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include "libft.h"

static void test(const char *name, int n, const char *expected)
{
    int pipefd[2];
    char buf[64] = {0};

    if (pipe(pipefd) == -1)
    {
        printf("FAIL %s pipe pipe_failed\n", name);
        return;
    }
    ft_putnbr_fd(n, pipefd[1]);
    close(pipefd[1]);
    read(pipefd[0], buf, sizeof(buf) - 1);
    close(pipefd[0]);
    if (strcmp(buf, expected) == 0)
        printf("PASS %s %s %s\n", name, expected, buf);
    else
        printf("FAIL %s %s %s\n", name, expected, buf);
}

int main(void)
{
    test("zero", 0, "0");
    test("positive", 42, "42");
    test("negative", -42, "-42");
    test("one", 1, "1");
    test("minus_one", -1, "-1");
    test("int_max", INT_MAX, "2147483647");
    test("int_min", INT_MIN, "-2147483648");
    test("hundred", 100, "100");
    test("large", 123456, "123456");
    test("ten", 10, "10");
    test("nine", 9, "9");
    return 0;
}

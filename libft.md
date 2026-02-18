# Libft

## Common Instructions

-   Your project must be written in C.
-   Your project must be written in accordance with the Norm. If you have bonus files/functions, they are included in the norm check, and you will receive a 0 if there is a norm error.
-   Your functions should not quit unexpectedly (segmentation fault, bus error, double free, etc.) except for undefined behavior. If this occurs, your project will be considered non-functional and will receive a 0 during the evaluation.
-   All heap-allocated memory must be properly freed when necessary. Memory leaks will not be tolerated.
-   If the subject requires it, you must submit a Makefile that compiles your source files to the required output with the flags -Wall, -Wextra, and -Werror, using cc. Additionally, your Makefile must not perform unnecessary relinking.
-   Your Makefile must contain at least the rules $(NAME), all, clean, fclean and re.
-   To submit bonuses for your project, you must include a bonus rule in your Makefile, which will add all the various headers, libraries, or functions that are not allowed in the main part of the project. Bonuses must be placed in _bonus.{c/h} files, unless the subject specifies otherwise. The evaluation of mandatory and bonus parts is conducted separately.
-   If your project allows you to use your libft, you must copy its sources and its associated Makefile into a libft folder. Your project’s Makefile must compile the library by using its Makefile, then compile the project.
-   We encourage you to create test programs for your project, even though this work does not need to be submitted and will not be graded. It will give you an opportunity to easily test your work and your peers’ work. You will find these tests especially useful during your defence. Indeed, during defence, you are free to use your tests and/or the tests of the peer you are evaluating.
-   Submit your work to the assigned Git repository. Only the work in the Git repository will be graded. If Deepthought is assigned to grade your work, it will occur after your peer-evaluations. If an error happens in any section of your work during Deepthought’s grading, the evaluation will stop.

## Mandatory part

| Programe Name     | libft.a                                                      |
| ----------------- | ------------------------------------------------------------ |
| Files to Submit   | Makefile, libft.h, ft_*.c                                    |
| Makefile          | NAME, all, clean, fclean, re                                 |
| External Function | Detailed below                                               |
| Libft authorized  | n/a                                                          |
| Description       | Create your own library: a collection of functions that will be a useful tool for your curriculum. |

### Technical considerations

-   Declaring global variables is strictly forbidden.
-   If you need helper functions to break down a more complex function, define them as static functions to restrict their scope to the appropriate file.
-   All files must be placed at the root of your repository.
-   Submitting unused files is not allowed.
-   Every .c file must compile with the following flags: **-Wall -Wextra -Werror**.
-   You must use the **ar** command to create your library. The use of `libtool` is strictly forbidden.
-   Your **libft.a** must be created at the root of your repository.

### Part 1 - Libc functions

To begin, you must reimplement a set of functions from the libc. Your version will have the same prototypes and behaviors as the originals, adhering strictly to their definitions in the man page. The only difference will be their names, which must start with the **’ft_’** prefix. For example, **strlen** becomes **ft_strlen**.

> 💡 **HINT**
>
> Some of the function prototypes you need to reimplement use the 'restrict' qualifier. This keyword is part of the C99 standard. Therefore, it is forbidden to include it in your own prototypes and to compile your code with the `-std=c99` flag.

You must write your own functions that implement the following originals. They must not rely on any external functions.

>   ℹ **INFO**
>
>   For the character classification functions (isalpha, isdigit, isalnum, isascii, isprint), the return value must be:
>
>   -   1 if the character matches the tested class
>   -   0 if the character does not match

-   isalpha
-    isdigit
-    isalnum
-    isascii
-    isprint
-    strlen
-    memset
-    bzero
-    memcpy
-    memmove
-    strlcpy
-    strlcat
-    toupper
-    tolower
-    strchr
-    strrchr
-    strncmp
-    memchr
-    memcmp
-    strnstr
-    atoi

In order to implement the two following functions, you will use malloc():

-   calloc
-   strdup

>   💡 **HINT**
>
>   Some of the function prototypes you need to reimplement use the 'restrict' qualifier. This keyword is part of the C99 standard. Therefore, it is forbidden to include it in your own prototypes and to compile your code with the `-std=c99` flag.

>   💡 **HINT**
>
>   Some functions that you must reimplement, such as strlcpy, strlcat, and bzero, are not included by default in the GNU C Library (glibc).
>   To test them against the system standard, you may need to include <bsd/string.h> and compile with the -lbsd flag.
>   This behaviour is specific to glibc systems. If you are curious, take the opportunity to explore the differences between glibc and BSD libc.

### Part 2 - Additional functions

In this second part, you must develop a set of functions that are either not included in the libc, or exist in a different form.

>   💡 **HINT**
>
>   Some of the functions from Part 1 may be useful for implementing the functions below.

| Function Name     | ft_substr                                                    |
| ----------------- | ------------------------------------------------------------ |
| Prototype         | char *ft_substr(char const *s, unsigned int start, size_t len); |
| Files to Submit   | -                                                            |
| Parameters        | s: The original string from which to create the substring.<br />start: The starting index of the substring within ’s’.<br/>len: The maximum length of the substring. |
| Return Value      | The substring.<br/>NULL if the allocation fails.             |
| External Function | malloc                                                       |
| Description       | Allocates memory (using malloc(3)) and returns a substring from the string ’s’.<br/>The substring starts at index ’start’ and has a maximum length of ’len’. |

| Function Name     | ft_strjoin                                                   |
| ----------------- | ------------------------------------------------------------ |
| Prototype         | char *ft_strjoin(char const *s1, char const *s2);            |
| Files to Submit   | -                                                            |
| Parameters        | s1: The prefix string.<br/>s2: The suffix string.            |
| Return Value      | The new string.<br/>NULL if the allocation fails             |
| External Function | malloc                                                       |
| Description       | Allocates memory (using malloc(3)) and returns a new string, which is the result of concatenating ’s1’ and ’s2’. |

| Function Name     | ft_strtrim                                                   |
| ----------------- | ------------------------------------------------------------ |
| Prototype         | char *ft_strtrim(char const *s1, char const *set);           |
| Files to Submit   | -                                                            |
| Parameters        | s1: The string to be trimmed.<br/>set: The string containing the set of characters to be removed. |
| Return Value      | The trimmed string.<br/>NULL if the allocation fails.        |
| External Function | malloc                                                       |
| Description       | Allocates memory (using malloc(3)) and returns a copy of ’s1’ with characters from ’set’ removed from the beginning and the end. |

| Function Name     | ft_split                                                     |
| ----------------- | ------------------------------------------------------------ |
| Prototype         | char **ft_split(char const *s, char c);                      |
| Files to Submit   | -                                                            |
| Parameters        | s: The string to be split.<br/>c: The delimiter character.   |
| Return Value      | The array of new strings resulting from the split. NULL if the allocation fails. |
| External Function | malloc, free                                                 |
| Description       | Allocates memory (using malloc(3)) and returns an array of strings obtained by splitting ’s’ using the character ’c’ as a delimiter. The array must end with a NULL pointer. |

| Function Name     | ft_itoa                                                      |
| ----------------- | ------------------------------------------------------------ |
| Prototype         | char *ft_itoa(int n);                                        |
| Files to Submit   |                                                              |
| Parameters        | n: The integer to convert.                                   |
| Return Value      | The string representing the integer.<br/>NULL if the allocation fails. |
| External Function | malloc                                                       |
| Description       | Allocates memory (using malloc(3)) and returns a string representing the integer received as an argument. Negative numbers must be handled. |

| Function Name     | ft_strmapi                                                   |
| ----------------- | ------------------------------------------------------------ |
| Prototype         | char *ft_strmapi(char const *s, char (*f)(unsigned int, char)); |
| Files to Submit   | -                                                            |
| Parameters        | s: The string to iterate over.<br/>f: The function to apply to each character. |
| Return Value      | The string created from the successive applications of ’f’.<br />Returns NULL if the allocation fails. |
| External Function | malloc                                                       |
| Description       | Applies the function f to each character of the string s, passing its index as the first argument and the character itself as the second. A new string is created (using malloc(3)) to store the results from the successive applications of f. |

| Function Name     | ft_striteri                                                  |
| ----------------- | ------------------------------------------------------------ |
| Prototype         | void ft_striteri(char *s, void (*f)(unsigned int, char*));   |
| Files to Submit   | -                                                            |
| Parameters        | s: The string to iterate over.<br/>f: The function to apply to each character. |
| Return Value      | None                                                         |
| External Function | None                                                         |
| Description       | Applies the function ’f’ to each character of the string passed as argument, passing its index as the first argument. Each character is passed by address to ’f’ so it can be modified if necessary |

| Function Name     | ft_putchar_fd                                                |
| ----------------- | ------------------------------------------------------------ |
| Prototype         | void ft_putchar_fd(char c, int fd);                          |
| Files to Submit   | -                                                            |
| Parameters        | c: The character to output.<br/>fd: The file descriptor on which to write. |
| Return Value      | None                                                         |
| External Function | write                                                        |
| Description       | Outputs the character ’c’ to the specified file<br/>descriptor. |

| Function Name     | ft_putstr_fd                                                 |
| ----------------- | ------------------------------------------------------------ |
| Prototype         | void ft_putstr_fd(char *s, int fd);                          |
| Files to Submit   | -                                                            |
| Parameters        | s: The string to output.<br/>fd: The file descriptor on which to write. |
| Return Value      | None                                                         |
| External Function | write                                                        |
| Description       | Outputs the string ’s’ to the specified file descriptor.     |

| Function Name     | ft_putendl_fd                                                |
| ----------------- | ------------------------------------------------------------ |
| Prototype         | void ft_putendl_fd(char *s, int fd);                         |
| Files to Submit   | -                                                            |
| Parameters        | s: The string to output.<br/>fd: The file descriptor on which to write. |
| Return Value      | None                                                         |
| External Function | write                                                        |
| Description       | Outputs the string ’s’ to the specified file descriptor followed by a newline. |

| Function Name     | ft_putnbr_fd                                                 |
| ----------------- | ------------------------------------------------------------ |
| Prototype         | void ft_putnbr_fd(int n, int fd);                            |
| Files to Submit   | -                                                            |
| Parameters        | n: The integer to output.<br/>fd: The file descriptor on which to write. |
| Return Value      | None                                                         |
| External Function | write                                                        |
| Description       | Outputs the integer ’n’ to the specified file descriptor.    |

### Part 3 - Linked list

Memory and string manipulation functions are useful. But you will soon discover that manipulating lists is even more useful.

In this third part, you have to implement functions using a structure to manipulate linked list. To do so, add the following structure declaration to your `libft.h` file:

```c
typedef struct		s_list
{
	void 			*content;
	struct s_list 	*next;
} 					t_list;
```

The members of the `t_list` struct are:

-   content: The data contained in the node.
    Using `void *` allows you to store any type of data.
-   next: The address of the next node, or `NULL` if the current node is the last one. Implement the following functions in order to easily use your lists:

| Function Name     | ft_lstnew                                                    |
| ----------------- | ------------------------------------------------------------ |
| Prototype         | t_list *ft_lstnew(void *content);                            |
| Files to Submit   | -                                                            |
| Parameters        | content: The content to store in the new node.               |
| Return Value      | A pointer to the new node                                    |
| External Function | malloc                                                       |
| Description       | Allocates memory (using malloc(3)) and returns a new node. The ’content’ member variable is initialized with the given parameter ’content’. The variable ’next’ is initialized to NULL. |

| Function Name     | ft_lstadd_front                                              |
| ----------------- | ------------------------------------------------------------ |
| Prototype         | void ft_lstadd_front(t_list **lst, t_list *new);             |
| Files to Submit   |                                                              |
| Parameters        | lst: The address of a pointer to the first node of a list.<br/>new: The address of a pointer to the node to be added. |
| Return Value      | None                                                         |
| External Function | None                                                         |
| Description       | Adds the node ’new’ at the beginning of the list.            |

| Function Name     | ft_lstsize                              |
| ----------------- | --------------------------------------- |
| Prototype         | int ft_lstsize(t_list *lst);            |
| Files to Submit   | -                                       |
| Parameters        | lst: The beginning of the list.         |
| Return Value      | The length of the list                  |
| External Function | None                                    |
| Description       | Counts the number of nodes in the list. |

| Function Name     | ft_lstlast                         |
| ----------------- | ---------------------------------- |
| Prototype         | t_list *ft_lstlast(t_list *lst);   |
| Files to Submit   | -                                  |
| Parameters        | lst: The beginning of the list.    |
| Return Value      | Last node of the list              |
| External Function | None                               |
| Description       | Returns the last node of the list. |

| Function Name     | ft_lstadd_back                                               |
| ----------------- | ------------------------------------------------------------ |
| Prototype         | void ft_lstadd_back(t_list **lst, t_list *new);              |
| Files to Submit   | -                                                            |
| Parameters        | lst: The address of a pointer to the first node of a list.<br/>new: The address of a pointer to the node to be added. |
| Return Value      | None                                                         |
| External Function | None                                                         |
| Description       | Adds the node ’new’ at the end of the list.                  |

| Function Name     | ft_lstdelone                                                 |
| ----------------- | ------------------------------------------------------------ |
| Prototype         | void ft_lstdelone(t_list *lst, void (*del)(void *));         |
| Files to Submit   | -                                                            |
| Parameters        | lst: The node to free.<br/>del: The address of the function used to delete the content. |
| Return Value      | None                                                         |
| External Function | free                                                         |
| Description       | Takes a node as parameter and frees its content using the function ’del’. Free the node itself but does NOT free the next node. |

| Function Name     | ft_lstclear                                                  |
| ----------------- | ------------------------------------------------------------ |
| Prototype         | void ft_lstclear(t_list **lst, void (*del)(void *));         |
| Files to Submit   | -                                                            |
| Parameters        | lst: The address of a pointer to a node.<br/>del: The address of the function used to delete the content of the node. |
| Return Value      | None                                                         |
| External Function | free                                                         |
| Description       | Deletes and frees the given node and all its successors, using the function ’del’ and free(3). Finally, set the pointer to the list to NULL. |

| Function Name     | ft_lstiter                                                   |
| ----------------- | ------------------------------------------------------------ |
| Prototype         | void ft_lstiter(t_list *lst, void (*f)(void *));             |
| Files to Submit   | -                                                            |
| Parameters        | lst: The address of a pointer to a node.<br/>f: The address of the function to apply to each<br/>node’s content. |
| Return Value      | None                                                         |
| External Function | None                                                         |
| Description       | Iterates through the list ’lst’ and applies the function ’f’ to the content of each node. |

| Function Name     | ft_lstmap                                                    |
| ----------------- | ------------------------------------------------------------ |
| Prototype         | t_list *ft_lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *)); |
| Files to Submit   | -                                                            |
| Parameters        | lst: The address of a pointer to a node.<br/>f: The address of the function applied to each node’s content.<br/>del: The address of the function used to delete a node’s content if needed. |
| Return Value      | The new list.<br/>NULL if the allocation fails.              |
| External Function | malloc, free                                                 |
| Description       | Iterates through the list ’lst’, applies the function ’f’ to each node’s content, and creates a new list resulting of the successive applications of the function ’f’. The ’del’ function is used to delete the content of a node if needed. |

### Readme Requirements

A README.md file must be provided at the root of your Git repository. Its purpose is to allow anyone unfamiliar with the project (peers, staff, recruiters, etc.) to quickly understand what the project is about, how to run it, and where to find more information on the topic.
The README.md must include at least:

-   The very first line must be italicized and read: This project has been created as part of the 42 curriculum by <login1>[, <login2>[, <login3>[...]]].
-   A “Description” section that clearly presents the project, including its goal and a brief overview.
-   An “Instructions” section containing any relevant information about compilation, installation, and/or execution.
-   A “Resources” section listing classic references related to the topic (documentation, articles, tutorials, etc.), as well as a description of how AI was used — specifying for which tasks and which parts of the project.

➠ Additional sections may be required depending on the project (e.g., usage examples, feature list, technical choices, etc.).
Any required additions will be explicitly listed below.

-   A detailed description of the library created for this project must also be included.

>   ℹ **INFO**
>
>   The choice of language is at your discretion. It is recommended towrite in English, but it is not mandatory.
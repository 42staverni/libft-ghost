export type LibftPart = 'Part 1' | 'Part 2' | 'Part 3';

export const PART_ORDER: LibftPart[] = ['Part 1', 'Part 2', 'Part 3'];

const PART_1_FUNCTIONS = new Set([
    'ft_isalpha',
    'ft_isdigit',
    'ft_isalnum',
    'ft_isascii',
    'ft_isprint',
    'ft_strlen',
    'ft_memset',
    'ft_bzero',
    'ft_memcpy',
    'ft_memmove',
    'ft_strlcpy',
    'ft_strlcat',
    'ft_toupper',
    'ft_tolower',
    'ft_strchr',
    'ft_strrchr',
    'ft_strncmp',
    'ft_memchr',
    'ft_memcmp',
    'ft_strnstr',
    'ft_atoi',
    'ft_calloc',
    'ft_strdup'
]);

export function getFunctionPart(functionName: string): LibftPart {
    if (functionName.startsWith('ft_lst')) return 'Part 3';
    if (PART_1_FUNCTIONS.has(functionName)) return 'Part 1';
    return 'Part 2';
}

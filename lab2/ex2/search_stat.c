#include <stdio.h>
#include <string.h>
#include <asm/errno.h>
#include <errno.h>

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        errno = EINVAL;
        perror("Usage: ./search_stat path/to/dir (> or < or =) date");
        return 1;
    }

    char* dir = argv[1];
    char* sign = argv[2];
    if (strlen(sign) != 1) {
        errno = EINVAL;
        perror("Usage: ./search_stat path/to/dir (> or < or =) date");
        return 2;
    }
    char* date = argv[3];
    switch (sign[0]) {
        case '<':
        case '>':
        case '=':
        default:
            errno = EINVAL;
            perror("Only <, > and = are supported as a second argument.");
            return 3;
    }
}
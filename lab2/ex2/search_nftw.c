#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <string.h>
#include <asm/errno.h>
#include <sys/stat.h>
#include <ftw.h>
#include <errno.h>
#include "utils.c"

static int (*compare)(char *a, char *b) = NULL;
static char *input_date = NULL;

int print_entry(const char *fpath, const struct stat *stat, int typeflag, struct FTW *ftwbuf)
{
    char *mod_time = NULL;
    // if regular file and modification date before/equal/after date provided
    if (typeflag == FTW_F && compare((mod_time = format_time(stat->st_mtime)), input_date) == 0)
    {
        const char *abs_path = fpath;
        if (fpath[0] != '/') {
            abs_path = to_absolute(fpath);
        }
        print_file_details(abs_path, mod_time, stat);
    }
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        errno = EINVAL;
        perror("Usage: ./search_nftw path/to/dir (> or < or =) date");
        return 1;
    }
    char *dir = argv[1];
    char *sign = argv[2];
    if (strlen(sign) != 1)
    {
        errno = EINVAL;
        perror("Usage: ./search_stat path/to/dir (> or < or =) date");
        return 2;
    }
    char *date = argv[3];
    input_date = date;
    switch (sign[0])
    {
        case '<':
            compare = &less_than;
            break;
        case '>':
            compare = &greater_than;
            break;
        case '=':
            compare = &equal;
            break;
        default:
            errno = EINVAL;
            perror("Only <, > and = are supported as a second argument.");
            return 3;
    }
    return nftw(dir, print_entry, 1024, FTW_PHYS);
}
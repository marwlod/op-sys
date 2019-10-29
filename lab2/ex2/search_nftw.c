#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <string.h>
#include <asm/errno.h>
#include <errno.h>
#include <ftw.h>



int file_before_date(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    return 0;
}

int file_at_date(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    return 0;
}

int file_after_date(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
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
    if (strlen(sign) != 1) {
        errno = EINVAL;
        perror("Usage: ./search_stat path/to/dir (> or < or =) date");
        return 2;
    }
    char *date = argv[3];
    switch (sign[0]) {
        case '<':
            return nftw(dir, file_before_date, 1024, FTW_F);
        case '>':
            return nftw(dir, file_after_date, 1024, FTW_F);
        case '=':
            return nftw(dir, file_at_date, 1024, FTW_F);
        default:
            errno = EINVAL;
            perror("Only <, > and = are supported as a second argument.");
            return 3;
    }

}
#include <time.h>
#include <string.h>
#include <zconf.h>

int less_than(char *a, char *b)
{
    return strcmp(a, b) < 0 ? 0 : 1;
}

int greater_than(char *a, char *b)
{
    return strcmp(a, b) > 0 ? 0 : 1;
}

int equal(char *a, char *b)
{
    return strcmp(a, b) == 0 ? 0 : 1;
}

// from time_t to local yyyy-MM-dd hh:mm:ss
char *format_time(time_t time)
{
    static char timestring[20];
    struct tm *tm = localtime(&time);
    strftime(timestring, sizeof(timestring), "%Y-%m-%d %H:%M:%S", tm);
    return timestring;
}

// from timespec to local yyyy-MM-dd hh:mm:ss
char *format_timespec(struct timespec timespec)
{
    static char timestring[20];
    struct tm *tm = localtime(&timespec.tv_sec);
    strftime(timestring, sizeof(timestring), "%Y-%m-%d %H:%M:%S", tm);
    return timestring;
}

int validate_date(char *date)
{
    int n[6] = {0};
    sscanf(date, "%*[0-9]%n-%*[0-9]%n-%*[0-9]%n %*[0-9]%n:%*[0-9]%n:%*[0-9]%n",
           &n[0], &n[1], &n[2], &n[3], &n[4], &n[5]);
    if (n[0] != 4 || n[1] != 7 || n[2] != 10 || n[3] != 13 || n[4] != 16 || n[5] != 19)
    {
        errno = EINVAL;
        perror("Date format is invalid: 'yyyy-MM-dd hh:mm:ss'");
        return 5;
    }
    return 0;
}

void print_file_details(const char *file_abs_path, char *mod_time, const struct stat *stat)
{
    printf("Permissions: ");
    printf(stat->st_mode & (unsigned int) S_IRUSR ? "r" : "-");
    printf(stat->st_mode & (unsigned int) S_IWUSR ? "w" : "-");
    printf(stat->st_mode & (unsigned int) S_IXUSR ? "x" : "-");
    printf(stat->st_mode & (unsigned int) S_IRGRP ? "r" : "-");
    printf(stat->st_mode & (unsigned int) S_IWGRP ? "w" : "-");
    printf(stat->st_mode & (unsigned int) S_IXGRP ? "x" : "-");
    printf(stat->st_mode & (unsigned int) S_IROTH ? "r" : "-");
    printf(stat->st_mode & (unsigned int) S_IWOTH ? "w" : "-");
    printf(stat->st_mode & (unsigned int) S_IXOTH ? "x" : "-");
    printf(", last modified: %s, path: %s, size: %ld\n",
           mod_time, file_abs_path, stat->st_size);
}

char *to_absolute(const char *fpath) {
    char cwd[500];
    static char abs_path[600];
    getcwd(cwd, sizeof(cwd));
    strcpy(abs_path, cwd);
    if (fpath[strlen(fpath)-1] != '/') {
        strcat(abs_path, "/");
    }
    strcat(abs_path, fpath);
    return abs_path;
}
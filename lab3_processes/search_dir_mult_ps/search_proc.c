#include <stdio.h>
#include <string.h>
#include <asm/errno.h>
#include <errno.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <wait.h>
#include "utils.c"

// similar to lab2_file_system/libraries but search each new directory in a separate process
int deep_search_files(char *dirname, int compare_func(char *, char *), char *date)
{
    DIR *dir = opendir(dirname);
    if (dir == NULL)
    {
        errno = EIO;
        perror("Directory doesn't exist");
        return 1;
    }
    struct stat *st = (struct stat *) malloc(sizeof(struct stat));
    struct dirent *dirent = NULL;
    char *new_path = NULL;
    while ((dirent = readdir(dir)))
    {
        // create path to dir-entry
        new_path = (char *) malloc(strlen(dirname) + sizeof("/") + strlen(dirent->d_name));
        strcpy(new_path, dirname);
        if (dirname[strlen(dirname)-1] != '/')
        {
            strcat(new_path, "/");
        }
        strcat(new_path, dirent->d_name);
        // if new directory found search it for files
        if (dirent->d_type == DT_DIR && strcmp(dirent->d_name, ".") != 0 && strcmp(dirent->d_name, "..") != 0)
        {
            pid_t cpid = fork();
            // parent process -> wait until child dies, check return status
            if (cpid)
            {
                int cstat;
                waitpid(cpid, &cstat, 0);
                if (cstat)
                {
                    perror("Something went wrong during searching directories");
                    return 2;
                }

            }
            else
            {
                printf("Child process. Dir: %s, PPID: %d, PID: %d\n", dirent->d_name, getppid(), getpid());
                deep_search_files(new_path, compare_func, date);
                free(st);
                free(new_path);
                closedir(dir);
                return 0;
            }
        }
        else if (dirent->d_type == DT_REG)
        {
            stat(new_path, st);
            // check if the last modified date of the file is before/equal/after date provided
            if (compare_func(format_timespec(st->st_mtim), date) == 0)
            {
                char *abs_path = new_path;
                if (new_path[0] != '/')
                {
                    abs_path = to_absolute(new_path);
                }
                print_file_details(abs_path, format_timespec(st->st_mtim), st);
            }
        }
        free(new_path);
    }
    free(st);
    closedir(dir);
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        errno = EINVAL;
        perror("Usage: ./search_stat path/to/dir ('>' or '<' or '=') 'yyyy-MM-dd hh:mm:ss'");
        return 1;
    }

    char *dirname = argv[1];
    char *sign = argv[2];
    if (strlen(sign) != 1)
    {
        errno = EINVAL;
        perror("Usage: ./search_stat path/to/dir ('>' or '<' or '=') 'yyyy-MM-dd hh:mm:ss'");
        return 2;
    }
    char *date = argv[3];
    if (validate_date(date) != 0)
    {
        return 3;
    }
    switch (sign[0])
    {
        case '<':
            return deep_search_files(dirname, less_than, date);
        case '>':
            return deep_search_files(dirname, greater_than, date);
        case '=':
            return deep_search_files(dirname, equal, date);
        default:
            errno = EINVAL;
            perror("Only '<', '>' and '=' are supported as a second argument.");
            return 4;
    }
}
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zconf.h>
#include <wait.h>
#include <ctype.h>

static const int MAX_ARGS = 5;
static const int MAX_PROGS_PIPED = 6;
static const int BUFFER_SIZE = 1024;

// modifies source string
void trim_whitespace(char **str)
{
    if (strlen(*str) == 0) {
        return;
    }

    // leading
    int leadingCounter = 0;
    char *trav = *str;
    while (isspace((unsigned char) *trav))
    {
        leadingCounter++;
        trav++;
    }

    // trailing
    char *end = *str + strlen(*str) - 1;
    while (isspace((unsigned char) *end))
    {
        end--;
    }
    end[1] = '\0';
    memmove(*str, *str + leadingCounter, strlen(*str));
}

char **tokenize(char *str)
{
    // max programs * (program name + args + NULL)
    char **tokens = (char **) calloc(MAX_PROGS_PIPED * (MAX_ARGS + 2), sizeof(char *));
    unsigned long tstart = 0;
    int tcounter = 0;
    short is_quoted = 0;
    for (unsigned long i = 0; i < strlen(str); i++)
    {
        if (str[i] == '\"')
        {
            // ending quote
            if (is_quoted)
            {
                tokens[tcounter] = (char *) calloc(BUFFER_SIZE, sizeof(char));
                // trim quotes
                strncpy(tokens[tcounter++], str + tstart, i - tstart);
                // next token starts after quote and space
                tstart += i - tstart + 2;
                i++;
                is_quoted = 0;
            }
            // starting quote
            else
            {
                is_quoted = 1;
                tstart++;
            }
        }
        else if (str[i] == ' ' && !is_quoted)
        {
            tokens[tcounter] = (char *) calloc(BUFFER_SIZE, sizeof(char));
            // copy whole word up to the point of space char
            strncpy(tokens[tcounter++], str + tstart, i - tstart);
            // starting index of new word is after space char
            tstart += i - tstart + 1;

        }
        // last token
        else if (i == strlen(str) - 1)
        {
            tokens[tcounter] = (char *) calloc(BUFFER_SIZE, sizeof(char));
            strncpy(tokens[tcounter++], str + tstart, i - tstart + 1);
        }
    }
    return tokens;
}

void free_dp(char **dp) {
    int i = 0;
    while (dp[i])
    {
        free(dp[i++]);
    }
    free(dp);
}

void free_pargs(char ***pargs) {
    for (int i = 0; i < MAX_PROGS_PIPED + 1; i++) {
        free_dp(pargs[i]);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        errno = EINVAL;
        perror("Usage: ./cmd_interpreter path/to/file.txt");
        return 1;
    }

    char *filename = argv[1];
    FILE *file = fopen(filename, "r+");
    if (!file)
    {
        errno = EIO;
        perror("Opening of file failed");
        return 2;
    }

    char *line = NULL;
    size_t len = 0;
    long linenum = 1;

    while (getline(&line, &len, file) != -1)
    {
        trim_whitespace(&line);
        char **tokens = tokenize(line);
        // max programs to pipe + NULL
        char **pargs[MAX_PROGS_PIPED + 1];
        for (int i = 0; i < MAX_PROGS_PIPED + 1; i++)
        {
            // program name + args + NULL
            pargs[i] = (char **) calloc(MAX_ARGS + 2, sizeof(char *));
        }

        int progNum = 0;
        int argNum = 0;
        char **ttrav = tokens;
        while (*ttrav)
        {
            if (strcmp(*ttrav, "|") != 0)
            {
                pargs[progNum][argNum] = (char *) calloc(BUFFER_SIZE, sizeof(char));
                memcpy(pargs[progNum][argNum++], *ttrav, strlen(*ttrav));
            }
            else
            {
                argNum = 0;
                progNum++;
            }
            ttrav++;
        }
        free_dp(tokens);
        int totalProgs = progNum + 1;

        int fds[2 * totalProgs];
        for (int i = 0; i < totalProgs; i++)
        {
            if (pipe(fds + 2 * i))
            {
                perror("Pipe creation failed");
                free(line);
                fclose(file);
                free_pargs(pargs);
                return 3;
            }
        }
        int currp = 0;
        while (*pargs[currp])
        {
            pid_t cpid = fork();
            if (cpid < 0)
            {
                fprintf(stderr, "Execution of program %s from line %ld failed\n", pargs[currp][0], linenum);
                free(line);
                fclose(file);
                free_pargs(pargs);
                return 4;
            }
            // child process
            else if (cpid == 0)
            {
                // if not last program
                if (*pargs[currp+1])
                {
                    if (dup2(fds[currp * 2 + 1], STDOUT_FILENO) < 0)
                    {
                        fprintf(stderr, "dup2 failed when trying to redirect output of current program to STDOUT\n");
                        free(line);
                        fclose(file);
                        free_pargs(pargs);
                        return 5;
                    }
                }
                // if not first program
                if (currp > 0)
                {
                    if (dup2(fds[(currp-1) * 2], STDIN_FILENO) < 0)
                    {
                        fprintf(stderr, "dup2 failed when trying to redirect input of previous program to STDIN\n");
                        free(line);
                        fclose(file);
                        free_pargs(pargs);
                        return 6;
                    }
                }
                // close all fds for pipes
                for (int i = 0; i < 2 * totalProgs; i++) close(fds[i]);

                printf("%ld.%d. Executing '%s', PPID: %d, PID: %d\n", linenum, currp, line, getppid(), getpid());
                int status = execvp(pargs[currp][0], pargs[currp]);
                if (status < 0)
                {
                    perror(pargs[currp][0]);
                    free(line);
                    fclose(file);
                    free_pargs(pargs);
                    return 7;
                }
            }
            currp++;
        }

        for (int i = 0; i < 2 * totalProgs; i++) close(fds[i]);
        for (int i = 0; i < totalProgs; i++) wait(NULL);
        linenum++;
        free_pargs(pargs);
    }
    free(line);
    fclose(file);
    return 0;
}

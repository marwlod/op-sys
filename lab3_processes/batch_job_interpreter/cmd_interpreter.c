#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zconf.h>
#include <wait.h>
#include <ctype.h>

static const int MAX_ARGS = 5;
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
    // program name + args + NULL
    char **tokens = (char **) calloc(MAX_ARGS + 2, sizeof(char *));
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
        char **pargs = tokenize(line);

        pid_t cpid = fork();
        // parent process -> wait until child dies, check return status
        if (cpid)
        {
            int cstat;
            waitpid(cpid, &cstat, 0);
            if (cstat)
            {
                fprintf(stderr, "Execution of program %s from line %ld failed", pargs[0], linenum);
                free(line);
                fclose(file);
                free_dp(pargs);
                return 3;
            }

        }
        // child process -> execute saved command with args
        else
        {
            printf("%ld. Executing %s command, PPID: %d, PID: %d\n", linenum, pargs[0], getppid(), getpid());
            int status = execvp(pargs[0], pargs);
            if (status < 0)
            {
                perror(pargs[0]);
                free(line);
                fclose(file);
                free_dp(pargs);
                return 4;
            }
        }
        linenum++;
        free_dp(pargs);
    }
    free(line);
    fclose(file);
    return 0;
}

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/times.h>
#include "utils.c"

// generate (records * record_size) amount of random bytes to a file
int generate(char *filename, long records, long record_size)
{
    int file = open(filename, O_WRONLY | O_CREAT| O_TRUNC, S_IRUSR | S_IWUSR);
    if (file < 0)
    {
        perror("Opening of file failed");
        return -1;
    }
    int random_source = open("/dev/urandom", O_RDONLY);
    if (random_source < 0)
    {
        close(file);
        perror("Opening of random generator failed");
        return -1;
    }

    for (long i = 0; i < records; i++)
    {
        char random_data[record_size];
        size_t data_read = 0;
        while (data_read < sizeof(random_data))
        {
            // read at most size of record, if previous call to read() read only part of data,
            // now try to read size of record reduced by the total amount already read
            ssize_t result = read(random_source, random_data + data_read, sizeof(random_data) - data_read);
            if (result < 0)
            {
                close(file);
                close(random_source);
                perror("Random values generation failed");
                return -1;
            }
            data_read += result;
        }
        ssize_t data_written = write(file, random_data, sizeof(random_data));
        if (data_written < sizeof(random_data))
        {
            close(file);
            close(random_source);
            perror("Writing random values to a file failed");
            return -1;
        }
    }
    close(file);
    close(random_source);
    return 0;
}

// insertion sort of records in a file (sort key is the first byte of the record interpreted as unsigned char)
// using only system functions
int sort_sys(char *filename, long records, long record_size)
{
    int fd = open(filename, O_RDWR, S_IRUSR | S_IWUSR);
    if (fd < 0)
    {
        perror("Opening of file failed");
        return -1;
    }
    char first_record[record_size];
    char second_record[record_size];
    for (long i = 1; i < records; i++)
    {
        for (long j = i; j > 0; j--)
        {
            // read two records and get their first chars to compare
            int data_read = read_starting_from(fd, first_record, sizeof(first_record), (j-1)*record_size);
            if (data_read < 0)
            {
                close(fd);
                perror("Couldn't read record from file");
                return -1;
            }
            data_read = read_starting_from(fd, second_record, sizeof(second_record), j*record_size);
            if (data_read < 0)
            {
                close(fd);
                perror("Couldn't read record from file");
                return -1;
            }
            unsigned char char_first_record = first_record[0];
            unsigned char char_second_record = second_record[0];
            // compare first chars of records, if not in ascending order, swap them, otherwise leave them in current order
            if (char_first_record > char_second_record)
            {
                int data_written = write_starting_from(fd, second_record, sizeof(second_record), (j-1)*record_size);
                if (data_written < 0)
                {
                    close(fd);
                    perror("Couldn't write a record to the file");
                    return -1;
                }
                data_written = write_starting_from(fd, first_record, sizeof(first_record), j*record_size);
                if (data_written < 0)
                {
                    close(fd);
                    perror("Couldn't write a record to the file");
                    return -1;
                }
            }
        }
    }
    close(fd);
    return 0;
}

// insertion sort of records in a file (sort key is the first byte of the record interpreted as unsigned char)
// using only C standard library functions
int sort_lib(char *filename, long records, long record_size)
{
    FILE *file = fopen(filename, "r+");
    if (file < 0)
    {
        perror("Opening of file failed.");
        return -1;
    }
    char first_record[record_size];
    char second_record[record_size];
    for (long i = 1; i < records; i++)
    {
        for (long j = i; j > 0; j--)
        {
            // read two records and get their first chars to compare
            int data_read = fread_starting_from(file, first_record, sizeof(first_record), (j-1)*record_size);
            if (data_read < 0)
            {
                fclose(file);
                perror("Couldn't read record from file");
                return -1;
            }
            data_read = fread_starting_from(file, second_record, sizeof(second_record), j*record_size);
            if (data_read < 0)
            {
                fclose(file);
                perror("Couldn't read record from file");
                return -1;
            }
            unsigned char char_first_record = first_record[0];
            unsigned char char_second_record = second_record[0];
            // compare first chars of records, if not in ascending order, swap them, otherwise leave them in current order
            if (char_first_record > char_second_record)
            {
                int data_written = fwrite_starting_from(file, second_record, sizeof(second_record), (j-1)*record_size);
                if (data_written < 0)
                {
                    fclose(file);
                    perror("Couldn't write a record to the file");
                    return -1;
                }
                data_written = fwrite_starting_from(file, first_record, sizeof(first_record), j*record_size);
                if (data_written < 0)
                {
                    fclose(file);
                    perror("Couldn't write a record to the file");
                    return -1;
                }
            }
        }
    }
    fclose(file);
    return 0;
}

// copy num of records of buffer_size from file using only system functions
int copy_sys(char *src_filename, char *dst_filename, long records, long buffer_size)
{
    int src_fd = open(src_filename, O_RDONLY, S_IRUSR | S_IWUSR);
    if (src_fd < 0)
    {
        perror("Opening of source file failed");
        return -1;
    }
    int dst_fd = open(dst_filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (dst_fd < 0)
    {
        close(src_fd);
        perror("Opening of destination file failed");
        return -1;
    }
    char buffer[buffer_size];
    for (long i = 0; i < records; i++)
    {
        int bytes_read = read(src_fd, buffer, buffer_size);
        if (bytes_read > 0)
        {
            int bytes_written = write(dst_fd, buffer, bytes_read);
            if (bytes_written != bytes_read) {
                close(src_fd);
                close(dst_fd);
                errno = EIO;
                perror("Writing to the destination file failed");
                return -1;
            }
        }
    }
    close(src_fd);
    close(dst_fd);
    return 0;
}

// copy num of records of buffer_size from file using only C standard library functions
int copy_lib(char *src_filename, char *dst_filename, long records, long buffer_size)
{
    FILE *src_file = fopen(src_filename, "r");
    if (!src_file)
    {
        perror("Opening of source file failed");
        return -1;
    }
    FILE *dst_file = fopen(dst_filename, "w");
    if (!dst_file)
    {
        fclose(src_file);
        perror("Opening of destination file failed");
        return -1;
    }
    char buffer[buffer_size];
    for (long i = 0; i < records; i++)
    {
        unsigned long bytes_read = fread(buffer, sizeof(char), buffer_size, src_file);
        if (bytes_read > 0)
        {
            unsigned long bytes_written = fwrite(buffer, sizeof(char), bytes_read, dst_file);
            if (bytes_written != bytes_read) {
                fclose(src_file);
                fclose(dst_file);
                errno = EIO;
                perror("Writing to the destination file failed");
                return -1;
            }
        }
    }
    fclose(src_file);
    fclose(dst_file);
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        errno = EINVAL;
        perror("Program needs more arguments. Functions supported are: generate, sort and copy");
        return 1;
    }

    struct tms *time_before = (struct tms *) malloc(sizeof(struct tms));
    struct tms *time_after = (struct tms *) malloc(sizeof(struct tms));
    times(time_before);


    if (strcmp(argv[1], "generate") == 0)
    {
        if (argc != 5)
        {
            free(time_before);
            free(time_after);
            errno = EINVAL;
            perror("Usage: ./program generate filename num_of_records record_size");
            return 2;
        }
        long num_of_records = strtol(argv[3], NULL, 10);
        long record_size = strtol(argv[4], NULL, 10);
        if (num_of_records == 0 || record_size == 0)
        {
            free(time_before);
            free(time_after);
            errno = EINVAL;
            perror("Number of records and record size must be integers");
            return 3;
        }
        int generated = generate(argv[2], num_of_records, record_size);
        if (generated < 0)
        {
            free(time_before);
            free(time_after);
            perror("Generation of random data failed");
            return 4;
        }
    }

    else if (strcmp(argv[1], "sort") == 0)
    {
        if (argc != 6)
        {
            free(time_before);
            free(time_after);
            errno = EINVAL;
            perror("Usage: ./program sort filename num_of_records record_size sys_or_lib");
            return 4;
        }
        char *file_name = argv[2];
        long num_of_records = strtol(argv[3], NULL, 10);
        long record_size = strtol(argv[4], NULL, 10);
        char *function_type = argv[5];

        if (num_of_records == 0 || record_size == 0)
        {
            free(time_before);
            free(time_after);
            errno = EINVAL;
            perror("Number of records and record size must be integers");
            return 5;
        }
        if (strcmp(function_type, "sys") == 0)
        {
            int sorted = sort_sys(file_name, num_of_records, record_size);
            if (sorted < 0)
            {
                free(time_before);
                free(time_after);
                perror("Sorting using system functions failed");
                return 5;
            }
        }
        else if (strcmp(function_type, "lib") == 0)
        {
            int sorted = sort_lib(file_name, num_of_records, record_size);
            if (sorted < 0)
            {
                free(time_before);
                free(time_after);
                perror("Sorting using C library functions failed");
                return 5;
            }
        }
        else
        {
            free(time_before);
            free(time_after);
            errno = EINVAL;
            perror("Last argument must be either sys or lib");
            return 6;
        }
    }


    else if (strcmp(argv[1], "copy") == 0)
    {
        if (argc != 7)
        {
            free(time_before);
            free(time_after);
            errno = EINVAL;
            perror("Usage: ./program copy src_file dst_file num_of_records buffer_size sys_or_lib");
            return 7;
        }
        char *src_file = argv[2];
        char *dst_file = argv[3];
        long num_of_records = strtol(argv[4], NULL, 10);
        long buffer_size = strtol(argv[5], NULL, 10);
        char *function_type = argv[6];

        if (num_of_records == 0 || buffer_size == 0)
        {
            free(time_before);
            free(time_after);
            errno = EINVAL;
            perror("Number of records and buffer size must be integers");
            return 8;
        }
        if (strcmp(function_type, "sys") == 0)
        {
            int copied = copy_sys(src_file, dst_file, num_of_records, buffer_size);
            if (copied < 0)
            {
                free(time_before);
                free(time_after);
                perror("Copying using system functions failed");
                return 8;
            }
        }
        else if (strcmp(function_type, "lib") == 0)
        {
            int copied = copy_lib(src_file, dst_file, num_of_records, buffer_size);
            if (copied < 0)
            {
                free(time_before);
                free(time_after);
                perror("Copying using C library functions failed");
                return 8;
            }
        }
        else
        {
            free(time_before);
            free(time_after);
            errno = EINVAL;
            perror("Last argument must be either sys or lib");
            return 9;
        }
    }

    else
    {
        free(time_before);
        free(time_after);
        errno = EINVAL;
        perror("Only generate, sort and copy functions are supported. Usage: ./program function_name other_arguments");
        return 10;
    }

    times(time_after);
    printf("User time: %Lf s, system time: %Lf s\n", calculate_time_secs(time_before->tms_utime, time_after->tms_utime),
           calculate_time_secs(time_before->tms_stime, time_after->tms_stime));
    free(time_before);
    free(time_after);
    return 0;
}
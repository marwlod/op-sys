#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/times.h>

long double calculate_time_secs(clock_t start, clock_t end) {
    return (long double)(end - start) / (long double)sysconf(_SC_CLK_TCK);
}

int generate(char *filename, long records, long record_size)
{
    int file = open(filename, O_WRONLY | O_CREAT| O_TRUNC, S_IRUSR | S_IWUSR);
    if (file < 0) {
        perror("Opening of file failed.");
        return 11;
    }
    int random_source = open("/dev/urandom", O_RDONLY);
    if (random_source < 0)
    {
        perror("Opening of random generator failed.");
        return 12;
    }

    for (long i = 0; i < records; i++) {
        char random_data[record_size];
        size_t data_read = 0;
        while (data_read < sizeof random_data) {
            ssize_t result = read(random_source, random_data + data_read, (sizeof random_data) - data_read);
            if (result < 0)
            {
                perror("Random values generation failed.");
                return 13;
            }
            data_read += result;
        }
        write(file, random_data, record_size);
    }
    close(file);
    close(random_source);
    return 0;
}

int sort_sys(char *filename, long records, long record_size)
{
    int file = open(filename, O_RDWR, S_IRUSR | S_IWUSR);
    if (file < 0) {
        perror("Opening of file failed.");
        return 14;
    }
    char first_record[record_size];
    char second_record[record_size];
    long seeked;
    for (long i = 1; i < records; i++) {
        for (long j = i; j > 0; j--) {
            seeked = lseek(file, (j-1) * record_size, SEEK_SET);
            if (seeked < 0) {
                perror("Couldn't change position in the file.");
                close(file);
                return 15;
            }
            read(file, first_record, sizeof(first_record));
            seeked = lseek(file, j * record_size, SEEK_SET);
            if (seeked < 0) {
                perror("Couldn't change position in the file.");
                close(file);
                return 16;
            }
            read(file, second_record, sizeof(second_record));
            unsigned char first_char = first_record[0];
            unsigned char second_char = second_record[0];
            if (first_char > second_char) {
                seeked = lseek(file, (j-1) * record_size, SEEK_SET);
                if (seeked < 0) {
                    perror("Couldn't change position in the file.");
                    close(file);
                    return 18;
                }
                int first_write = write(file, second_record, sizeof(second_record));
                seeked = lseek(file, j * record_size, SEEK_SET);
                if (seeked < 0) {
                    perror("Couldn't change position in the file.");
                    close(file);
                    return 19;
                }
                int second_write = write(file, first_record, sizeof(first_record));
                if (first_write < 0 || second_write < 0) {
                    perror("Writing record to file failed.");
                    close(file);
                    return 20;
                }
            }
        }
    }
    close(file);
    return 0;
}


int sort_lib(char *filename, long records, long record_size)
{
    FILE *file = fopen(filename, "r+");
    if (file < 0) {
        perror("Opening of file failed.");
        return 21;
    }
    char first_record[record_size];
    char second_record[record_size];
    long seeked;
    for (long i = 1; i < records; i++) {
        for (long j = i; j > 0; j--) {
            seeked = fseek(file, (j-1) * record_size, SEEK_SET);
            if (seeked < 0) {
                perror("Couldn't change position in the file.");
                fclose(file);
                return 22;
            }
            fread(first_record, sizeof(first_record), 1, file);
            seeked = fseek(file, j * record_size, SEEK_SET);
            if (seeked < 0) {
                perror("Couldn't change position in the file.");
                fclose(file);
                return 23;
            }
            fread(second_record, sizeof(second_record), 1, file);
            unsigned char first_char = first_record[0];
            unsigned char second_char = second_record[0];
            if (first_char > second_char) {
                seeked = fseek(file, (j-1) * record_size, SEEK_SET);
                if (seeked < 0) {
                    perror("Couldn't change position in the file.");
                    fclose(file);
                    return 25;
                }
                unsigned long first_write = fwrite(second_record, sizeof(char), sizeof(second_record), file);
                seeked = fseek(file, j * record_size, SEEK_SET);
                if (seeked < 0) {
                    perror("Couldn't change position in the file.");
                    fclose(file);
                    return 26;
                }
                unsigned long second_write = fwrite(first_record, sizeof(char), sizeof(second_record), file);
                if (first_write < 0 || second_write < 0) {
                    perror("Writing record to file failed.");
                    fclose(file);
                    return 27;
                }
            }
        }
    }
    fclose(file);
    return 0;
}

int copy_sys(char *src_filename, char *dst_filename, long records, long buffer_size)
{
    int src_file = open(src_filename, O_RDONLY, S_IRUSR | S_IWUSR);
    if (src_file < 0) {
        perror("Opening of source file failed.");
        return 28;
    }
    int dst_file = open(dst_filename, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
    if (dst_file < 0) {
        perror("Opening of destination file failed.");
        close(src_file);
        return 29;
    }
    char buffer[buffer_size];
    int bytes_read;
    for (long i = 0; i < records; i++) {
        bytes_read = read(src_file, buffer, buffer_size);
        if (bytes_read > 0) {
            write(dst_file, buffer, bytes_read);
        }
    }
    close(src_file);
    close(dst_file);
    return 0;
}

int copy_lib(char *src_filename, char *dst_filename, long records, long buffer_size)
{
    FILE *src_file = fopen(src_filename, "r");
    if (src_file < 0) {
        perror("Opening of source file failed.");
        return 30;
    }
    FILE *dst_file = fopen(dst_filename, "w");
    if (dst_file < 0) {
        perror("Opening of destination file failed.");
        fclose(src_file);
        return 31;
    }
    char buffer[buffer_size];
    unsigned long bytes_read;
    for (long i = 0; i < records; i++) {
        bytes_read = fread(buffer, sizeof(char), buffer_size, src_file);
        if (bytes_read > 0) {
            fwrite(buffer, sizeof(char), bytes_read, dst_file);
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
        perror("Program needs more arguments. Functions supported are: generate, sort and copy.");
        return 1;
    }

    struct tms *time_before = (struct tms*) malloc(sizeof(struct tms));
    struct tms *time_after = (struct tms*) malloc(sizeof(struct tms));
    times(time_before);


    if (strcmp(argv[1], "generate") == 0)
    {
        if (argc != 5)
        {
            errno = EINVAL;
            perror("Usage: ./program generate filename num_of_records record_size");
            return 2;
        }
        long num_of_records = strtol(argv[3], NULL, 10);
        long record_size = strtol(argv[4], NULL, 10);
        if (num_of_records == 0 || record_size == 0)
        {
            errno = EINVAL;
            perror("Number of records and record size must be integers.");
            return 3;
        }
        return generate(argv[2], num_of_records, record_size);
    }

    else if (strcmp(argv[1], "sort") == 0)
    {
        if (argc != 6)
        {
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
            errno = EINVAL;
            perror("Number of records and record size must be integers.");
            return 5;
        }
        if (strcmp(function_type, "sys") == 0)
        {
            int sorted = sort_sys(file_name, num_of_records, record_size);
            if (sorted > 0) {
                perror("Sorting using system functions failed.");
                return 5;
            }
        }
        else if (strcmp(function_type, "lib") == 0)
        {
            int sorted = sort_lib(file_name, num_of_records, record_size);
            if (sorted > 0) {
                errno = EINVAL;
                perror("Sorting using C library functions failed");
                return 5;
            }
        }
        else
        {
            errno = EINVAL;
            perror("Last argument must be either sys or lib.");
            return 6;
        }
    }


    else if (strcmp(argv[1], "copy") == 0)
    {
        if (argc != 7)
        {
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
            errno = EINVAL;
            perror("Number of records and buffer size must be integers.");
            return 8;
        }
        if (strcmp(function_type, "sys") == 0)
        {
            int copied = copy_sys(src_file, dst_file, num_of_records, buffer_size);
            if (copied > 0) {
                errno = EINVAL;
                perror("Copying using system functions failed");
                return 8;
            }
        }
        else if (strcmp(function_type, "lib") == 0)
        {
            int copied = copy_lib(src_file, dst_file, num_of_records, buffer_size);
            if (copied > 0) {
                errno = EINVAL;
                perror("Copying using C library functions failed");
                return 8;
            }
        }
        else
        {
            errno = EINVAL;
            perror("Last argument must be either sys or lib.");
            return 9;
        }
    }

    else
    {
        errno = EINVAL;
        perror("Only generate, sort and copy functions are supported. Usage: ./program function_name other_arguments.");
        return 10;
    }

    times(time_after);
    printf("User time: %Lf s, system time: %Lf s\n", calculate_time_secs(time_before->tms_utime, time_after->tms_utime),
           calculate_time_secs(time_before->tms_stime, time_after->tms_stime));
}
long double calculate_time_secs(clock_t start, clock_t end)
{
    return (long double)(end - start) / (long double)sysconf(_SC_CLK_TCK);
}

int read_starting_from(int fd, char *buffer, unsigned long buffer_size, long start)
{
    int seeked = lseek(fd, start, SEEK_SET);
    if (seeked < 0)
    {
        perror("Couldn't change position in the file");
        return -1;
    }
    int data_read = read(fd, buffer, buffer_size);
    if (data_read < 0)
    {
        perror("Couldn't read the file");
        return -1;
    }
    return 0;
}

int write_starting_from(int fd, char *buffer, unsigned long buffer_size, long start)
{
    int seeked = lseek(fd, start, SEEK_SET);
    if (seeked < 0)
    {
        perror("Couldn't change position in the file");
        return -1;
    }
    int data_written = write(fd, buffer, buffer_size);
    if (data_written < 0)
    {
        perror("Couldn't write to the file");
        return -1;
    }
    return 0;
}

int fread_starting_from(FILE *file, char *buffer, unsigned long buffer_size, long start)
{
    int seeked = fseek(file, start, SEEK_SET);
    if (seeked < 0)
    {
        perror("Couldn't change position in the file");
        return -1;
    }
    unsigned long data_read = fread(buffer, 1, buffer_size, file);
    if (data_read < buffer_size)
    {
        errno = EIO;
        perror("Couldn't read the file");
        return -1;
    }
    return 0;
}

int fwrite_starting_from(FILE *file, char *buffer, unsigned long buffer_size, long start)
{
    int seeked = fseek(file, start, SEEK_SET);
    if (seeked < 0)
    {
        perror("Couldn't change position in the file");
        return -1;
    }
    unsigned long data_written = fwrite(buffer, 1, buffer_size, file);
    if (data_written < buffer_size)
    {
        errno = EIO;
        perror("Couldn't write to the file");
        return -1;
    }
    return 0;
}
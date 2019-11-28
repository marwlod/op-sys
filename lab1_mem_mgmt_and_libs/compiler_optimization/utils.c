static const int M = 1000*1000;
static const int G = 1000*1000*1000;

struct timespec calc_duration(struct timespec start, struct timespec end) {
    long start_nanos = start.tv_sec*G + start.tv_nsec;
    long end_nanos = end.tv_sec*G + end.tv_nsec;
    long duration_secs = (end_nanos - start_nanos) / G;
    long duration_nanos = (end_nanos - start_nanos) % G;
    struct timespec duration = {duration_secs, duration_nanos};
    return duration;
}

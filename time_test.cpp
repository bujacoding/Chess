#include <time.h>
#include <stdio.h>
#include <sys/time.h>

int main() {



    struct timeval t;
    gettimeofday(&t, NULL);

    printf("time_t %d, %ld", t.tv_usec, t.tv_sec);
    return 0;
}
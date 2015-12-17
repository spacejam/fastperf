/*  Protoss test - uses precomputed protobuf messagess
 *  to naively determine req/s
 *
 *  gcc -o bench bench.c -pthread && ./bench
 */

#include <stdlib.h>
#include <pthread.h>
#include <strings.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <time.h>

#define NOPS      1E4
#define NTHREADS  50
#define ADDLEN    1
#define ACKLEN    2

static unsigned long long z = 0;
char add[] = "\xcc";
int mode = 0;

void *work()
{
    int sockfd;
    struct sockaddr_in dest;
    char buffer[64];

    bzero(&dest, sizeof(dest));
    dest.sin_family = AF_INET;
    dest.sin_port = htons(3030);
    if ( inet_aton("127.0.0.1", &dest.sin_addr.s_addr) == 0 ) {
        perror("1.2.3.4");
        exit(errno);
    }
    
    int was;
    while(z < NOPS){
       if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
            perror("Socket");
            exit(errno);
        }

        if ( connect(sockfd, (struct sockaddr*)&dest, sizeof(dest)) != 0 ) {
            printf("failed after %d successes.\n", z);
            perror("Connect ");
            return 1;
        }

        write(sockfd, add, ADDLEN);
        bzero(buffer, 64);
        recv(sockfd, buffer, sizeof(buffer), 0);
        close(sockfd);

        __sync_add_and_fetch(&z, 1);
    }
    return 0;
}

int main(int argc, char **argv)
{
    pthread_t threads[NTHREADS];
    int i;
    FILE *addf, *getf;

    clock_t timer = clock();
    for (i=0; i<NTHREADS; ++i)
        pthread_create(&threads[i], NULL, work, NULL);
    for (i=0; i<NTHREADS; ++i)
        pthread_join(threads[i], NULL);
    timer = clock() - timer;

    float duration = ((float)timer)/CLOCKS_PER_SEC;
    float rate = z / duration;

    printf("%llu in %f seconds (%f/second)\n", z, duration, rate);

    return 0;
}

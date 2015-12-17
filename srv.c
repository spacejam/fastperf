/*
 * srv
 *  Tyler Neely <t@jujit.su>
 * compile:
 *  gcc -o srv srv.c
 * run:
 *  PORT0=8080 ./srv
 */

#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define BACKLOG 1000

int main(void)
{
    struct sockaddr_storage their_addr;
    socklen_t addr_size;
    struct addrinfo hints, *res;
    int sockfd, cli_sk;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
		
    getaddrinfo(NULL, getenv("PORT0"), &hints, &res);

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    bind(sockfd, res->ai_addr, res->ai_addrlen);
    listen(sockfd, BACKLOG);
    printf("listening on %d\n", atoi(getenv("PORT0")));

    addr_size = sizeof their_addr;
		for (;;) {
				cli_sk = accept(sockfd, (struct sockaddr *)&their_addr, &addr_size);
				close(cli_sk);
		}
}

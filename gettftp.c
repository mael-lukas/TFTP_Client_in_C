#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>

#define MAXSIZE 256

int main(int argc, char** argv) {
    // check number of arguments
    if (argc != 3) {
        printf("Invalid number of arguments: expected 3, received %d\r\n",argc);
        exit(EXIT_FAILURE);
    }

    char* host = argv[1];
    char* file = argv[2];

    struct addrinfo *res;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_protocol = IPPROTO_UDP;

    int status = getaddrinfo(host, NULL, &hints, &res);
    if (status == -1) {
        printf("Unable to reach host: %s\r\n",host);
        exit(EXIT_FAILURE);
    }
    int sfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
    if (sfd == -1) {
        perror("socket error");
        exit(EXIT_FAILURE);
    }
    printf("created socket\r\n");

    char sendBuffer[MAXSIZE] = {0};
    char receiveBuffer[MAXSIZE] = {0};

    struct sockaddr_in sockadd;
    sockadd.sin_family = res->ai_family;
    sockadd.sin_port = htons(1069);



    return 0;
}
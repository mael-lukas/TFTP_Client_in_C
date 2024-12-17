#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAXSIZE 256
#define RRQ 1
#define WRQ 2
#define SERVER_PORT 1069

int main(int argc, char** argv) {
    // check number of arguments
    if (argc != 3) {
        printf("Invalid number of arguments: expected 3, received %d\r\n",argc);
        exit(EXIT_FAILURE);
    }

    char* localhost = "127.0.0.1";
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
    // filling the send buffer
    // first two bytes used for operation code, here read request
    sendBuffer[0] = 0;
    sendBuffer[1] = RRQ;
    // next bytes are filled with the file name we want to read
    sprintf(sendBuffer + 2,"%s",file);
    // reserved byte
    sendBuffer[2+strlen(file)] = 0;
    // ascii mode
    sprintf(sendBuffer + 3 + strlen(file), "NETASCII");
    // reserved byte
    sendBuffer[11 + strlen(file)] = 0;

    int sendBufferSize = 12 + strlen(file);

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = res->ai_family;
    serverAddr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET,localhost,&serverAddr.sin_addr.s_addr);

    ssize_t sentBytes = sendto(sfd,sendBuffer,sendBufferSize,0,&serverAddr,sizeof(serverAddr));
    if (sentBytes == -1 ) {
        printf("Error while sending request\r\n");
        exit(EXIT_FAILURE);
    }


    return 0;
}
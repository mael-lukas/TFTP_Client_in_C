#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAXSIZE 512
#define RRQ 1
#define WRQ 2
#define DAT 3
#define ACK 4
#define ERR 5
#define SERVER_PORT 1069

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
    inet_pton(AF_INET,host,&serverAddr.sin_addr.s_addr);
    socklen_t serverAddrSize = sizeof(serverAddr);

    ssize_t sentBytes = sendto(sfd,sendBuffer,sendBufferSize,0,&serverAddr,serverAddrSize);
    if (sentBytes == -1) {
        printf("Error while sending request\r\n");
        exit(EXIT_FAILURE);
    }
    printf("Read request for %s sent\r\n",file);


    int nbSplits;
    char ackBuffer[MAXSIZE] = {0};
    int ackBuffSize = 4;
    ssize_t recBytes;

    do {
        recBytes = recvfrom(sfd,receiveBuffer,MAXSIZE,0,&serverAddr,&serverAddrSize);
        if (recBytes == -1) {
            printf("Error during reception\r\n");
            exit(EXIT_FAILURE);
        }
        printf("Just received %d bytes\r\n",recBytes);

        if (receiveBuffer[0] == 0 && receiveBuffer[1] == ERR) {
            printf("Error in received packet, in split %d \t code %d%d:\n%s\n",nbSplits,receiveBuffer[2],receiveBuffer[3],receiveBuffer + 4);
            exit(EXIT_FAILURE);
        }

        if (receiveBuffer[0] == 0 && receiveBuffer[1] == DAT) {
            nbOfSplits = (receiveBuffer[2] << 8) | receiveBuffer[3];
            ackBuffer[0] = 0;
            ackBuffer[1] = ACK;
            ackBuffer[2] = receiveBuffer[2];
            ackBuffer[3] = receiveBuffer[3];
            ssize_t sentBytes2 = sendto(sfd,ackBuffer,ackBuffSize,0,&serverAddr,serverAddrSize);
            if (sentBytes2 == -1) {
                printf("Error while sending acknowledgement\r\n");
                exit(EXIT_FAILURE);
            }
            printf("Acknowledgement for split %d sent\r\n",nbOfSplits);
        }
    } while (recBytes == MAXSIZE);
    return 0;
}
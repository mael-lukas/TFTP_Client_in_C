#include "tftpLabUtils.h"

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

    int status = getaddrinfo(host, SERVER_PORT, &hints, &res);
    if (status == -1) {
        printf("Unable to reach host: %s\r\n",host);
        exit(EXIT_FAILURE);
    }
    // opens socket
    int sfd = socket(res->ai_family,res->ai_socktype,res->ai_protocol);
    if (sfd == -1) {
        perror("socket error");
        exit(EXIT_FAILURE);
    }
    printf("created socket\r\n");

    // filling the send buffer
    char sendBuffer[MAXSIZE] = {0};
    // first two bytes used for operation code, here read request
    sendBuffer[0] = 0;
    sendBuffer[1] = RRQ;
    // next bytes are filled with the file name we want to read
    sprintf(sendBuffer + 2,"%s",file);
    // reserved byte
    sendBuffer[2+strlen(file)] = 0;
    // set mode to octet
    sprintf(sendBuffer + 3 + strlen(file), "octet");
    // reserved byte
    sendBuffer[11 + strlen(file)] = 0;
    int sendBufferSize = 9 + strlen(file);


    ssize_t sentBytes = sendto(sfd,sendBuffer,sendBufferSize,0,res->ai_addr,res->ai_addrlen);
    if (sentBytes == -1) {
        printf("Error while sending request\r\n");
        exit(EXIT_FAILURE);
    }
    printf("Read request for %s sent\r\n",file);


    int nbOfSplits;
    char receiveBuffer[MAXSIZE] = {0};
    char ackBuffer[ACK_BUFF_SIZE] = {0};
    int recBytes;

    do {
        recBytes = recvfrom(sfd,receiveBuffer,MAXSIZE,0,res->ai_addr,&(res->ai_addrlen));
        if (recBytes == -1) {
            printf("Error during reception\r\n");
            exit(EXIT_FAILURE);
        }
        printf("Just received %d bytes\r\n",recBytes);

        if (receiveBuffer[0] == 0 && receiveBuffer[1] == ERR) {
            printf("Error in received packet, in split %d \t code %d%d:\n%s\n",nbOfSplits,receiveBuffer[2],receiveBuffer[3],receiveBuffer + 4);
            exit(EXIT_FAILURE);
        }

        if (receiveBuffer[0] == 0 && receiveBuffer[1] == DAT) {
            nbOfSplits = (receiveBuffer[2] << 8) | receiveBuffer[3];
            ackBuffer[0] = 0;
            ackBuffer[1] = ACK;
            ackBuffer[2] = receiveBuffer[2];
            ackBuffer[3] = receiveBuffer[3];
            ssize_t sentBytes2 = sendto(sfd,ackBuffer,ACK_BUFF_SIZE,0,res->ai_addr,res->ai_addrlen);
            if (sentBytes2 == -1) {
                printf("Error while sending acknowledgement\r\n");
                exit(EXIT_FAILURE);
            }
            printf("Acknowledgement for split %d sent\r\n",nbOfSplits);
        }
    } while (recBytes == MAXSIZE);
    return 0;
}
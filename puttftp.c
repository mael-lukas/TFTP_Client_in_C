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
    sendBuffer[1] = WRQ;
    // next bytes are filled with the file name we want to read
    sprintf(sendBuffer + 2,"%s",file);
    // reserved byte
    sendBuffer[2+strlen(file)] = 0;
    // set mode to octet
    sprintf(sendBuffer + 3 + strlen(file), "octet");
    // reserved byte
    sendBuffer[11 + strlen(file)] = 0;
    int sendBufferSize = 9 + strlen(file);

    // sends write request
    ssize_t sentBytes = sendto(sfd,sendBuffer,sendBufferSize,0,res->ai_addr,res->ai_addrlen);
    if (sentBytes == -1) {
        printf("Error while sending request\r\n");
        exit(EXIT_FAILURE);
    }
    printf("Read request for %s sent\r\n",file);

    // check for server acknowledgement of write request
    char WRQAckBuffer[ACK_BUFF_SIZE] = {0};
    ssize_t ackRecBytesWRQ = recvfrom(sfd,WRQAckBuffer,ACK_BUFF_SIZE,0,res->ai_addr,&(res->ai_addrlen));
    if (ackRecBytesWRQ == -1) {
        printf("Error while receiving acknowledgement of write request\r\n");
        exit(EXIT_FAILURE);
    }

    // open a local file and put it in a char array to send to server
    long fileSize = getFileSize(file);
    char* dataToSend = fileToArray(file,fileSize);

    char ackBuffer[ACK_BUFF_SIZE] = {0};
    int sentDataNb;
    int nbOfSplits = 1;

    do {
        // sends data packet to server
        int sentDataNb = sendto(sfd,dataToSend,(MAXSIZE + 4),0,res->ai_addr,res->ai_addrlen);
        if (sentDataNb == -1) {
            printf("Error while sending data to server\r\n");
            exit(EXIT_FAILURE);
        }
        printf("Block number %d sent\r\n",nbOfSplits);
        // receives server acknowledgement
        int ackRecBytes = recvfrom(sfd,ackBuffer,ACK_BUFF_SIZE,0,res->ai_addr,&(res->ai_addrlen));
        if (ackRecBytes == -1) {
            printf("Error while receiving acknowledgement\r\n");
            exit(EXIT_FAILURE);
        }
        if (ackBuffer[0] == 0 && ackBuffer[1] == ACK) {
              nbOfSplits++;
        }
    } while(sentDataNb == (MAXSIZE + 4));
}
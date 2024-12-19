#ifndef TFTPLABUTILS_H
#define TFTPLABUTILS_H

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
#define SERVER_PORT "1069"
#define ACK_BUFF_SIZE 4

long getFileSize(char* fileName);
char* fileToArray(char* fileName,int fileSize);

#endif //TFTPLABUTILS_H

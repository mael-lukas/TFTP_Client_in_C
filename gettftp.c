#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>

int main(int argc, char** argv) {
    // check number of arguments
    if (argc != 3) {
        printf("Invalid number of arguments: expected 2, received %d\r\n",argc);
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
        printf("Unable to reach host %s\r\n",host);
    }


    return 0;
}
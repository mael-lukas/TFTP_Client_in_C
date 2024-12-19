#include "tftpLabUtils.h"

long getFileSize(char* fileName) {
    FILE *file = fopen(fileName, "rb"); // open the file in binary mode
    if (file == NULL) {
        perror("Error opening file");
        return -1;
    }

    fseek(file, 0, SEEK_END); // move file pointer to the end of the file
    long size = ftell(file); // get the current position of the file pointer
    fclose(file);

    return size;
}

char* fileToArray(char* fileName,int fileSize){
    char* content = malloc(fileSize);
    FILE *file = fopen(fileName, "rb"); // open the file in binary mode
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    int i = 0, c;
    while((c = fgetc(file)) != EOF) {
        content[i] = c;
        i++;
    }
    fclose(file);
    return content;
}
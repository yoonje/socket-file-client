#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "transfer.h"

int main(int argc, char *argv[]) {
    struct sockaddr_in serveraddr;

    char *filename = basename(argv[1]);

    char buff[BUFFSIZE] = {0};
    strncpy(buff, filename, strlen(filename));
    FILE *fp = fopen(argv[1], "rb");

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Can't allocate sockfd");
        exit(1);
    }

    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(SERVERPORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serveraddr.sin_addr) < 0) {
        perror("IPaddress Convert Error");
        exit(1);
    }

    if (connect(sockfd, (const struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) {
        perror("Connect Error");
        exit(1);
    }

    if (filename == NULL) {
        perror("Can't get filename");
        exit(1);
    }

    if (send(sockfd, buff, BUFFSIZE, 0) == -1) {
        perror("Can't send filename");
        exit(1);
    }

    if (fp == NULL) {
        perror("Can't open file");
        exit(1);
    }

    sendfile(fp, sockfd);
    puts("Send Success");

    fclose(fp);
    close(sockfd);
    return 0;
}

void sendfile(FILE *fp, int sockfd) {
    int n;
    char sendline[MAX_LINE] = {0};
    while ((n = fread(sendline, sizeof(char), MAX_LINE, fp)) > 0) {
        if (n != MAX_LINE && ferror(fp)) {
            perror("Read File Error");
            exit(1);
        }
        if (send(sockfd, sendline, n, 0) == -1) {
            perror("Can't send file");
            exit(1);
        }
        memset(sendline, 0, MAX_LINE);
    }
}
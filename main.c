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
    char *filename = basename(argv[1]); // filename만을 긁어 오는 함수
    char buff[BUFFSIZE] = {0};
    strncpy(buff, filename, strlen(filename));
    FILE *fp = fopen(argv[1], "rb"); // argv로 들어온 파일 path에 대하여 open

    int client_socket;

    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(SERVERPORT);

    client_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (client_socket < 0) {
        printf("Can't allocate client_socket");
        exit(1);
    }

    if (inet_pton(AF_INET, "127.0.0.1", &serveraddr.sin_addr) < 0) { // 주소 정보 할당
        printf("IPaddress Convert Error");
        exit(1);
    }

    if (connect(client_socket, (const struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) {
        printf("Connect Error");
        exit(1);
    }

    if (filename == NULL) {
        printf("Can't get filename");
        exit(1);
    }

    if (send(client_socket, buff, BUFFSIZE, 0) == -1) {
        printf("Can't send filename");
        exit(1);
    }

    if (fp == NULL) {
        printf("Can't open file");
        exit(1);
    }

    send_file(fp, client_socket);

    fclose(fp);
    close(client_socket);
    return 0;
}

void send_file(FILE *fp, int sockfd) {
    int n;
    char line_buf[BUFFSIZE] = {0};
    while ((n = fread(line_buf, sizeof(char), BUFFSIZE, fp)) > 0) {
        if (n != BUFFSIZE && ferror(fp)) {
            printf("Read File Error");
            exit(1);
        }
        if (send(sockfd, line_buf, n, 0) == -1) {
            printf("Can't send file");
            exit(1);
        }
        memset(line_buf, 0, BUFFSIZE);
    }
}
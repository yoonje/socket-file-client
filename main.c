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
    char *filename = basename(argv[1]); // get filename
    char buff[BUFFSIZE] = {0};
    strncpy(buff, filename, strlen(filename));
    FILE *fp;

    int client_socket;

    // initialize server info
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(SERVERPORT);

    // create client client_socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (client_socket < 0) {
        printf("Can't allocate client_socket\n");
        exit(1);
    }

    // initialize server info
    if (inet_pton(AF_INET, "127.0.0.1", &serveraddr.sin_addr) < 0) {
        printf("IP address Convert Error\n");
        exit(1);
    }

    // connect server
    if (connect(client_socket, (const struct sockaddr *) &serveraddr, sizeof(serveraddr)) < 0) {
        printf("Connect Error\n");
        exit(1);
    }

    if (filename == NULL) {
        printf("Can't get filename\n");
        exit(1);
    }

    // file open
    if ((fp = fopen(argv[1], "rb")) == NULL) {
        printf("Can't open file\n");
        exit(1);
    }

    // send filename
    if (send(client_socket, buff, BUFFSIZE, 0) == -1) {
        printf("Can't send filename\n");
        exit(1);
    }

    // send file
    send_file(fp, client_socket);

    fclose(fp);
    close(client_socket);
    return 0;
}

void send_file(FILE *fp, int sockfd) {
    int n;
    char buff[BUFFSIZE] = {0};
    while ((n = fread(buff, sizeof(char), BUFFSIZE, fp)) > 0) {
        if (n != BUFFSIZE && ferror(fp)) {
            printf("Read File Error");
            exit(1);
        }
        if (send(sockfd, buff, n, 0) == -1) {
            printf("Can't send file");
            exit(1);
        }
    }
}
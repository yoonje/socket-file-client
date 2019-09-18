#ifndef TRANSFER_FILE_TRANSFER_H
#define TRANSFER_FILE_TRANSFER_H

#define SERVERPORT 8877
#define BUFFSIZE 4096

void send_file(FILE *fp, int sockfd);

#endif
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Port number not provided. Program terminated.\n");
        exit(1);
    }

    int sockfd, newsockfd, portno, n;
    char buffer[255];

    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;

    // SOCKET

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        error("Error opening socket.");
    }

    bzero((char*) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]); // gets port number from argv[1]

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    // BIND

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("Binding failed.");
    }

    // LISTEN

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    // ACCEPT

    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

    if (newsockfd < 0) {
        error("Error on accept");
    }

    while(1) { // loops until game ends

        bzero(buffer, 255); // clears the buffer

        n = read(newsockfd, buffer, 255);
        if (n < 0) {
            error("Error on read");
        }
        printf("Client : %s\n", buffer);

        bzero(buffer, 255); // clears the buffer
        fgets(buffer, 255, stdin);

        n = write(newsockfd, buffer, strlen(buffer));
        if (n < 0) {
            error("Error on write");
        }
        int i = strncmp("Bye", buffer, 3);
        if(i == 0) {
            break;
        }
    }

    close(newsockfd);
    close(sockfd);
    return 0;
}
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "string_utils.h"

// Global Variables

char board[3][3];
const char PLAYER1 = 'X';
const char PLAYER2 = 'O';
char player; // server will assign this client X or O

// Prototypes

void resetBoard();
void printBoard();
void printResult(char);

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char** argv) {

    if (argc < 3) {
        error("Error: not enough argument for ttt.c");
    }

    char buffer[255];
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char* domainName = argv[1];
    portno = atoi(argv[2]);

    // SOCKET

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        error("Error opening socket");
    }

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        error("Error, no such host");
    }

    bzero((char*) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char*) server->h_addr, (char*) &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    // CONNECT

    if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
        error("Connection failure");
    }

    // SENDS PLAY TO SERVER and RECEIVES WAIT

    bzero(buffer, 255);
    char line[50];
    int len;

    printf("Enter player name: ");
    fgets(line, 50, stdin);
    line[strcspn(line, "\n")] = 0;
    len = strlen(line) + 1;

    char temp;
    sprintf(buffer, "PLAY|%d|%s|", len, line);

    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0) {
        error("Error on writing");
    }
    printf("%s\n", buffer);

    bzero(buffer, 255);
    n = read(sockfd, buffer, 255);
    if (n < 0) {
        error("Error on reading");
    }
    printf("%s\n", buffer);
    return 0;

    // MESSAGE LOOP

    char winner = ' '; 
    resetBoard();

    while(1) {
        bzero(buffer, 255);
        
    }
    printBoard();
    printResult(winner);
    return 0;
}

void resetBoard() {

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            board[i][j] = ' ';
        }
    }
    return;
}

void printBoard() {

    printf(" %c | %c | %c ", board[0][0], board[0][1], board[0][2]);
    printf("\n---|---|---\n");
    printf(" %c | %c | %c ", board[1][0], board[1][1], board[1][2]);
    printf("\n---|---|---\n");
    printf(" %c | %c | %c ", board[2][0], board[2][1], board[2][2]);
    printf("\n");
    return;
}

void printResult(char winner) {
    if (winner == 'X') {
        printf("PLAYER 1 WINS!\n");
    } else if (winner == 'O') {
        printf("PLAYER 2 WINS\n");
    } else {
        printf("TIE GAME!\n");
    }
    return;
}
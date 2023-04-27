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

// GLOBAL VARIABLES

char board[3][3];
const char PLAYER1 = 'X';
const char PLAYER2 = 'O';

// PROTOTYPES

void resetBoard();
void printBoard();
int checkFreeSpaces();
void playerXMove(int, int);
void playerOMove(int, int);
char checkWinner();
void printResult(char);

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

    // SENDS WAIT TO CLIENT

    bzero(buffer, 255); // clears the buffer

    // READ

    n = read(newsockfd, buffer, 255);
    if (n < 0) {
        error("Error on read");
    }
    printf("%s\n", buffer);

    bzero(buffer, 255); // clears the buffer

    // WRITE
    strcpy(buffer, "WAIT|0|");
    n = write(newsockfd, buffer, strlen(buffer));
    if (n < 0) {
        error("Error on write");
    }
    printf("%s\n", buffer);
    return 0; 

    int row, col;
    char winner = ' '; 
    resetBoard();

    // GAME LOOP

    while(winner == ' ' && checkFreeSpaces() != 0) { // while there is no winner and there are free spaces...

        printBoard();

        // RECEIVES INFO FROM PLAYER 1

        // ---------------------------

        playerXMove(row, col);
        winner = checkWinner();
        if (winner != '.' || checkFreeSpaces() == 0) {
            break;
        }

        // RECEIVES INFO FROM PLAYER 2

        // ---------------------------

        playerOMove(row, col);
        winner = checkWinner();
        if (winner != '.' || checkFreeSpaces() == 0) {
            break;
        }
    }
    printBoard();
    printResult(winner);

    // EXAMPLE LOOP

    while(1) {

        bzero(buffer, 255); // clears the buffer

        // READ

        n = read(newsockfd, buffer, 255);
        if (n < 0) {
            error("Error on read");
        }
        printf("%s\n", buffer);

        bzero(buffer, 255); // clears the buffer

        // WRITE

        fgets(buffer, 255, stdin); // reads bytes from input stream
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

void resetBoard() {

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            board[i][j] = '.';
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

int checkFreeSpaces() {
    int freeSpaces = 9;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] != ' ') {
                freeSpaces--;
            }
        }
    }
    return freeSpaces;
}

void playerXMove(int row, int col) {
    do {

        row--;
        col--;

        if (board[row][col] != '.') {
            printf("Invalid move: position is already taken!\n");
        } else {
            board[row][col] = PLAYER1;
            break;
        }

    } while (board[row][col] != '.');

}

void playerOMove(int row, int col) {
    do {

        row--;
        col--;

        if (board[row][col] != '.') {
            printf("Invalid move: position is already taken!\n");
        } else {
            board[row][col] = PLAYER2;
            break;
        }

    } while (board[row][col] != '.');

}

char checkWinner() { // returns the mark of the winner (otherwise returns nothing)
    // Check for row win
    for (int i = 0; i < 3; i++) {
        if (board[i][0] == board[i][1] && board[i][0] == board[i][2]) {
            return board[i][0];
        }
    }
    // Check for col win
    for (int i = 0; i < 3; i++) {
        if (board[0][i] == board[1][i] && board[0][i] == board[2][i]) {
            return board[0][i];
        }
    }
    // Check for diag win
    if (board[0][0] == board[1][1] && board[0][0] == board[2][2]) {
        return board[0][0];
    }
    if (board[0][2] == board[1][1] && board[0][2] == board[2][0]) {
        return board[0][2];
    }
    return '.';
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
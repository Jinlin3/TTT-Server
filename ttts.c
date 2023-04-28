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
char buffer[255];

// PROTOTYPES

void resetBoard();
void printBoard();
char* boardString();
int checkFreeSpaces();
int playerMove(int, int, char);
char checkWinner();
void printResult(char, char*, char*);
char** split(char*, char*);
int interpret(char);

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char** argv) {

    char** array1; // stores messages sent by client 1
    char** array2; // stores messages sent by client 2

    if (argc < 2) {
        fprintf(stderr, "Port number not provided. Program terminated.\n");
        exit(1);
    }

    int sockfd, newsockfd1, newsockfd2, portno, n;

    struct sockaddr_in serv_addr, cli_addr1, cli_addr2;
    socklen_t clilen1, clilen2;

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

    clilen1 = sizeof(cli_addr1);
    clilen2 = sizeof(cli_addr2);

    // ACCEPT

    newsockfd1 = accept(sockfd, (struct sockaddr *) &cli_addr1, &clilen1);
    printf("Player 1 found!\n");
    newsockfd2 = accept(sockfd, (struct sockaddr *) &cli_addr2, &clilen2);
    printf("Player 2 found!\n");

    if (newsockfd1 < 0 || newsockfd2 < 0) {
        error("Error on accept");
    }

    /*
        THIS SECTION WILL CONNECT WITH THE CLIENTS AND SEND WAIT
    */

    char player1[64];
    char player2[64];

    // PLAYER 1 CONNECTION

    bzero(buffer, 255); // clears the buffer

    n = read(newsockfd1, buffer, 255); // reads from client
    if (n < 0) {
        error("Error on read");
    }
    printf("%s\n", buffer);
    array1 = split(buffer, "|");
    strcpy(player1, array1[2]);

    bzero(buffer, 255); // clears the buffer

    strcpy(buffer, "WAIT|0|");
    n = write(newsockfd1, buffer, 255); // writes to client
    if (n < 0) {
        error("Error on write");
    }
    printf("%s\n", buffer);

    // PLAYER 2 CONNECTION

    bzero(buffer, 255); // clears the buffer

    n = read(newsockfd2, buffer, 255); // reads from client
    if (n < 0) {
        error("Error on read");
    }
    printf("%s\n", buffer);
    array2 = split(buffer, "|");
    strcpy(player2, array2[2]);

    bzero(buffer, 255); // clears the buffer

    strcpy(buffer, "WAIT|0|");
    n = write(newsockfd2, buffer, 255); // writes to client
    if (n < 0) {
        error("Error on write");
    }
    printf("%s\n", buffer);
    printf("Both players found!\n");

    /*
        END OF SECTION
    */

    /*
        GAME BEGINS BELOW
        newsockfd1 = X
        newsockfd2 = O
    */

    // BEGN PLAYER2

    bzero(buffer, 255); // clears the buffer
    sprintf(buffer, "BEGN|%lu|O|%s|", strlen(player1) + 1, player1);
    n = write(newsockfd2, buffer, 255);
    if (n < 0) {
        error("Error on write");
    }
    printf("%s\n", buffer);

    // BEGN PLAYER1

    bzero(buffer, 255); // clears the buffer
    sprintf(buffer, "BEGN|%lu|X|%s|", strlen(player2) + 1, player2);
    n = write(newsockfd1, buffer, 255);
    if (n < 0) {
        error("Error on write");
    }
    printf("%s\n", buffer);

    /*
        GAME COMMENCES
    */

    int success; // 0 - good; 1 - bad | used for controlling the do-while loop
    char winner = ' '; 
    resetBoard();

    while(winner == ' ' && checkFreeSpaces() != 0) { // while there is no winner and there are free spaces...

        printBoard();
        // PLAYER 1's MOVE
        do {
            bzero(buffer, 255);
            n = read(newsockfd1, buffer, 255);
            if (n < 0) {
                error("Error on read");
            }
            printf("%s\n", buffer);
            success = interpret('X');

            // CHECK FOR WINNER
            printf("Checking for winner\n");
            winner = checkWinner();
            printf("winnerMark: %c\n", winner);
            if (winner != ' ' || checkFreeSpaces() == 0) {
                break;
            }
            printBoard();
            
            n = write(newsockfd1, buffer, 255);
            if (n < 0) {
                error("Error on write");
            }
            n = write(newsockfd2, buffer, 255);
            if (n < 0) {
                error("Error on write");
            }
            printf("%s\n", buffer);
        } while (success != 0);
        if (winner != ' ' || checkFreeSpaces() == 0) {
            break;
        }
        // PLAYER 2's MOVE
        do {
            bzero(buffer, 255);
            n = read(newsockfd2, buffer, 255);
            if (n < 0) {
                error("Error on read");
            }
            printf("%s\n", buffer);

            success = interpret('O');

            // CHECK FOR WINNER
            printf("Checking for winner\n");
            winner = checkWinner();
            printf("winnerMark: %c\n", winner);
            if (winner != ' ' || checkFreeSpaces() == 0) {
                break;
            }

            n = write(newsockfd2, buffer, 255);
            if (n < 0) {
                error("Error on write");
            }
            n = write(newsockfd1, buffer, 255);
            if (n < 0) {
                error("Error on write");
            }

            printf("%s\n", buffer);
        } while (success != 0);
        if (winner != ' ' || checkFreeSpaces() == 0) {
            break;
        }
    }


    printBoard();
    printResult(winner, player1, player2);
    n = write(newsockfd1, buffer, 255);
    n = write(newsockfd2, buffer, 255);

    close(newsockfd1);
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

char* boardString() {

    char string[100];
    sprintf(string, "%c%c%c%c%c%c%c%c%c", board[0][0], board[0][1], board[0][2], board[1][0], board[1][1], board[1][2], board[2][0], board[2][1], board[2][2]);
    return string;

}

int checkFreeSpaces() {
    int freeSpaces = 9;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] != '.') {
                freeSpaces--;
            }
        }
    }
    printf("freespaces: %d\n", freeSpaces);
    return freeSpaces;
}

/*
    This function is used to receive the coordinates of a desired move and then determine whether it's
    legal. It will then fill the buffer with an appropriate message to send back to the client.
*/

int playerMove(int row, int col, char mark) {

    int success = 0;
    row--;
    col--;
    printf("row: %d, col: %d\n", row, col);

    bzero(buffer, 255);

    if (board[row][col] != '.') { // INVALID MOVE
        success = 1;
        char* errorString = "Invalid move: position is already taken!";
        sprintf(buffer, "INVL|%lu|%s|", strlen(errorString) + 1, errorString);
    } else { // VALID MOVE
        board[row][col] = mark;
        char* board = boardString();
        sprintf(buffer, "MOVD|%lu|%c|%d,%d|%s|", strlen(board) + 7, mark, row, col, board);
    }

    return success;

}

char checkWinner() { // returns the mark of the winner (otherwise returns nothing)
    // Check for row win
    for (int i = 0; i < 3; i++) {
        if (board[i][0] != '.' && (board[i][0] == board[i][1] && board[i][0] == board[i][2])) {
            return board[i][0];
        }
    }
    // Check for col win
    for (int i = 0; i < 3; i++) {
        if (board[0][i] != '.' && (board[0][i] == board[1][i] && board[0][i] == board[2][i])) {
            return board[0][i];
        }
    }
    // Check for diag win
    if (board[0][0] != '.' && (board[0][0] == board[1][1] && board[0][0] == board[2][2])) {
        return board[0][0];
    }
    if (board[0][2] != '.' && (board[0][2] == board[1][1] && board[0][2] == board[2][0])) {
        return board[0][2];
    }
    return ' ';
}

void printResult(char winner, char* player1, char* player2) {
    bzero(buffer, 255);
    if (winner == 'X') {
        printf("PLAYER 1 WINS!\n");
        sprintf(buffer, "OVER|%lu|W|%s wins.", strlen(player1) + 8, player1);
    } else if (winner == 'O') {
        printf("PLAYER 2 WINS\n");
        sprintf(buffer, "OVER|%lu|W|%s wins.", strlen(player2) + 8, player2);
    } else {
        printf("TIE GAME!\n");
        sprintf(buffer, "OVER|11|D|Tie game.");
    }
    return;
}

char** split(char* string, char* delim) {
    
    char** tokens = malloc(sizeof(char*) * 64);
    char* token;
    int pos = 0;

    if (!tokens) {
        printf("split function has failed");
        exit(1);
    }

    token = strtok(string, delim);
    while (token != NULL) {
        tokens[pos] = token;
        pos++;
        token = strtok(NULL, delim);
    }
    tokens[pos] = NULL;
    return tokens;

}

/*
    This function is used to do several things:
    1. To read the message from the client and then react accordingly
    2. To fill the buffer with an appropriate message to send back to the client

    Note: everything to do with sockets is done in the main function, therefore the only thing we can do is
    change the contents of the buffer.
*/

int interpret(char playerMark) {

    int success = 0;
    char** tokens = split(buffer, "|");

    if (strcmp(tokens[0], "MOVE") == 0) {
        
        char** pos = split(tokens[3], ",");
        int row = atoi(pos[0]);
        int col = atoi(pos[1]);
        success = playerMove(row, col, playerMark);

    } else if (strcmp(tokens[0], "RSGN") == 0) {
        
    } else { // Last case: DRAW
        
    }

    return success;

}
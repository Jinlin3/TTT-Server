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

// Global Variables

char board[3][3];
char name[50];
char playerMark; // server will assign this client X or O
char buffer[255];

// Prototypes

void resetBoard();
void printBoard();
void updateBoard(char*);
void printResult(char);
char** split(char*, char*);
void action();
int reaction();

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char** argv) {

    char** array;

    if (argc < 3) {
        error("Error: not enough argument for ttt.c");
    }

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

    server = gethostbyname(domainName);
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

    /*
        THIS SECTION WILL ESTABLISH CONNECTION WITH THE SERVER
    */

    bzero(buffer, 255);
    int len;

    printf("Enter player name: ");
    fgets(name, 50, stdin);
    name[strcspn(name, "\n")] = 0;
    len = strlen(name) + 1;

    sprintf(buffer, "PLAY|%d|%s|", len, name);

    n = write(sockfd, buffer, 255);
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

    /*
        END OF SECTION
    */

    // BEGN

    bzero(buffer, 255);
    n = read(sockfd, buffer, 255);
    if (n < 0) {
        error("Error on reading");
    }
    printf("%s\n", buffer);

    array = split(buffer, "|");
    playerMark = *array[2];

    /*
        GAME STARTS BELOW
    */

    resetBoard();
    int success = 0;
    int counter = 0;

    while(1) {

        if (playerMark == 'X' && counter == 0) { // print board only if player 1
            printBoard();
            counter++;
        }

        if (playerMark == 'O') { // only occurs if this is player 2
            do {
                bzero(buffer, 255);
                n = read(sockfd, buffer, 255);
                if (n < 0) {
                    error("Error reading");
                }
                printf("PLAYER 1 - %s\n", buffer);
                success = reaction();
                printBoard();
            } while (success == 1); // While INVL
        }

        if (success == 3) {
            break;
        }

        do { // For handling situations where INVL is received and player needs to go again
            bzero(buffer, 255);
            action(); // Player makes their move
            n = write(sockfd, buffer, 255); // Sends message to server
            if (n < 0) {
                error("Error writing");
            }
            printf("%s\n", buffer);

            bzero(buffer, 255);
            n = read(sockfd, buffer, 255); // Reads reaction message from server
            if (n < 0) {
                error("Error reading");
            }
            printf("%s\n", buffer);
            success = reaction();
            printBoard();
        } while (success == 1);
        if (success == 3) { // Checks for OVER
            break;
        }
        if (playerMark == 'X') { // only occurs if this is player 1
            do {
                bzero(buffer, 255);
                n = read(sockfd, buffer, 255);
                if (n < 0) {
                    error("Error reading");
                }
                printf("PLAYER 2 - %s\n", buffer);
                success = reaction();
                printBoard();
            } while (success == 1); // While INVL
        }
        if (success == 3) {
            break;
        }

    }

    printf("GAME OVER!\n");

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

void updateBoard(char* boardString) {

    int counter = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            board[i][j] = boardString[counter];
            counter++;
        }
    }
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
    This function will let the user make their move
*/

void action() {

    int choice;
    int success = 0;

    do {
        printf("Select a move: (1-3)\n");
        printf("1) MOVE\n");
        printf("2) RSGN\n");
        printf("3) DRAW\n");

        scanf("%d", &choice);

        if (choice == 1) { // MOVE
            int row;
            int col;

            printf("Enter row #(1-3): ");
            scanf("%d", &row);
            printf("Enter col #(1-3): ");
            scanf("%d", &col);

            bzero(buffer, 255);
            sprintf(buffer, "MOVE|6|%c|%d,%d|", playerMark, row, col);

            success = 0;
        } else if (choice == 2) { // RSGN
            bzero(buffer, 255);
            sprintf(buffer, "RSGN|%lu|%s has resigned.|", strlen(name) + 15, name);
            success = 0;
        } else if (choice == 3) { // DRAW
            success = 0;
        } else { // INVALID INPUT; LOOP AGAIN
            success = 1;
        }
    } while (success != 0);
    
}

/*
    This function will react to server's message
*/

int reaction() {
    char** array = split(buffer, "|");
    if (strcmp(array[0], "MOVD") == 0) {
        updateBoard(array[4]);
        return 0;
    } else if (strcmp(array[0], "INVL") == 0) {
        return 1;
    } else if (strcmp(array[0], "DRAW") == 0) {

    } else { // "OVER"
        return 3;
    }
    return 0;
}
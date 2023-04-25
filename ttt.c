#include <stdio.h>
#include <stdlib.h>

char board[3][3];
const char PLAYER1 = 'X';
const char PLAYER2 = 'O';

void resetBoard();
void printBoard();
int checkFreeSpaces();
void player1Move();
void player2Move();
char checkWinner();
void printWinner(char);

int main(int argc, char** argv) {
/*  if (argc < 3) {
        printf("error: Not enough arguments for ttt.c\n");
    }
    char* domainName = argv[1];
    char* portNumber = argv[2];
*/

    char winner = ' '; 
    resetBoard();



    return 0;
}

void resetBoard();
void printBoard();
int checkFreeSpaces();
void playerMove();
void player2Move();
char checkWinner();
void printWinner(char winner);
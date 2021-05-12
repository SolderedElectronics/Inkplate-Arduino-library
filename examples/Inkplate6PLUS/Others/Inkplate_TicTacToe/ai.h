#ifndef AI
#define AI

#include "Arduino.h"

// Find out more about minimax algorithm here:
// https://github.com/Cledersonbc/tic-tac-toe-minimax

// Returns board state: 0 - still playing, 1 - x won, 2 - o won, 3 tie
uint8_t result(char board[3][3]);

// Struct to make it easier to return best move and it's score
struct best
{
    int move;
    int score;
};

// Main minimax function
struct best minimax(char player, char max, char board[3][3], int depth);

// HELPER FUNCTIONS:
void swap(int *a, int *b);
void fisher_yates_shuffle(int n, int a[]);

#endif
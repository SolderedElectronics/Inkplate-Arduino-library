#include "ai.h"

// Helpers:
void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

void fisher_yates_shuffle(int n, int a[])
{
    for (int i = 0; i < n; i++)
        swap(a + i, a + i + random(n - 1 - i));
}

// Returns supplied board state
uint8_t result(char board[3][3]) // 0 - still playing, 1 - x won, 2 - o won, 3 tie
{
    // Horizontal/diagonal win
    for (int i = 0; i < 3; ++i)
    {
        if (board[i][0] == 'x' && board[i][1] == 'x' && board[i][2] == 'x')
            return 1;
        if (board[i][0] == 'o' && board[i][1] == 'o' && board[i][2] == 'o')
            return 2;
        if (board[0][i] == 'x' && board[1][i] == 'x' && board[2][i] == 'x')
            return 1;
        if (board[0][i] == 'o' && board[1][i] == 'o' && board[2][i] == 'o')
            return 2;
    }
    // Diagonal win
    if (board[0][0] == 'x' && board[1][1] == 'x' && board[2][2] == 'x')
        return 1;
    if (board[0][2] == 'x' && board[1][1] == 'x' && board[2][0] == 'x')
        return 1;
    if (board[0][0] == 'o' && board[1][1] == 'o' && board[2][2] == 'o')
        return 2;
    if (board[0][2] == 'o' && board[1][1] == 'o' && board[2][0] == 'o')
        return 2;
    // Check if there are empty fields
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            if (board[i][j] == '_')
                return 0;
    // If there are non, return a tie
    return 3;
}

// Recursive function, as described here: https://github.com/Cledersonbc/tic-tac-toe-minimax#understanding-the-algorithm
struct best minimax(char player, char max, char board[3][3], int depth)
{
    // Evaluate current state and return it
    if (depth == 0 || result(board) != 0)
    {
        int srX[] = {0, -1, 1, 0};
        int srO[] = {0, 1, -1, 0};
        struct best r = {-1, (max == 'o' ? srX : srO)[result(board)]};
        return r;
    }

    // Try all possible moves, and return min or max, depending on who's move it is
    struct best bs = {-1, player == max ? -1e8 : 1e8};
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
        {
            if (board[i][j] != '_')
                continue;

            board[i][j] = player;
            struct best t = {j + i * 3, minimax(player == 'x' ? 'o' : 'x', max, board, depth - 1).score};
            board[i][j] = '_';

            if (depth == 0)
            {
                Serial.printf("%d %d\n", j + 3 * j, t.score);
            }
            if (player == max)
            {
                if (t.score > bs.score)
                    bs = t;
            }
            else
            {
                if (t.score < bs.score)
                    bs = t;
            }
        }
    return bs;
}

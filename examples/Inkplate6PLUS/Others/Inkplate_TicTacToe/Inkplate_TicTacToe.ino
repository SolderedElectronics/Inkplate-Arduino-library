/*
   Inkplate_TicTacToe sketch for e-radionica.com Inkplate 6PLUS
   Select "Inkplate 6PLUS(ESP32)" from Tools -> Board menu.
   Don't have "Inkplate 6PLUS(ESP32)" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   This example shows you how to use some higher level touchscreen functions to easily create
   interactable touchscreen user interfaces.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   17 February 2021 by e-radionica.com
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE6PLUS
    #error "Wrong board selection for this example, please select Inkplate 6PLUS plus in the boards menu."
#endif

#include "Inkplate.h"
#include "ai.h"

#include "generatedUIMenu.h"

Inkplate display(INKPLATE_1BIT);

// How far to search for best move
int difficultyDepth[] = {6, 7, 10};

// Global game states
int difficulty = -1;
int firstHuman = -1;
int firstXO = -1;

bool menu = 1;
bool game = 0;
int move = 0;

// Global board array
char board[3][3] = {
    {'_', '_', '_'},
    {'_', '_', '_'},
    {'_', '_', '_'},
};

// If your Inkplate doesn't have external (or second) MCP I/O expander, you should uncomment next line,
// otherwise your code could hang out when you send code to your Inkplate.
// You can easily check if your Inkplate has second MCP by turning it over and 
// if there is missing chip near place where "MCP23017-2" is written, but if there is
// chip soldered, you don't have to uncomment line and use external MCP I/O expander

void setup()
{
    Serial.begin(115200);
    display.begin(); // Initialize Inkplate object

    // Initialize touchscreen
    if (!display.tsInit(true))
        Serial.println("Touchscreen init failed!");

    // Call main draw function defined below
    mainDrawMenu();
    display.display();
}

void loop()
{
    // Main game loop
    if (menu)
        menuEvents();
    else if (game)
        gameEvents();
    delay(15);
}

// All touchscreen events happening while in menu
void menuEvents()
{
    if (display.touchInArea(39, 225, 300, 55)) // Easy difficulty
    {
        difficulty = 0;
        display.clearDisplay();
        mainDrawMenu();
        drawChoices();
        display.partialUpdate();
    }

    if (display.touchInArea(39, 320, 300, 55)) // Medium difficulty
    {
        difficulty = 1;
        display.clearDisplay();
        mainDrawMenu();
        drawChoices();
        display.partialUpdate();
    }

    if (display.touchInArea(39, 420, 300, 55)) // Hard difficulty
    {
        difficulty = 2;
        display.clearDisplay();
        mainDrawMenu();
        drawChoices();
        display.partialUpdate();
    }

    if (display.touchInArea(39, 516, 300, 55)) // 2 player game
    {
        difficulty = 3;
        display.clearDisplay();
        mainDrawMenu();
        drawChoices();
        display.partialUpdate();
    }

    if (display.touchInArea(553, 246, 300, 55) && difficulty != 3) // Human plays first
    {
        firstHuman = 0;
        display.clearDisplay();
        mainDrawMenu();
        drawChoices();
        display.partialUpdate();
    }

    if (display.touchInArea(553, 326, 300, 55) && difficulty != 3) // Computer plays first
    {
        firstHuman = 1;
        display.clearDisplay();
        mainDrawMenu();
        drawChoices();
        display.partialUpdate();
    }

    if (display.touchInArea(553, 447, 300, 55)) // First player is X
    {
        firstXO = 0;
        display.clearDisplay();
        mainDrawMenu();
        drawChoices();
        display.partialUpdate();
    }

    if (display.touchInArea(553, 512, 300, 55)) // First player is O
    {
        firstXO = 1;
        display.clearDisplay();
        mainDrawMenu();
        drawChoices();
        display.partialUpdate();
    }

    if (display.touchInArea(585, 627, 301, 79)) // Start gane
    {
        // Check if all settings are legal, and if so, start the game
        if (difficulty == 3 && (firstXO == 0 || firstXO == 1))
        {
            menu = 0;
            game = 1;
            display.clearDisplay();
            mainDrawGame();
            display.display();
        }
        else if (difficulty != -1 && firstXO != -1 && firstHuman != -1)
        {
            menu = 0;
            game = 1;
            display.clearDisplay();
            mainDrawGame();
            display.display();

            // Make first move, if computer is to make a first move
            if (difficulty != 3 && firstHuman == 1)
            {
                struct best bm =
                    minimax((move + firstXO + 1) % 2 ? 'x' : 'o', (move + firstXO + 1) % 2 ? 'x' : 'o', board, 10);
                board[bm.move / 3][bm.move % 3] = (move + firstXO + 1) % 2 ? 'x' : 'o';
                ++move;

                display.clearDisplay();
                mainDrawGame();
                display.partialUpdate();
            }
        }
        else // All settings are not legal
        {
            text13_content = "Please select all options!";
            display.clearDisplay();
            mainDrawMenu();
            drawChoices();
            display.partialUpdate();
            text13_content = "";
        }
    }
}

// Draw choice radio buttons
void drawChoices()
{
    // Switch case to select correct options
    switch (difficulty)
    {
    case 0:
        display.fillCircle(circle2_center_x, circle2_center_y, 7, BLACK);
        break;
    case 1:
        display.fillCircle(circle3_center_x, circle3_center_y, 7, BLACK);
        break;
    case 2:
        display.fillCircle(circle4_center_x, circle4_center_y, 7, BLACK);
        break;
    case 3:
        display.fillCircle(circle5_center_x, circle5_center_y, 7, BLACK);
        break;
    case 4:
        display.fillCircle(circle6_center_x, circle6_center_y, 7, BLACK);
        break;
    }
    // Switch case to select correct options
    switch (firstHuman)
    {
    case 0:
        display.fillCircle(circle7_center_x, circle7_center_y, 7, BLACK);
        break;
    case 1:
        display.fillCircle(circle6_center_x, circle6_center_y, 7, BLACK);
        break;
    }
    switch (firstXO)
    {
    case 0:
        display.fillCircle(circle8_center_x, circle8_center_y, 7, BLACK);
        break;
    case 1:
        display.fillCircle(circle9_center_x, circle9_center_y, 7, BLACK);
        break;
    }
    // If 2 player mode selected, draw computer first options out
    if (difficulty == 3)
        crossOutHumanFirst();
}

// Simulate faded text by drawing white checkerboard pattern over
void crossOutHumanFirst()
{
    for (int i = rect6_a_y; i < rect6_b_y; ++i)
        for (int j = rect6_a_x + (i % 2); j < rect6_b_x; j += 2)
            display.drawPixel(j, i, WHITE);
}

// All touchscreen events during normal game
void gameEvents()
{
    if (display.touchInArea(15, 600, 215, 140)) // Go back
    {
        memset(board, '_', sizeof board);
        menu = 1;
        game = 0;
        move = 0;
        display.clearDisplay();
        mainDrawMenu();
        drawChoices();
        display.display();
    }
    // Check if any board field pressed
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            if (display.touchInArea(268 + 243 * j, 20 + 243 * i, 243, 243))
            {
                // If field already pressed, skip
                if (board[i][j] != '_' || result(board) != 0)
                    continue;
                board[i][j] = (move + firstXO + 1) % 2 ? 'x' : 'o';

                ++move;
                display.clearDisplay();
                mainDrawGame();
                display.partialUpdate();

                // If game is against a compuer, make a move
                if (result(board) == 0 && difficulty != 3)
                {
                    delay(1000);
                    struct best bm = minimax((move + firstXO + 1) % 2 ? 'x' : 'o', (move + firstXO + 1) % 2 ? 'x' : 'o',
                                             board, difficultyDepth[difficulty]);
                    board[bm.move / 3][bm.move % 3] = (move + firstXO + 1) % 2 ? 'x' : 'o';

                    // Draw board again
                    ++move;
                    display.clearDisplay();
                    mainDrawGame();
                    display.partialUpdate();
                }
            }
}

// Draw game elements to screen
void mainDrawGame()
{
    // Draw board lines
    display.drawThickLine(511, 20, 511, 748, BLACK, 10);
    display.drawThickLine(754, 20, 754, 748, BLACK, 10);

    display.drawThickLine(268, 263, 996, 263, BLACK, 10);
    display.drawThickLine(268, 506, 996, 506, BLACK, 10);

    // Draw who's turn it is
    display.setFont(&FreeSerifBold24pt7b);
    display.setCursor(22, 100);
    display.print((move + firstXO + 1) % 2 ? "X's turn:" : "O's turn:");

    // Draw game state
    display.setCursor(22, 392);
    switch (result(board))
    {
    case 0:
        display.print("Game on!");
        break;
    case 1:
        display.print("X won!");
        break;
    case 2:
        display.print("O won!");
        break;
    case 3:
        display.print("Game is a tie");
        break;
    }

    // Draw back button to screen
    display.setCursor(30, 685);
    display.print("Go Back");
    display.drawRoundRect(15, 600, 215, 140, 10, BLACK);

    drawBoard();
}

// Draw X's and O's to screen
void drawBoard()
{
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
        {
            int x = 268 + 243 * j, y = 20 + 243 * i;
            if (board[i][j] == 'x')
            {
                display.drawThickLine(x + 15, y + 15, x + 223, y + 223, BLACK, 7);
                display.drawThickLine(x + 223, y + 15, x + 15, y + 223, BLACK, 7);
            }
            if (board[i][j] == 'o')
            {
                for (int k = 0; k < 7; ++k)
                    display.drawCircle(x + 121, y + 121, 100 - k, BLACK);
            }
        }
}

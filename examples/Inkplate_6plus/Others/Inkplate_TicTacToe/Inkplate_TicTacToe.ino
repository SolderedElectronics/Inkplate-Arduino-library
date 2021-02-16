#include "Inkplate.h"

#include "generatedUIMenu.h"

Inkplate display(INKPLATE_1BIT);

int difficulty = -1;
int firstHuman = -1;
int firstXO = -1;

bool menu = 1;
bool game = 0;

char board[3][4] = {
    "   ",
    "   ",
    "   ",
};

void setup()
{
    Serial.begin(115200);
    display.begin();

    if (!display.tsInit(true))
        Serial.println("Touchscreen init failed!");

    mainDrawMenu();
    display.display();
}

void loop()
{
    if (menu)
        menuEvents();
    else if (game)
        gameEvents();
    delay(15);
}

void menuEvents()
{
    if (display.inRect(39, 225, 300, 55))
    {
        difficulty = 0;
        display.clearDisplay();
        mainDrawMenu();
        drawChoices();
        display.partialUpdate();
    }

    if (display.inRect(39, 320, 300, 55))
    {
        difficulty = 1;
        display.clearDisplay();
        mainDrawMenu();
        drawChoices();
        display.partialUpdate();
    }

    if (display.inRect(39, 420, 300, 55))
    {
        difficulty = 2;
        display.clearDisplay();
        mainDrawMenu();
        drawChoices();
        display.partialUpdate();
    }

    if (display.inRect(39, 516, 300, 55))
    {
        difficulty = 3;
        display.clearDisplay();
        mainDrawMenu();
        drawChoices();
        display.partialUpdate();
    }

    if (display.inRect(553, 246, 300, 55) && difficulty != 3)
    {
        firstHuman = 0;
        display.clearDisplay();
        mainDrawMenu();
        drawChoices();
        display.partialUpdate();
    }

    if (display.inRect(553, 326, 300, 55) && difficulty != 3)
    {
        firstHuman = 1;
        display.clearDisplay();
        mainDrawMenu();
        drawChoices();
        display.partialUpdate();
    }

    if (display.inRect(553, 447, 300, 55))
    {
        firstXO = 0;
        display.clearDisplay();
        mainDrawMenu();
        drawChoices();
        display.partialUpdate();
    }

    if (display.inRect(553, 512, 300, 55))
    {
        firstXO = 1;
        display.clearDisplay();
        mainDrawMenu();
        drawChoices();
        display.partialUpdate();
    }

    if (display.inRect(585, 627, 301, 79))
    {
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
        }
        else
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

void drawChoices()
{
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
    if (difficulty == 3)
        crossOutHumanFirst();
}

void crossOutHumanFirst()
{
    for (int i = rect6_a_y; i < rect6_b_y; ++i)
        for (int j = rect6_a_x + (i % 2); j < rect6_b_x; j += 2)
            display.drawPixel(j, i, WHITE);
}

void gameEvents()
{
}

void mainDrawGame()
{
    display.drawThickLine(492, 20, 492, 748, BLACK, 10);
    display.drawThickLine(772, 20, 772, 748, BLACK, 10);

    display.drawThickLine(268, 252, 996, 252, BLACK, 10);
    display.drawThickLine(268, 516, 996, 516, BLACK, 10);

    display.setFont(&FreeSerifBold24pt7b);
    display.setCursor(15, 100);
    display.print("X's turn:");

    display.setCursor(65, 685);
    display.print("Go Back");

    display.drawRoundRect(50, 600, 215, 140, 10, BLACK);
}
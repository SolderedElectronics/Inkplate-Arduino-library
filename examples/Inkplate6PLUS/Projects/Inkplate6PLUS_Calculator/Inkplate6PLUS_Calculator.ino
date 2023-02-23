/*
   Inkplate6PLUS_Calculator example for Soldered Inkplate 6Plus
   For this example you will need USB cable and Inkplate 6PLUS (and a calculator, if you don't trust Inkplate :) ).
   Select "e-radionica Inkplate 6Plus" or "Soldered Inkplate 6Plus" from Tools -> Board menu.
   Don't have "e-radionica Inkplate 6Plus" or "Soldered Inkplate 6Plus" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example will show you how simple GUI can be created on Inkplate 6PLUS with this simple calculator example.
   You can do simple math calculations on this calculator (like addition, subtraction, multiplication, division).

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   28 July 2020 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_INKPLATE6PLUS) && !defined(ARDUINO_INKPLATE6PLUSV2)
#error                                                                                                                 \
    "Wrong board selection for this example, please select e-radionica Inkplate 6Plus or Soldered Inkplate 6Plus in the boards menu."
#endif

#include "Calculator.h"
#include "Inkplate.h"

Inkplate display(INKPLATE_1BIT);

#define X_REZ_OFFSET 15

double leftNumber = 0;
double rightNumber = 0;
char op = ' ';
double result = 0;
int rightNumPos = 0;
bool decimalPointOnCurrentNumber = false;
int numOfDecimalDigitsOnCurrentNumber = 0;
int numOfDigitsEntered = 0;

void setup()
{
    Serial.begin(115200);
    display.begin(); // Initialize Inkplate object
    display.clearDisplay();
    // Initialize touchscreen
    if (!display.tsInit(true))
    {
        Serial.println("Touchscreen init failed!");
    }
    // Call main draw function defined below
    mainDraw();
    display.display();
}

void loop()
{
    keysEvents();
    delay(20);
}

void keysEvents()
{
    if (display.touchInArea(800, 20, 200, 80)) // Refresh
    {
        display.clearDisplay();
        mainDraw();
        display.display();
    }

    if (display.touchInArea(600, 20, 200, 80)) // Clear
    {
        text18_cursor_x = 800;
        text18_cursor_y = 260;
        text18_content = "";
        op = ' ';
        rightNumPos = 0;
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();
        decimalPointOnCurrentNumber = false;
        numOfDigitsEntered = 0;
        numOfDecimalDigitsOnCurrentNumber = 0;
    }

    if (display.touchInArea(50, 50, 100, 50)) // Clear history
    {
        text19_content = "";
        text19_cursor_x = 50;
        text19_cursor_y = 700;

        display.clearDisplay();
        mainDraw();
        display.partialUpdate();
    }

    if (display.touchInArea(800, 650, 100, 100) && (op != ' ') && atof(&text18_content[rightNumPos+3]) != 0) // Calculate
    {
        result = calculate();

        if (text19_content == "")
        {
            text19_content = text18_content + " = " + result;
        }
        else
        {
            text19_cursor_y -= 55;
            text19_content = text19_content + '\n' + "    " + text18_content + " = " + result;
        }
        text18_content = result;

        display.clearDisplay();
        mainDraw();
        display.partialUpdate();

        text18_content = "";
        text18_cursor_x = 800;
        text18_cursor_y = 260;
        result = 0;
        op = ' ';
        decimalPointOnCurrentNumber = false;
        numOfDigitsEntered = 0;
        numOfDecimalDigitsOnCurrentNumber = 0;
    }

    if (display.touchInArea(900, 650, 100, 100) && (op == ' ') && (rightNumPos > 0))
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + " + ";
        op = '+';
        decimalPointOnCurrentNumber = false;
        numOfDecimalDigitsOnCurrentNumber = 0;
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();
    }

    if (display.touchInArea(700, 650, 100, 100) && numOfDigitsEntered < 6 && numOfDecimalDigitsOnCurrentNumber < 2)
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + "0";
        numOfDigitsEntered++;
        if(decimalPointOnCurrentNumber)
        {
            numOfDecimalDigitsOnCurrentNumber++;
        }
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();

        if (op == ' ')
        {
            ++rightNumPos;
        }
    }

    if (display.touchInArea(600, 650, 100, 100) && !decimalPointOnCurrentNumber && numOfDigitsEntered < 6)
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + ".";
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();

        if (op == ' ')
        {
            ++rightNumPos;
        }
        decimalPointOnCurrentNumber = true;
    }

    if (display.touchInArea(900, 550, 100, 100) && (op == ' ') && (rightNumPos > 0))
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + " - ";
        op = '-';
        decimalPointOnCurrentNumber = false;
        numOfDecimalDigitsOnCurrentNumber = 0;
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();
    }

    if (display.touchInArea(800, 550, 100, 100) && numOfDigitsEntered < 6 && numOfDecimalDigitsOnCurrentNumber < 2)
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + "3";
        numOfDigitsEntered++;
        if(decimalPointOnCurrentNumber)
        {
            numOfDecimalDigitsOnCurrentNumber++;
        }
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();

        if (op == ' ')
        {
            ++rightNumPos;
        }
    }

    if (display.touchInArea(700, 550, 100, 100) && numOfDigitsEntered < 6 && numOfDecimalDigitsOnCurrentNumber < 2)
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + "2";
        numOfDigitsEntered++;
        if(decimalPointOnCurrentNumber)
        {
            numOfDecimalDigitsOnCurrentNumber++;
        }
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();

        if (op == ' ')
        {
            ++rightNumPos;
        }
    }

    if (display.touchInArea(600, 550, 100, 100) && numOfDigitsEntered < 6 && numOfDecimalDigitsOnCurrentNumber < 2)
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + "1";
        numOfDigitsEntered++;
        if(decimalPointOnCurrentNumber)
        {
            numOfDecimalDigitsOnCurrentNumber++;
        }
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();

        if (op == ' ')
        {
            ++rightNumPos;
        }
    }

    if (display.touchInArea(900, 450, 100, 100) && (op == ' ') && (rightNumPos > 0))
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + " x ";
        op = 'x';
        decimalPointOnCurrentNumber = false;
        numOfDecimalDigitsOnCurrentNumber = 0;
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();
    }

    if (display.touchInArea(800, 450, 100, 100) && numOfDigitsEntered < 6 && numOfDecimalDigitsOnCurrentNumber < 2)
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + "6";
        numOfDigitsEntered++;
        if(decimalPointOnCurrentNumber)
        {
            numOfDecimalDigitsOnCurrentNumber++;
        }
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();

        if (op == ' ')
        {
            ++rightNumPos;
        }
    }

    if (display.touchInArea(700, 450, 100, 100) && numOfDigitsEntered < 6 && numOfDecimalDigitsOnCurrentNumber < 2)
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + "5";
        numOfDigitsEntered++;
        if(decimalPointOnCurrentNumber)
        {
            numOfDecimalDigitsOnCurrentNumber++;
        }
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();

        if (op == ' ')
        {
            ++rightNumPos;
        }
    }

    if (display.touchInArea(600, 450, 100, 100) && numOfDigitsEntered < 6 && numOfDecimalDigitsOnCurrentNumber < 2)
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + "4";
        numOfDigitsEntered++;
        if(decimalPointOnCurrentNumber)
        {
            numOfDecimalDigitsOnCurrentNumber++;
        }
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();

        if (op == ' ')
        {
            ++rightNumPos;
        }
    }

    if (display.touchInArea(900, 350, 100, 100) && (op == ' ') && (rightNumPos > 0))
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + " / ";
        op = '/';
        decimalPointOnCurrentNumber = false;
        numOfDecimalDigitsOnCurrentNumber = 0;
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();
    }

    if (display.touchInArea(800, 350, 100, 100) && numOfDigitsEntered < 6 && numOfDecimalDigitsOnCurrentNumber < 2)
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + "9";
        numOfDigitsEntered++;
        if(decimalPointOnCurrentNumber)
        {
            numOfDecimalDigitsOnCurrentNumber++;
        }
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();

        if (op == ' ')
        {
            ++rightNumPos;
        }
    }

    if (display.touchInArea(700, 350, 100, 100) && numOfDigitsEntered < 6 && numOfDecimalDigitsOnCurrentNumber < 2)
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + "8";
        numOfDigitsEntered++;
        if(decimalPointOnCurrentNumber)
        {
            numOfDecimalDigitsOnCurrentNumber++;
        }
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();

        if (op == ' ')
        {
            ++rightNumPos;
        }
    }

    if (display.touchInArea(600, 350, 100, 100) && numOfDigitsEntered < 6 && numOfDecimalDigitsOnCurrentNumber < 2)
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + "7";
        numOfDigitsEntered++;
        if(decimalPointOnCurrentNumber)
        {
            numOfDecimalDigitsOnCurrentNumber++;
        }
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();

        if (op == ' ')
        {
            ++rightNumPos;
        }
    }
}

float calculate()
{
    double res = 0;
    rightNumPos += 3;

    leftNumber = atof(&text18_content[0]);
    rightNumber = atof(&text18_content[rightNumPos]);
    rightNumPos = 0;

    switch (op)
    {
    case '+':
        res = leftNumber + rightNumber;
        break;
    case '-':
        res = leftNumber - rightNumber;
        break;
    case 'x':
        res = leftNumber * rightNumber;
        break;
    case '/':
        res = leftNumber / rightNumber;
        break;

    default:
        break;
    }

    return res;
}

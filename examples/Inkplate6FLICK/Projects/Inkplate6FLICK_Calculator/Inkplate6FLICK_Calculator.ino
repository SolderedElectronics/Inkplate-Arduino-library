/*
   Inkplate6FLICK_Calculator example for Soldered Inkplate 6FLICK
   For this example you will need USB cable and Inkplate 6FLICK (and a calculator, if you don't trust Inkplate :) ).
   Select "Soldered Inkplate 6FLICK" from Tools -> Board menu.
   Don't have "Soldered Inkplate 6FLICK" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example will show you how simple GUI can be created on Inkplate 6FLICK with this simple calculator example.
   You can do simple math calculations on this calculator (like addition, subtraction, multiplication, division).

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   15 March 2024 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE6FLICK
#error "Wrong board selection for this example, please select Soldered Inkplate 6FLICK"
#endif

#include "Calculator.h"
#include "Inkplate.h"

// Create Inkplate object
Inkplate display(INKPLATE_1BIT);

// Variables which store the state of the calculator
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
    // Begin serial communication
    Serial.begin(115200);

    // Initialize Inkplate object and clear the display buffer

    display.begin(); 
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
    // Periodically check touchscreen and perform events based on touches
    keysEvents();
    delay(20);
}

// This function contains all the events which occur when interacting with the calculator
// Eg. writing numbers, selecting the mathematical operations, calculating and clearing the display
void keysEvents()
{
    if (display.touchInArea(800, 20, 200, 80)) // Refresh
    {
        // Clear screen
        display.clearDisplay();
        mainDraw();
        display.display();
    }

    if (display.touchInArea(600, 20, 200, 80)) // Clear
    {
        // Reset the state of the calculator
        text18_cursor_x = 800;
        text18_cursor_y = 260;
        text18_content = "";
        op = ' ';
        rightNumPos = 0;
        decimalPointOnCurrentNumber = false;
        numOfDigitsEntered = 0;
        numOfDecimalDigitsOnCurrentNumber = 0;

        // Clear screen and redraw using partialUpdate
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();
    }

    if (display.touchInArea(50, 50, 100, 50)) // Clear history
    {
        text19_content = "";
        text19_cursor_x = 50;
        text19_cursor_y = 700;

        // Clear screen and redraw using partialUpdate
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();
    }

    if (display.touchInArea(800, 650, 100, 100) && (op != ' ') && atof(&text18_content[rightNumPos+3]) != 0) // Calculate
    {
        // Do calculation
        result = calculate();

        // Create result string
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

        // Clear screen and redraw using partialUpdate
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();

        // Reset the state of the calculator
        text18_content = "";
        text18_cursor_x = 800;
        text18_cursor_y = 260;
        result = 0;
        op = ' ';
        decimalPointOnCurrentNumber = false;
        numOfDigitsEntered = 0;
        numOfDecimalDigitsOnCurrentNumber = 0;
    }

    if (display.touchInArea(900, 650, 100, 100) && (op == ' ') && (rightNumPos > 0)) // Sum
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + " + ";
        op = '+';
        decimalPointOnCurrentNumber = false;
        numOfDecimalDigitsOnCurrentNumber = 0;

        // Clear screen and redraw using partialUpdate
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();
    }

    if (display.touchInArea(700, 650, 100, 100) && numOfDigitsEntered < 6 && numOfDecimalDigitsOnCurrentNumber < 2) // 0
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + "0";
        numOfDigitsEntered++;
        if(decimalPointOnCurrentNumber)
        {
            numOfDecimalDigitsOnCurrentNumber++;
        }

        // Clear screen and redraw using partialUpdate
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();

        if (op == ' ')
        {
            ++rightNumPos;
        }
    }

    if (display.touchInArea(600, 650, 100, 100) && !decimalPointOnCurrentNumber && numOfDigitsEntered < 6) // Decimal point
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + ".";

        // Clear screen and redraw using partialUpdate
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();

        if (op == ' ')
        {
            ++rightNumPos;
        }

        // Set flag that decimal point has been used
        decimalPointOnCurrentNumber = true;
    }

    if (display.touchInArea(900, 550, 100, 100) && (op == ' ') && (rightNumPos > 0)) // Subtraction
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + " - ";
        op = '-';
        decimalPointOnCurrentNumber = false;
        numOfDecimalDigitsOnCurrentNumber = 0;

        // Clear screen and redraw using partialUpdate
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();
    }

    if (display.touchInArea(800, 550, 100, 100) && numOfDigitsEntered < 6 && numOfDecimalDigitsOnCurrentNumber < 2) // 3
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + "3";
        numOfDigitsEntered++;
        if(decimalPointOnCurrentNumber)
        {
            numOfDecimalDigitsOnCurrentNumber++;
        }

        // Clear screen and redraw using partialUpdate
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();

        if (op == ' ')
        {
            ++rightNumPos;
        }
    }

    if (display.touchInArea(700, 550, 100, 100) && numOfDigitsEntered < 6 && numOfDecimalDigitsOnCurrentNumber < 2) // 2
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + "2";
        numOfDigitsEntered++;
        if(decimalPointOnCurrentNumber)
        {
            numOfDecimalDigitsOnCurrentNumber++;
        }

        // Clear screen and redraw using partialUpdate
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();

        if (op == ' ')
        {
            ++rightNumPos;
        }
    }

    if (display.touchInArea(600, 550, 100, 100) && numOfDigitsEntered < 6 && numOfDecimalDigitsOnCurrentNumber < 2) // 1
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + "1";
        numOfDigitsEntered++;
        if(decimalPointOnCurrentNumber)
        {
            numOfDecimalDigitsOnCurrentNumber++;
        }

        // Clear screen and redraw using partialUpdate
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();

        if (op == ' ')
        {
            ++rightNumPos;
        }
    }

    if (display.touchInArea(900, 450, 100, 100) && (op == ' ') && (rightNumPos > 0)) // X
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + " x ";
        op = 'x';
        decimalPointOnCurrentNumber = false;
        numOfDecimalDigitsOnCurrentNumber = 0;

        // Clear screen and redraw using partialUpdate
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();
    }

    if (display.touchInArea(800, 450, 100, 100) && numOfDigitsEntered < 6 && numOfDecimalDigitsOnCurrentNumber < 2) // 6
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + "6";
        numOfDigitsEntered++;
        if(decimalPointOnCurrentNumber)
        {
            numOfDecimalDigitsOnCurrentNumber++;
        }

        // Clear screen and redraw using partialUpdate
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();

        if (op == ' ')
        {
            ++rightNumPos;
        }
    }

    if (display.touchInArea(700, 450, 100, 100) && numOfDigitsEntered < 6 && numOfDecimalDigitsOnCurrentNumber < 2) // 5
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + "5";
        numOfDigitsEntered++;
        if(decimalPointOnCurrentNumber)
        {
            numOfDecimalDigitsOnCurrentNumber++;
        }

        // Clear screen and redraw using partialUpdate
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();

        if (op == ' ')
        {
            ++rightNumPos;
        }
    }

    if (display.touchInArea(600, 450, 100, 100) && numOfDigitsEntered < 6 && numOfDecimalDigitsOnCurrentNumber < 2) // 4
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + "4";
        numOfDigitsEntered++;
        if(decimalPointOnCurrentNumber)
        {
            numOfDecimalDigitsOnCurrentNumber++;
        }

        // Clear screen and redraw using partialUpdate
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();

        if (op == ' ')
        {
            ++rightNumPos;
        }
    }

    if (display.touchInArea(900, 350, 100, 100) && (op == ' ') && (rightNumPos > 0)) // Division
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + " / ";
        op = '/';
        decimalPointOnCurrentNumber = false;
        numOfDecimalDigitsOnCurrentNumber = 0;

        // Clear screen and redraw using partialUpdate
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();
    }

    if (display.touchInArea(800, 350, 100, 100) && numOfDigitsEntered < 6 && numOfDecimalDigitsOnCurrentNumber < 2) // 9
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + "9";
        numOfDigitsEntered++;
        if(decimalPointOnCurrentNumber)
        {
            numOfDecimalDigitsOnCurrentNumber++;
        }

        // Clear screen and redraw using partialUpdate
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();

        if (op == ' ')
        {
            ++rightNumPos;
        }
    }

    if (display.touchInArea(700, 350, 100, 100) && numOfDigitsEntered < 6 && numOfDecimalDigitsOnCurrentNumber < 2) // 8
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + "8";
        numOfDigitsEntered++;
        if(decimalPointOnCurrentNumber)
        {
            numOfDecimalDigitsOnCurrentNumber++;
        }

        // Clear screen and redraw using partialUpdate
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();

        if (op == ' ')
        {
            ++rightNumPos;
        }
    }

    if (display.touchInArea(600, 350, 100, 100) && numOfDigitsEntered < 6 && numOfDecimalDigitsOnCurrentNumber < 2) // 7
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + "7";
        numOfDigitsEntered++;
        if(decimalPointOnCurrentNumber)
        {
            numOfDecimalDigitsOnCurrentNumber++;
        }

        // Clear screen and redraw using partialUpdate
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();

        if (op == ' ')
        {
            ++rightNumPos;
        }
    }
}

// Do calculation based on inputs
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

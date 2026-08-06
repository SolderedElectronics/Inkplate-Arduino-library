/**
 **************************************************
 * @file        Inkplate4TEMPERA_Calculator.ino
 * @brief       Simple touchscreen calculator GUI using 1-bit e-paper rendering
 *              and partial updates on Inkplate 4 TEMPERA.
 *
 * @details     This example implements a basic on-screen calculator controlled
 *              entirely via the Inkplate 4 TEMPERA touchscreen. It draws a GUI
 *              keypad and display area (provided by helper code in Calculator.h)
 *              and lets you enter numbers and perform the four basic operations:
 *              addition, subtraction, multiplication, and division.
 *
 *              Touch input is handled with touchInArea() checks for each button.
 *              After most interactions, the UI is redrawn and updated using
 *              partialUpdate() to reduce flashing and improve responsiveness.
 *              Results are shown on-screen and the last expression/result can
 *              be stored as a simple "history" line.
 *
 *              Display mode is 1-bit (BW), since partial updates are supported
 *              only in BW mode; for best image quality perform a full refresh
 *              periodically, as repeated partial updates can leave artifacts on
 *              e-paper. Touchscreen init is required - if it fails the UI may
 *              still draw, but touch interaction will not work reliably. Division
 *              by zero is guarded before calculating (the right operand must be
 *              non-zero to trigger calculation). GUI layout, fonts and helper
 *              variables (e.g. text18_content/text19_content) are defined in
 *              Calculator.h, so keep that file with this example.
 *
 *              Expected output: a calculator UI with buttons, an input line and a
 *              history/result line, updating as buttons are tapped, plus the
 *              touchscreen init status on Serial.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 4 TEMPERA
 * - Hardware:   Inkplate 4 TEMPERA, USB-C cable
 * - Extra:      none
 * - Serial:     115200 baud (optional, for touchscreen init messages)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 4 TEMPERA"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 4 TEMPERA.
 * 3) Use the touchscreen buttons to enter a number.
 * 4) Tap an operator (+, -, x, /), enter the second number, then tap '='.
 * 5) Use "Clear" to reset current input, "Clear history" to erase the history
 *    line, and "Refresh" to redraw the full UI.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/4tempera/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2023-07-26
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE4TEMPERA
#error "Wrong board selection for this example, please select Inkplate 4 TEMPERA in the boards menu."
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
    if (!display.touchscreen.init(true))
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
    if (display.touchscreen.touchInArea(30, 130, 150, 50)) // Refresh
    {
        // Clear screen
        display.clearDisplay();
        mainDraw();
        display.display();
    }

    if (display.touchscreen.touchInArea(30, 80, 150, 50)) // Clear
    {
        // reset the state of the calculator
        text18_cursor_x = 550;
        text18_cursor_y = 144;
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

    if (display.touchscreen.touchInArea(30, 30, 150, 50)) // Clear history
    {
        text19_content = "";
        text19_cursor_x = 240;
        text19_cursor_y = 81;

        // Clear screen and redraw using partialUpdate
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();
    }

    if (display.touchscreen.touchInArea(300, 471, 135, 97) && (op != ' ') &&
        atof(&text18_content[rightNumPos + 3]) != 0) // Calculate
    {
        // Do calculation
        result = calculate();

        // Create the result string
        text19_content = text18_content + " = " + result;

        // Clear screen and redraw using partialUpdate
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();

        // reset the state of the calculator
        text18_content = "";
        text18_cursor_x = 550;
        text18_cursor_y = 144;
        result = 0;
        op = ' ';
        decimalPointOnCurrentNumber = false;
        numOfDigitsEntered = 0;
        numOfDecimalDigitsOnCurrentNumber = 0;
    }

    if (display.touchscreen.touchInArea(435, 471, 135, 97) && (op == ' ') && (rightNumPos > 0)) // Sum
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

    if (display.touchscreen.touchInArea(165, 471, 135, 97) && numOfDigitsEntered < 6 && numOfDecimalDigitsOnCurrentNumber < 2) // 0
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + "0";
        numOfDigitsEntered++;
        if (decimalPointOnCurrentNumber)
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

    if (display.touchscreen.touchInArea(30, 471, 135, 97) && !decimalPointOnCurrentNumber &&
        numOfDigitsEntered < 6) // Decimal point
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

    if (display.touchscreen.touchInArea(435, 374, 135, 97) && (op == ' ') && (rightNumPos > 0)) // Subtraction
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

    if (display.touchscreen.touchInArea(300, 374, 135, 97) && numOfDigitsEntered < 6 && numOfDecimalDigitsOnCurrentNumber < 2) // 3
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + "3";
        numOfDigitsEntered++;
        if (decimalPointOnCurrentNumber)
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

    if (display.touchscreen.touchInArea(165, 374, 135, 97) && numOfDigitsEntered < 6 && numOfDecimalDigitsOnCurrentNumber < 2) // 2
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + "2";
        numOfDigitsEntered++;
        if (decimalPointOnCurrentNumber)
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

    if (display.touchscreen.touchInArea(30, 374, 135, 97) && numOfDigitsEntered < 6 && numOfDecimalDigitsOnCurrentNumber < 2) // 1
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + "1";
        numOfDigitsEntered++;
        if (decimalPointOnCurrentNumber)
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

    if (display.touchscreen.touchInArea(435, 277, 135, 97) && (op == ' ') && (rightNumPos > 0)) // X
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

    if (display.touchscreen.touchInArea(300, 277, 135, 97) && numOfDigitsEntered < 6 && numOfDecimalDigitsOnCurrentNumber < 2) // 6
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + "6";
        numOfDigitsEntered++;
        if (decimalPointOnCurrentNumber)
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

    if (display.touchscreen.touchInArea(165, 277, 135, 97) && numOfDigitsEntered < 6 && numOfDecimalDigitsOnCurrentNumber < 2) // 5
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + "5";
        numOfDigitsEntered++;
        if (decimalPointOnCurrentNumber)
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

    if (display.touchscreen.touchInArea(30, 277, 135, 97) && numOfDigitsEntered < 6 && numOfDecimalDigitsOnCurrentNumber < 2) // 4
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + "4";
        numOfDigitsEntered++;
        if (decimalPointOnCurrentNumber)
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

    if (display.touchscreen.touchInArea(435, 180, 135, 97) && (op == ' ') && (rightNumPos > 0)) // Division
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

    if (display.touchscreen.touchInArea(300, 180, 135, 97) && numOfDigitsEntered < 6 && numOfDecimalDigitsOnCurrentNumber < 2) // 9
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + "9";
        numOfDigitsEntered++;
        if (decimalPointOnCurrentNumber)
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

    if (display.touchscreen.touchInArea(168, 180, 135, 97) && numOfDigitsEntered < 6 && numOfDecimalDigitsOnCurrentNumber < 2) // 8
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + "8";
        numOfDigitsEntered++;
        if (decimalPointOnCurrentNumber)
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

    if (display.touchscreen.touchInArea(30, 180, 135, 97) && numOfDigitsEntered < 6 && numOfDecimalDigitsOnCurrentNumber < 2) // 7
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + "7";
        numOfDigitsEntered++;
        if (decimalPointOnCurrentNumber)
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

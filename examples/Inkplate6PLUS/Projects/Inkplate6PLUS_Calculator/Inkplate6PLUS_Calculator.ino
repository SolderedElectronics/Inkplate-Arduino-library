/**
 **************************************************
 * @file        Inkplate6PLUS_Calculator.ino
 * @brief       Touchscreen calculator GUI for Inkplate 6PLUS with partial
 *              e-paper updates.
 *
 * @details     This example implements a simple on-screen calculator using the
 *              Inkplate 6PLUS touchscreen. It draws a calculator-style GUI and
 *              handles touch events for number entry, decimal point input, basic
 *              operators (+, -, x, /), calculation, and clearing the current
 *              entry or the on-screen history.
 *
 *              The display runs in 1-bit (BW) mode and uses partialUpdate() for
 *              responsive UI redraws after each touch. E-paper partial updates
 *              accumulate ghosting, so a full refresh is recommended
 *              periodically (about every 5-10 partial updates); the example
 *              provides a dedicated Refresh button for a full redraw. The
 *              example is interactive and does not use deep sleep.
 *
 *              Expected output: the calculator GUI with the current
 *              expression/result and a running history on the display, plus the
 *              touchscreen init status (e.g. a failure message if init fails) on
 *              the Serial Monitor.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6PLUS
 * - Hardware:   Inkplate 6PLUS, USB cable
 * - Extra:      none
 * - Serial:     115200 baud (optional; used for init status messages)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate6PLUS"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 6PLUS.
 * 3) After boot, the calculator UI appears on the e-paper display.
 * 4) Tap digits and operators to build an expression, then tap "=" to calculate.
 * 5) Use "Clear" to reset the current entry or "Clear history" to erase history.
 * 6) Use "Refresh" to perform a full redraw and reduce partial-update ghosting.
 *
 * @note        Quick start guide: Inkplate 6PLUS has no dedicated page yet,
 *              see https://docs.soldered.com/inkplate/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2024-03-15
 * @license     GNU GPL V3
 **************************************************/
// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_INKPLATE6PLUS) && !defined(ARDUINO_INKPLATE6PLUSV2)
#error                                                                                                                 \
    "Wrong board selection for this example, please select e-radionica Inkplate 6Plus or Soldered Inkplate 6Plus in the boards menu."
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
    // Uncomment this line if you have a USB Power Only Inkplate6PLUS
    // Must be called before display.begin()!
    //display.setInkplatePowerMode(INKPLATE_USB_PWR_ONLY);
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
    if (display.touchscreen.touchInArea(800, 20, 200, 80)) // Refresh
    {
        // Clear screen
        display.clearDisplay();
        mainDraw();
        display.display();
    }

    if (display.touchscreen.touchInArea(600, 20, 200, 80)) // Clear
    {
        // reset the state of the calculator
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

    if (display.touchscreen.touchInArea(50, 50, 100, 50)) // Clear history
    {
        text19_content = "";
        text19_cursor_x = 50;
        text19_cursor_y = 700;

        // Clear screen and redraw using partialUpdate
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();
    }

    if (display.touchscreen.touchInArea(800, 650, 100, 100) && (op != ' ') && atof(&text18_content[rightNumPos+3]) != 0) // Calculate
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

        // reset the state of the calculator
        text18_content = "";
        text18_cursor_x = 800;
        text18_cursor_y = 260;
        result = 0;
        op = ' ';
        decimalPointOnCurrentNumber = false;
        numOfDigitsEntered = 0;
        numOfDecimalDigitsOnCurrentNumber = 0;
    }

    if (display.touchscreen.touchInArea(900, 650, 100, 100) && (op == ' ') && (rightNumPos > 0)) // Sum
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

    if (display.touchscreen.touchInArea(700, 650, 100, 100) && numOfDigitsEntered < 6 && numOfDecimalDigitsOnCurrentNumber < 2) // 0
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

    if (display.touchscreen.touchInArea(600, 650, 100, 100) && !decimalPointOnCurrentNumber && numOfDigitsEntered < 6) // Decimal point
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

    if (display.touchscreen.touchInArea(900, 550, 100, 100) && (op == ' ') && (rightNumPos > 0)) // Subtraction
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

    if (display.touchscreen.touchInArea(800, 550, 100, 100) && numOfDigitsEntered < 6 && numOfDecimalDigitsOnCurrentNumber < 2) // 3
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

    if (display.touchscreen.touchInArea(700, 550, 100, 100) && numOfDigitsEntered < 6 && numOfDecimalDigitsOnCurrentNumber < 2) // 2
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

    if (display.touchscreen.touchInArea(600, 550, 100, 100) && numOfDigitsEntered < 6 && numOfDecimalDigitsOnCurrentNumber < 2) // 1
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

    if (display.touchscreen.touchInArea(900, 450, 100, 100) && (op == ' ') && (rightNumPos > 0)) // X
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

    if (display.touchscreen.touchInArea(800, 450, 100, 100) && numOfDigitsEntered < 6 && numOfDecimalDigitsOnCurrentNumber < 2) // 6
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

    if (display.touchscreen.touchInArea(700, 450, 100, 100) && numOfDigitsEntered < 6 && numOfDecimalDigitsOnCurrentNumber < 2) // 5
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

    if (display.touchscreen.touchInArea(600, 450, 100, 100) && numOfDigitsEntered < 6 && numOfDecimalDigitsOnCurrentNumber < 2) // 4
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

    if (display.touchscreen.touchInArea(900, 350, 100, 100) && (op == ' ') && (rightNumPos > 0)) // Division
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

    if (display.touchscreen.touchInArea(800, 350, 100, 100) && numOfDigitsEntered < 6 && numOfDecimalDigitsOnCurrentNumber < 2) // 9
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

    if (display.touchscreen.touchInArea(700, 350, 100, 100) && numOfDigitsEntered < 6 && numOfDecimalDigitsOnCurrentNumber < 2) // 8
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

    if (display.touchscreen.touchInArea(600, 350, 100, 100) && numOfDigitsEntered < 6 && numOfDecimalDigitsOnCurrentNumber < 2) // 7
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

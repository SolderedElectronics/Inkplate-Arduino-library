// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE6PLUS
    #error "Wrong board selection for this example, please select Inkplate 6 plus in the boards menu."
#endif

#include "Inkplate.h"
#include "Calculator.h"

Inkplate display(INKPLATE_1BIT);

#define X_REZ_OFFSET 15

double leftNumber = 0;
double rightNumber = 0;
char op = ' ';
double result = 0;
int rightNumPos = 0;

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
    if (display.touchInArea(800, 20, 200, 80))//Refresh
    {
        display.clearDisplay();
        mainDraw();
        display.display();
    }

    if (display.touchInArea(600, 20, 200, 80))//Clear
    {
        text18_cursor_x = 800;
        text18_cursor_y = 260;
        text18_content = "";
        op = ' ';
        rightNumPos = 0;
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();
    }

    if (display.touchInArea(50, 50, 100, 50))//Clear history
    {
        text19_content = "";
        text19_cursor_x = 50;
        text19_cursor_y = 700;
        
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();
    }

    if (display.touchInArea(800, 650, 100, 100) && (op != ' '))//Calculate
    {
        result = calculate();
        
        
        if(text19_content == "")
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
    }

    if (display.touchInArea(900, 650, 100, 100) && (op == ' ') && (rightNumPos > 0))
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + " + ";
        op = '+';
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();
    }

    if (display.touchInArea(700, 650, 100, 100))
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + "0";
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();
        
        if(op == ' ')
        {
            ++rightNumPos;
        }
    }

    if (display.touchInArea(600, 650, 100, 100))
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + ".";
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();
        
        if(op == ' ')
        {
            ++rightNumPos;
        }
    }

    if (display.touchInArea(900, 550, 100, 100) && (op == ' ') && (rightNumPos > 0))
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + " - ";
        op = '-';
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();
    }

    if (display.touchInArea(800, 550, 100, 100))
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + "3";
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();
        
        if(op == ' ')
        {
            ++rightNumPos;
        }
    }

    if (display.touchInArea(700, 550, 100, 100))
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + "2";
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();

        if(op == ' ')
        {
            ++rightNumPos;
        }
    }

    if (display.touchInArea(600, 550, 100, 100))
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + "1";
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();

        if(op == ' ')
        {
            ++rightNumPos;
        }
    }

    if (display.touchInArea(900, 450, 100, 100) && (op == ' ') && (rightNumPos > 0))
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + " x ";
        op = 'x';
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();
    }

    if (display.touchInArea(800, 450, 100, 100))
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + "6";
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();
        
        if(op == ' ')
        {
            ++rightNumPos;
        }
    }

    if (display.touchInArea(700, 450, 100, 100))
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + "5";
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();
        
        if(op == ' ')
        {
            ++rightNumPos;
        }
    }

    if (display.touchInArea(600, 450, 100, 100))
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + "4";
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();
        
        if(op == ' ')
        {
            ++rightNumPos;
        }
    }

    if (display.touchInArea(900, 350, 100, 100) && (op == ' ') && (rightNumPos > 0))
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + " / ";
        op = '/';
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();
    }

    if (display.touchInArea(800, 350, 100, 100))
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + "9";
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();
        
        if(op == ' ')
        {
            ++rightNumPos;
        }
    }

    if (display.touchInArea(700, 350, 100, 100))
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + "8";
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();
        
        if(op == ' ')
        {
            ++rightNumPos;
        }
    }

    if (display.touchInArea(600, 350, 100, 100))
    {
        text18_cursor_x -= X_REZ_OFFSET;
        text18_content = text18_content + "7";
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();
        
        if(op == ' ')
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

    //Serial.println(leftNumber);
    //Serial.println(rightNumber);
    //Serial.println(rightNumPos);
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
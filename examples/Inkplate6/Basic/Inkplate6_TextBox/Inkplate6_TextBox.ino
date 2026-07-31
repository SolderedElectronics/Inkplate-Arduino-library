/**
 **************************************************
 * @file        Inkplate6_TextBox.ino
 * @brief       TextBox usage example for Soldered Inkplate 6.
 *
 * @details     Demonstrates how to use the drawTextBox() function to render
 *              multi-line text inside a defined rectangular area.
 *              The example shows:
 *              - A basic TextBox with default parameters.
 *              - A fully customized TextBox using a custom font,
 *                text scaling, spacing, and optional border.
 *
 *              If a word does not fit at the end of a row, it automatically
 *              wraps to the next line. If the text exceeds the lower boundary
 *              of the box, it ends with three dots (...) to indicate that
 *              not all text is displayed.
 *
 *              The example runs in 1-bit (black & white) mode. Some custom
 *              fonts are drawn bottom-to-top and may require a vertical offset
 *              for correct positioning. Always call display.display() after
 *              drawing operations to update the physical e-paper screen.
 *              Expected result is two text boxes rendered on the screen: a
 *              default TextBox and a custom styled TextBox using the Roboto
 *              font.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6
 * - Hardware:   Inkplate 6, USB cable
 * - Extra:      Custom font file (e.g. Roboto_Light_36.h)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate6"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 6.
 * 3) Both text boxes are drawn and shown after the display refreshes.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/6/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2025-04-24
 * @license     GNU GPL V3
 **************************************************/

#include "Inkplate.h"            //Include Inkplate library to the sketch
#include "Roboto_Light_36.h"
Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1 Bit mode (BW)

// Define the text you will show in the text box
const char* text="This is an example of a text written in a textbox. When a word doesn't fit into the current row, it goes to the next one."\
" If the text reaches the lower bound, it ends with three dots (...) to mark that the text isnt displayed fully";

void setup()
{
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.display();      // Put clear image on display

    // Create a text box without any optional parameters
    // x0- x coordinate of upper left corner
    // y0- y coordinate of upper left corner
    // x1- x coordinate of bottom right corner
    // y1- y coordinate of bottom right corner
    // text - text we want to display
    display.drawTextBox(100,100,300,300,text);

    // Create a text box with all parameters
    // x0- x coordinate of upper left corner
    // y0- y coordinate of upper left corner
    // x1- x coordinate of bottom right corner
    // y1- y coordinate of bottom right corner
    // text - text we want to display
    // textSizeMultiplier - by what factor we want to enlarge the size of a font
    // font - address of selected custom font
    // verticalSpacing - how many pixels between each row of text
    // showBorder - Create a visible rectangle around the box
    // fontSize - size of the used font in pt
    int offset=32; // Note - some custom fonts are drawn from bottom-to-top which requires an offset, use an offset that best suits the font you use 
    display.drawTextBox(400,100+offset,600,300,text,1,&Roboto_Light_36,27,false,36);

    // Display both text boxes
    display.display();
}

void loop()
{
    // Nothing...
}

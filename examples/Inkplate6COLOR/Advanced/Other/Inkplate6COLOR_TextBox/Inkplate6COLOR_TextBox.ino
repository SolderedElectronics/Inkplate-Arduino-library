/**
 **************************************************
 * @file        Inkplate6COLOR_TextBox.ino
 * @brief       Demonstrates automatic text wrapping using the drawTextBox()
 *              function on Inkplate 6COLOR.
 *
 * @details     This example shows how to render formatted text inside a
 *              rectangular text box on the Inkplate 6COLOR display. The
 *              drawTextBox() helper function automatically wraps words to the
 *              next line when they no longer fit within the horizontal bounds
 *              of the defined box.
 *
 *              If the text reaches the bottom of the box, the function stops
 *              rendering and adds an ellipsis ("...") to indicate that the
 *              content has been truncated.
 *
 *              The example demonstrates two ways of using drawTextBox():
 *
 *              1) A minimal version with only the required parameters
 *                 (coordinates and text).
 *
 *              2) An extended version that uses optional parameters to control
 *                 font selection, font size scaling, vertical spacing, and
 *                 whether a border should be drawn around the text box.
 *
 *              The second example also uses a custom font (Roboto Light) to
 *              demonstrate how external fonts can be used with text boxes.
 *              Custom fonts may render relative to the font baseline, which can
 *              require a vertical offset adjustment when positioning the text
 *              box. Each display update performs a full refresh on the colour
 *              e-paper panel. drawTextBox() simplifies UI layout for menus,
 *              descriptions and other formatted text content.
 *
 *              Expected output: two rectangular text areas containing wrapped
 *              text, where words automatically move to the next line at the box
 *              edge and text exceeding the vertical space ends with "...".
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6COLOR
 * - Hardware:   Inkplate 6COLOR, USB cable
 * - Extra:      Roboto_Light_36 font file present in the sketch folder
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 6COLOR"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 6COLOR.
 * 3) After startup, the display is cleared.
 * 4) The sketch renders two text boxes: one using the default font and minimal
 *    parameters, one using a custom font with advanced formatting options.
 * 5) The display refreshes to show both formatted text regions.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/6color/quick-start-guide/
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
Inkplate display; // Create an object on Inkplate library and also set library into 1 Bit mode (BW)

// Define the text you will show in the text box
const char* text="This is an example of a text written in a textbox. When a word doesn't fit into the current row, it goes to the next one."\
" If the text reaches the lower bound, it ends with three dots (...) to mark that the text isnt displayed fully";

void setup()
{
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.display();      // Put clear image on display
    display.setTextColor(INKPLATE_BLACK);

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

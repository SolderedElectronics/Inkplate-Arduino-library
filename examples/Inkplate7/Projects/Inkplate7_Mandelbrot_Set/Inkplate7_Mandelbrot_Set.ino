/*
   Inkplate7_Mandelbrot_Set example for Soldered Inkplate 7
   For this example you will need only a USB-C cable and Inkplate 7.
   Select "Soldered Inkplate7" from Tools -> Board menu.
   Don't have "Soldered Inkplate7" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example renders the mandelbrot set to coordiantes to Inkplate.
   Due to the nature of Mandelbrot set, it is quite slow on low powered MCUs, so please be patient :)

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   4 April 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE7
#error "Wrong board selection for this example, please select Soldered Inkplate7 in the boards menu."
#endif

// Include Inkplate library to the sketch
#include "Inkplate.h"

#define MAXITERATIONS 150

// Create object on Inkplate library
Inkplate display;

// It takes a long time to render, cca a half minute for the partial set, 45 sec for the whole set
// You can watch the progress on the serial monitor, when it gets to 299, it's over

// Explore different positions to draw
// Some interesting ones can be found here http://www.cuug.ab.ca/dewara/mandelbrot/Mandelbrowser.html
double xFrom = -0.7423, xTo = -0.8463;
double yFrom = 0.1092, yTo = 0.2102;

void setup()
{
    // Init serial communication to see rendering progress
    Serial.begin(115200);

    // Init library (you should call this function ONLY ONCE)
    display.begin();

    // Clear the display and draw the set
    display.clearDisplay();
    for (int j = 0; j < E_INK_HEIGHT; ++j)
    {
        for (int i = 0; i < E_INK_WIDTH; ++i)
        {
            display.drawPixel(
                i, j, colorAt(xFrom + (double)i * (xTo - xFrom) / 400.0, yFrom + (double)j * (yTo - yFrom) / 300.0));

            // For whole set:
            // display.drawPixel(i, j, colorAt(-2.0 + (3.0 * (double)i / 400.0), -1.0 + 2.0 * (double)j / 300.0));
        }

        // Print rendering progress on the Serial Monitor
        Serial.println(j);
    }

    // When it gets to 299, it's over. Print a message and display the set
    Serial.println("That's it!");
    display.display();
}

void loop()
{
    // Empty...
}

struct complex
{
    double re;
    double im;
};

void addComplex(struct complex *z, struct complex *c)
{
    z->re += c->re;
    z->im += c->im;
}

void squareComplex(struct complex *z)
{
    double re = z->re;
    double im = z->im;
    z->re = re * re - im * im;
    z->im = 2 * re * im;
}

double modulusComplexSqr(struct complex *z)
{
    return z->re * z->re + z->im * z->im;
}

uint8_t colorAt(double x, double y)
{
    struct complex z = {0.0, 0.0};
    struct complex c = {x, y};

    int i;
    for (i = 0; i < MAXITERATIONS && modulusComplexSqr(&z) <= 4.0; ++i)
    {
        squareComplex(&z);
        addComplex(&z, &c);
    }
    return i == MAXITERATIONS;
}

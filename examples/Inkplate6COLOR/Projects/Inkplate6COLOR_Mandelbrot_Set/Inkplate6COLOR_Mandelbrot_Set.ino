/**
 **************************************************
 * @file        Inkplate6COLOR_Mandelbrot_Set.ino
 * @brief       Renders a Mandelbrot fractal view on Inkplate 6COLOR.
 *
 * @details     This example calculates and draws the Mandelbrot set directly on
 *              the Inkplate 6COLOR display. For each screen pixel, the sketch
 *              maps the pixel position to a complex-plane coordinate, runs the
 *              Mandelbrot iteration, and decides whether that point belongs to
 *              the set.
 *
 *              The selected coordinate range in this example focuses on a zoomed
 *              region of the fractal, but the bounds can be changed to explore
 *              other parts of the Mandelbrot set, including the full set view -
 *              some regions reveal far more detail than the full-set overview.
 *              Because fractal rendering requires many floating-point operations
 *              per pixel, this example is computationally heavy and can take a
 *              long time to complete on a microcontroller; increasing
 *              MAXITERATIONS improves fractal detail but also increases
 *              rendering time.
 *
 *              Rendering progress is reported over Serial one row at a time, and
 *              the completed image is shown only after the full framebuffer has
 *              been calculated and sent to the display in a single full refresh.
 *              The board runs in Inkplate 6COLOR colour e-paper mode, but this
 *              example draws using black/white pixel values only.
 *
 *              Expected output: a Mandelbrot fractal rendered as a monochrome
 *              image on the panel, with a startup message and row-by-row
 *              progress output on Serial during rendering.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6COLOR
 * - Hardware:   Inkplate 6COLOR, USB cable
 * - Extra:      none
 * - Serial:     115200 baud
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 6COLOR"
 *    from Tools -> Board.
 * 2) Optionally adjust xFrom, xTo, yFrom and yTo in the sketch to explore
 *    different fractal regions.
 * 3) Upload the sketch to Inkplate 6COLOR.
 * 4) Open Serial Monitor at 115200 baud to observe rendering progress.
 * 5) Wait while the sketch computes the fractal row by row.
 * 6) After rendering finishes, the completed Mandelbrot image is refreshed on
 *    the display.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/6color/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2020-07-15
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
#endif

#include "Inkplate.h"

#define MAXITERATIONS 150

Inkplate display;

// Takes a long time to render, approx. 3 minutes

// Explore different positions to draw
// Some interesting ones can be found here http://www.cuug.ab.ca/dewara/mandelbrot/Mandelbrowser.html
double xFrom = -0.7423, xTo = -0.8463;
double yFrom = 0.1092, yTo = 0.2102;

void setup()
{
    Serial.begin(115200);
    display.begin();
    Serial.println("Started");
}

void loop()
{
    display.clearDisplay();
    for (int j = 0; j < E_INK_HEIGHT; ++j)
    {
        for (int i = 0; i < E_INK_WIDTH; ++i)
            display.drawPixel(
                i, j, colorAt(xFrom + (double)i * (xTo - xFrom) / 800.0, yFrom + (double)j * (yTo - yFrom) / 600.0));
        // for whole set:
        // display.drawPixel(i, j, colorAt(-2.0 + (3.0 * (double)i / 800.0), -1.0 + 2.0 * (double)j / 600.0));
        Serial.println(j);
    }
    display.display();
    delay(5000);
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

/**
 **************************************************
 * @file        Inkplate4TEMPERA_Mandelbrot_Set.ino
 * @brief       Render a Mandelbrot set view on Inkplate 4 TEMPERA in 1-bit
 *              (BW) mode by computing each pixel on the ESP32.
 *
 * @details     This example computes and renders a Mandelbrot set region by
 *              iterating the complex function z = z^2 + c for each pixel on the
 *              display. For every (x, y) coordinate mapped into the complex
 *              plane, the code runs up to MAXITERATIONS iterations and decides
 *              whether the point escapes. Points that do not escape are drawn
 *              as black (inside the set) and others remain white.
 *
 *              Rendering is performed fully in the ESP32 frame buffer and then
 *              pushed to the e-paper panel with a full refresh via
 *              display.display(); partial updates are not used, so the whole
 *              image is generated before the first refresh. Because the
 *              computation is heavy and uses double-precision math, this sketch
 *              can take a long time to complete on an MCU and draws more power
 *              while computing. MAXITERATIONS controls detail vs. speed:
 *              increasing it improves boundary detail but increases render time.
 *
 *              Expected output: a black/white Mandelbrot set rendering of the
 *              selected coordinate window, with the row index (j) printed on
 *              Serial as progress while rendering.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 4 TEMPERA
 * - Hardware:   Inkplate 4 TEMPERA, USB-C cable
 * - Extra:      none
 * - Serial:     115200 baud (optional; prints progress per rendered row)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 4 TEMPERA"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 4 TEMPERA.
 * 3) Wait for the render to complete (may take several minutes).
 * 4) The final Mandelbrot image is displayed; after a short delay the render
 *    repeats.
 * 5) To explore other regions, change xFrom/xTo/yFrom/yTo and re-upload.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/4tempera/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2023-07-24
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE4TEMPERA
#error "Wrong board selection for this example, please select Inkplate 4 TEMPERA in the boards menu."
#endif

#include "Inkplate.h"

#define MAXITERATIONS 150

Inkplate display(INKPLATE_1BIT);

// Takes a long time to render, approx. 3 minutes

// Explore different positions to draw
// Some interesting ones can be found here http://www.cuug.ab.ca/dewara/mandelbrot/Mandelbrowser.html
double xFrom = -0.7423, xTo = -0.8463;
double yFrom = 0.1092, yTo = 0.2102;

void setup()
{
    Serial.begin(115200);
    display.begin();
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

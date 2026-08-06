/**
 **************************************************
 * @file        Inkplate6FLICK_Mandelbrot_Set.ino
 * @brief       Render the Mandelbrot set on Inkplate 6FLICK in 1-bit (BW) mode.
 *
 * @details     This example computes and renders the Mandelbrot set by iterating
 *              the complex recurrence z = z^2 + c for each screen pixel. Pixels
 *              that do not escape within MAXITERATIONS are drawn as black (inside
 *              the set); pixels that escape are drawn as white (outside).
 *
 *              Rendering is performed entirely on the MCU and is computationally
 *              intensive, so drawing a full frame can take minutes depending on
 *              the chosen view window and iteration limit - high MAXITERATIONS
 *              and wide/zoomed views increase render time significantly.
 *              Progress is printed to the Serial Monitor once per rendered row.
 *
 *              The display runs in 1-bit (BW) mode (INKPLATE_1BIT) and uses a
 *              full-screen refresh (display()) only after all pixels are
 *              computed. The example does not use partial updates, touchscreen,
 *              WiFi or deep sleep.
 *
 *              Expected output: a black/white Mandelbrot rendering for the
 *              configured coordinate window, with the current row index (j)
 *              printed on the Serial Monitor during rendering.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6FLICK
 * - Hardware:   Inkplate 6FLICK, USB cable
 * - Extra:      none
 * - Serial:     115200 baud (recommended to see row progress)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate6FLICK"
 *    from Tools -> Board.
 * 2) Optionally adjust the Mandelbrot view window (xFrom/xTo/yFrom/yTo) and
 *    MAXITERATIONS, which controls detail vs. render time.
 * 3) Upload the sketch to Inkplate 6FLICK.
 * 4) Wait while the frame is computed (this can take a few minutes).
 * 5) After rendering completes, the image is shown on the display.
 * 6) The sketch waits briefly and then recomputes the same view again.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/6flick/quick-start-guide/
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
#ifndef ARDUINO_INKPLATE6FLICK
#error "Wrong board selection for this example, please select Soldered Inkplate 6 FLICK"
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

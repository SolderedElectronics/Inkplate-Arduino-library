/**
 **************************************************
 * @file        Inkplate6_Mandelbrot_Set.ino
 * @brief       Renders the Mandelbrot set on Inkplate 6 using per-pixel
 *              iteration in 1-bit black/white mode.
 *
 * @details     This example demonstrates CPU-based rendering of the Mandelbrot
 *              fractal directly into the Inkplate framebuffer. For each pixel
 *              on the 1200x825 e-paper panel, the sketch maps screen
 *              coordinates to a region of the complex plane and iterates the
 *              Mandelbrot function up to MAXITERATIONS. Pixels that do not
 *              diverge within the iteration limit are drawn as black; others
 *              are white, producing a 1-bit (BW) fractal image.
 *
 *              Rendering is intentionally done in a simple nested loop and is
 *              computationally heavy for a microcontroller. Expect a long
 *              render time before the final full refresh is sent to the panel.
 *              Progress is printed to Serial (one line per rendered row).
 *
 *              Display mode is 1-bit BW (INKPLATE_1BIT); partial updates are
 *              possible in BW mode in general, but this example performs a
 *              full-frame render followed by a full refresh. Rendering speed is
 *              limited by per-pixel double-precision math and the selected
 *              iteration count, so increasing MAXITERATIONS significantly
 *              increases runtime. For a full-set view, a reference mapping is
 *              included in commented code. Expected output is a black/white
 *              Mandelbrot fractal image on the display plus Serial output
 *              showing the current rendered row index (0..824).
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6
 * - Hardware:   Inkplate 6, USB cable
 * - Extra:      none
 * - Serial:     115200 baud
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate6"
 *    from Tools -> Board.
 * 2) Optionally adjust xFrom/xTo/yFrom/yTo to explore different regions and
 *    MAXITERATIONS to trade detail vs. render time.
 * 3) Upload the sketch to Inkplate 6.
 * 4) Open Serial Monitor at 115200 baud to watch rendering progress.
 * 5) Wait for the full frame to finish rendering (can take minutes).
 * 6) After the image is shown, the sketch waits 5 seconds and renders again.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/6/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2021-02-11
 * @license     GNU GPL V3
 **************************************************/
// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_ESP32_DEV) && !defined(ARDUINO_INKPLATE6V2)
#error "Wrong board selection for this example, please select e-radionica Inkplate6 or Soldered Inkplate6 in the boards menu."
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

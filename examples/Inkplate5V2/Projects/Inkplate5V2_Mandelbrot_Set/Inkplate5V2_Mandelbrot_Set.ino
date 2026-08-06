/**
 **************************************************
 * @file        Inkplate5V2_Mandelbrot_Set.ino
 * @brief       Renders the Mandelbrot set on Inkplate 5v2 using per-pixel
 *              iteration in 1-bit black/white mode.
 *
 * @details     This example demonstrates CPU-based rendering of the Mandelbrot
 *              fractal directly into the Inkplate framebuffer. For each pixel
 *              on the 1280x720 e-paper panel, the sketch maps screen
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
 *              showing the current rendered row index (0..719).
 *
 * Requirements:
 * - Board:      Soldered Inkplate 5v2
 * - Hardware:   Inkplate 5v2, USB cable
 * - Extra:      none
 * - Serial:     115200 baud
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate5v2"
 *    from Tools -> Board.
 * 2) Optionally adjust xFrom/xTo/yFrom/yTo to explore different regions and
 *    MAXITERATIONS to trade detail vs. render time.
 * 3) Upload the sketch to Inkplate 5v2.
 * 4) Open Serial Monitor at 115200 baud to watch rendering progress.
 * 5) Wait for the full frame to finish rendering (can take minutes).
 * 6) After the image is shown, the sketch waits 5 seconds and renders again.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/5v2/quick-start-guide/
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
#ifndef ARDUINO_INKPLATE5V2
#error "Wrong board selection for this example, please select Soldered Inkplate5 V2 in the boards menu."
#endif

// Include Inkplate library to the sketch
#include "Inkplate.h"

#define MAXITERATIONS 150

// Create object on Inkplate library and set library to work in Black and White mode (1-bit)
Inkplate display(INKPLATE_1BIT);

// Takes a long time to render, approx. 3 minutes

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
            display.drawPixel(i, j, colorAt(xFrom + (double)i * (xTo - xFrom) / 960.0, yFrom + (double)j * (yTo - yFrom) / 540.0));
            
            // For whole set:
            // display.drawPixel(i, j, colorAt(-2.0 + (3.0 * (double)i / 960.0), -1.0 + 2.0 * (double)j / 540.0));
        }

        // Print rendering progress on the Serial Monitor
        Serial.println(j);
    }

    // When it gets to 539, it's over. Print a message and display the set
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

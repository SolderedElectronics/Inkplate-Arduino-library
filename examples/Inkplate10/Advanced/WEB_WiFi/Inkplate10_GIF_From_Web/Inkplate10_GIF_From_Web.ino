/**
 **************************************************
 * @file        Inkplate10_GIF_From_Web.ino
 * @brief       Download and play a GIF animation from the web (Inkplate 10).
 *
 * @details     Demonstrates how to connect Inkplate 10 to a WiFi network,
 *              download a GIF file from a URL, and play it back on the
 *              e-paper display using partial updates. The whole file is
 *              downloaded into a PSRAM buffer first, then played from there.
 *
 *              Partial update (and therefore GIF playback) only works in
 *              INKPLATE_1BIT mode, hence the display object below is created
 *              with INKPLATE_1BIT. The whole GIF file is held in PSRAM at once
 *              (like the other drawXFromWeb functions in this library), so make
 *              sure the file is small enough to fit. e-paper partial refresh
 *              takes far longer than a typical GIF frame delay, so actual
 *              playback speed is limited by the panel, not by the file.
 *              leaveOn (last argument, defaults to true) keeps the panel powered
 *              between frames instead of power-cycling it on every
 *              partialUpdate() call; pass false to power the panel down after
 *              each frame instead. Expected output is the GIF from gifUrl
 *              playing centered on the display, looping until reset or
 *              power-cycle.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 10
 * - Hardware:   Inkplate 10, USB cable
 * - Extra:      Available WiFi connection
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate10"
 *    from Tools -> Board.
 * 2) Enter your WiFi SSID and password in the sketch.
 * 3) Set gifUrl to a direct link to a GIF file (no HTML redirect pages).
 * 4) Upload the sketch to Inkplate 10.
 * 5) The board connects to WiFi, downloads the GIF, and loops it forever.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/10/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2026-07-01
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_INKPLATE10) && !defined(ARDUINO_INKPLATE10V2)
#error "Wrong board selection for this example, please select e-radionica Inkplate10 or Soldered Inkplate10 in the boards menu."
#endif

// Include needed libraries
#include "Inkplate.h" // Include Inkplate library to the sketch
#include "WiFi.h"     // Include library for WiFi

// Enter your WiFi credentials
const char *ssid = "";
const char *pass = "";

// Direct link to a GIF file (no HTML redirect pages)
const char *gifUrl = "";

Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library, GIF playback requires 1-bit mode

void setup()
{
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.display();      // Put clear image on display

    display.setTextSize(2); // Use larger text for easier readability
    display.print("Connecting to WiFi...");
    display.partialUpdate();

    // Connect to the WiFi network.
    WiFi.mode(WIFI_MODE_STA);
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        display.print(".");
        display.partialUpdate();
    }
    display.println("\nWiFi OK! Downloading...");
    display.partialUpdate();

    display.clearDisplay();

    // Download and play the GIF, looping forever, centered on the screen.
    if (!display.gif.playGifFromWeb(gifUrl, E_INK_WIDTH / 2 - 125, E_INK_HEIGHT / 2 - 125, false, true, 20, true))
    {
        // If something failed (bad URL, download error, unsupported/corrupt GIF), write an error on the screen.
        display.setTextColor(BLACK);
        display.setTextSize(3);
        display.println("GIF download/open error");
        display.display();
    }

    WiFi.mode(WIFI_OFF);
}

void loop()
{
    // Nothing...
}

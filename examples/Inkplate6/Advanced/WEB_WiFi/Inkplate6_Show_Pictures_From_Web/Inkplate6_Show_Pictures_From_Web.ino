/**
 **************************************************
 * @file        Inkplate6_Show_Pictures_From_Web.ino
 * @brief       Download and display BMP images from the web (Inkplate 6).
 *
 * @details     Demonstrates how to connect Inkplate 6 to a WiFi network,
 *              download a BMP image from a web URL (HTTP/HTTPS depending on
 *              the sketch implementation), and render it on the e-paper display
 *              using the Inkplate image drawing functions.
 *
 *              Supported BMP formats are Windows BMP with 1/4/8/24-bit color
 *              depth. Images must fit the display; large images may not render
 *              properly. Make sure the URL points directly to the BMP file (no
 *              HTML redirect pages). Expected output is the downloaded BMP image
 *              shown on the Inkplate screen.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6
 * - Hardware:   Inkplate 6, USB cable
 * - Extra:      Available WiFi connection
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate6"
 *    from Tools -> Board.
 * 2) Enter your WiFi SSID and password in the sketch.
 * 3) Set the image URL to a compatible BMP file.
 * 4) Upload the sketch to Inkplate 6.
 * 5) The board connects to WiFi, downloads the image, and displays it.
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

#include "HTTPClient.h"          //Include library for HTTPClient
#include "Inkplate.h"            //Include Inkplate library to the sketch
#include "WiFi.h"                //Include library for WiFi
Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1 Bit mode (BW)

const char ssid[] = "";    // Your WiFi SSID
const char *password = ""; // Your WiFi password

void setup()
{
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.display();      // Put clear image on display

    display.print("Connecting to WiFi...");
    display.partialUpdate();

    // Connect to the WiFi network.
    WiFi.mode(WIFI_MODE_STA);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        display.print(".");
        display.partialUpdate();
    }
    display.println("\nWiFi OK! Downloading...");
    display.partialUpdate();

    // Draw the first image from web.
    // Monochromatic bitmap with 1 bit depth. Images like this load quickest.
    // NOTE: Both drawImage methods allow for an optional fifth "invert" parameter. Setting this parameter to true
    // will flip all colors on the image, making black white and white black. This may be necessary when exporting
    // bitmaps from certain softwares. Forth parameter will dither the image. Photo taken by: Roberto Fernandez
    if (!display.image.draw("https://varipass.org/neowise_mono.bmp", 0, 0, false, true))
    {
        // If is something failed (wrong filename or wrong bitmap format), write error message on the screen.
        // REMEMBER! You can only use Windows Bitmap file with color depth of 1, 4, 8 or 24 bits with no compression!
        display.println("Image open error");
        display.display();
    }
    display.display();

    // Draw the second image from web, this time using a HTTPClient to fetch the response manually.
    // Full color 24 bit images are large and take a long time to load, will take around 20 secs.
    HTTPClient http;
    // Set parameters to speed up the download process.
    http.getStream().setNoDelay(true);
    http.getStream().setTimeout(1);

    // Photo taken by: Roberto Fernandez
    http.begin("https://varipass.org/neowise.bmp");

    // Check response code.
    int httpCode = http.GET();
    if (httpCode == 200)
    {
        // Get the response length and make sure it is not 0.
        int32_t len = http.getSize();
        if (len > 0)
        {
            if (!display.image.drawBitmapFromWeb(http.getStreamPtr(), 0, 0, len))
            {
                // If is something failed (wrong filename or wrong bitmap format), write error message on the screen.
                // REMEMBER! You can only use Windows Bitmap file with color depth of 1, 4, 8 or 24 bits with no
                // compression!
                display.println("Image open error");
                display.display();
            }
            display.display();
        }
        else
        {
            display.println("Invalid response length");
            display.display();
        }
    }
    else
    {
        display.println("HTTP error");
        display.display();
    }

    display.clearDisplay();
    delay(3000);

    // Try to load image and display it on e-paper at position X=0, Y=100
    // NOTE: Both drawJpegFromWeb methods allow for an optional fifth "invert" parameter. Setting this parameter to
    // true will flip all colors on the image, making black white and white black. forth parameter will dither the
    // image.
    if (!display.image.draw("https://varipass.org/destination.jpg", 0, 100, true, false))
    {
        // If is something failed (wrong filename or format), write error message on the screen.
        display.println("Image open error");
        display.display();
    }
    display.display();

    http.end();

    WiFi.mode(WIFI_OFF);
}

void loop()
{
    // Nothing...
}

/**
 **************************************************
 * @file        Inkplate2_Show_Pictures_From_Web.ino
 * @brief       Download JPG/BMP/PNG images over WiFi and display them on the
 *              Inkplate 2 e-paper display.
 *
 * @details     This example demonstrates how to fetch images from the Internet
 *              using WiFi and HTTP, then render them on Inkplate 2. It shows
 *              two workflows:
 *              - Direct URL drawing via display.image.draw() for JPG/PNG/BMP
 *              - Manual download using HTTPClient and rendering a BMP stream
 *                via drawBitmapFromWeb()
 *
 *              The display runs in 1-bit (black/white) mode and uses a full
 *              refresh (display()). Optional dithering can be enabled when
 *              drawing to convert grayscale/color sources into a 1-bit dithered
 *              image; it improves appearance but increases processing time.
 *
 *              Image constraints: max size 212x104 px (larger images may fail or
 *              render incorrectly); JPG must be baseline DCT with Huffman coding;
 *              BMP must be Windows BMP, 1/4/8/24-bit, uncompressed (no RLE); PNG
 *              is generally supported, and if an image fails, re-save it with an
 *              editor. Large 24-bit images can be slow to download and decode.
 *
 *              After showing several images with delays, WiFi is turned off and
 *              the ESP32 enters deep sleep. Deep sleep restarts the ESP32 on
 *              wake and this sketch does not configure a wake source, so a reset
 *              or power cycle is required to run it again.
 *
 *              Expected output: downloaded images rendered full-screen
 *              (212x104), each followed by a full refresh, and WiFi connection
 *              progress plus a "Going to sleep.." message in the Serial Monitor.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 2
 * - Hardware:   Inkplate 2, USB cable
 * - Extra:      WiFi connection + Internet access
 * - Serial:     115200 baud (optional, for connection logs)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate2"
 *    from Tools -> Board.
 * 2) Enter your WiFi SSID and password (ssid, password) and update the image
 *    URLs if you want to load your own images.
 * 3) Upload the sketch to Inkplate 2.
 * 4) Open Serial Monitor at 115200 baud to watch WiFi connection status.
 * 5) The sketch connects to WiFi, downloads and displays multiple images,
 *    then turns WiFi off and enters deep sleep.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/2/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2022-03-28
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Soldered Inkplate2 in the boards menu."
#endif

#include "HTTPClient.h" // Include library for HTTPClient
#include "Inkplate.h"   // Include Inkplate library to the sketch
#include "WiFi.h"       // Include library for WiFi
Inkplate display;       // Create an object on Inkplate library and also set library into 1 Bit mode (BW)

const char ssid[] = "";     // Your WiFi SSID
const char password[] = ""; // Your WiFi password

void setup()
{
    Serial.begin(115200);   // Init Serial communication.
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display

    // Set settings for error printing
    display.setCursor(10, 10);
    display.setTextSize(2);
    display.setTextColor(INKPLATE2_BLACK);

    // Connect to the WiFi network.
    WiFi.mode(WIFI_MODE_STA);
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(1000);
    }
    Serial.println("Connected!");

    // Draw the first image from web.
    // Make sure the link is complete and correct (contains https:// or http://).
    // The example image is a monochromatic bitmap with 1 bit depth. Images like this load quickest.
    // NOTE: Both drawImage methods allow for an optional fifth "invert" parameter. Setting this parameter to true
    // will flip all colors on the image, making black white and white black. This may be necessary when exporting
    // bitmaps from certain softwares.
    // Fourth parameter will dither the image, but this image is already dithered
    // so it is not needed to dither it again while drawing.
    display.clearDisplay();

    if (!display.image.draw("https://raw.githubusercontent.com/SolderedElectronics/Inkplate-Arduino-library/"
                           "master/examples/Inkplate2/Advanced/WEB_WiFi/"
                           "Inkplate2_Show_Pictures_From_Web/cat_dithered.jpg",
                           0, 0, false, false))
    {
        // If is something failed (wrong url or unsupported format), write error message on the screen.
        // REMEMBER! You can only use Windows Bitmap file with color depth of 1, 4, 8 or 24 bits with no compression!
        display.println("Image open error");
    }
    display.display(); // Refresh the display
    delay(8000);       // Wait a little bit

    // Draw the second image from web, this time using a HTTPClient to fetch the response manually.
    // Full color 24 bit images are large and take a long time to load, will take around 20 secs.
    HTTPClient http;
    // Set parameters to speed up the download process.
    http.getStream().setNoDelay(true);
    http.getStream().setTimeout(1);

    http.begin("https://raw.githubusercontent.com/SolderedElectronics/Inkplate-Arduino-library/"
               "master/examples/Inkplate2/Advanced/WEB_WiFi/"
               "Inkplate2_Show_Pictures_From_Web/car.bmp");

    // Check response code.
    int httpCode = http.GET();
    if (httpCode == 200)
    {
        // Get the response length and make sure it is not 0.
        int32_t len = http.getSize();
        if (len > 0)
        {
            if (!display.image.drawBitmapFromWeb(http.getStreamPtr(), 0, 0, len, true, false))
            {
                // If is something failed (wrong filename or wrong bitmap format), write error message on the screen.
                // REMEMBER! You can only use Windows Bitmap file with color depth of 1, 4, 8 or 24 bits with no
                // compression!
                display.println("Image open error");
            }
            display.display(); // Refresh the display.
        }
        else
        {
            // If something goes wrong, print out the error message and refresh the display.
            display.println("Invalid response length");
            display.display();
        }
    }
    else
    {
        // Print out the error message and refresh the display.
        display.println("HTTP error");
        display.display();
    }

    display.clearDisplay(); // Clear the frame buffer
    delay(8000);            // Wait a little bit

    // Try to load image and display it on e-paper at position X=0, Y=0
    // NOTE: Both drawJpegFromWeb methods allow for an optional fifth "invert" parameter. Setting this parameter to
    // true will flip all colors on the image, making black white and white black. fourth parameter will dither the
    // image.
    if (!display.image.draw("https://raw.githubusercontent.com/SolderedElectronics/Inkplate-Arduino-library/"
                           "master/examples/Inkplate2/Advanced/WEB_WiFi/"
                           "Inkplate2_Show_Pictures_From_Web/mountain.png",
                           0, 0, true, false))
    {
        // If is something failed (wrong filename or format), write error message on the screen.
        display.clearDisplay();
        display.println("Image open error");
    }
    display.display(); // Refresh the display
    http.end();        // Close HTTP connection.

    WiFi.mode(WIFI_OFF); // Turn off the WiFi

    // Go to deep sleep
    Serial.println("Going to sleep..");
    esp_deep_sleep_start();       // Put ESP32 into deep sleep. Program stops here
}

void loop()
{
    // Nothing...
}

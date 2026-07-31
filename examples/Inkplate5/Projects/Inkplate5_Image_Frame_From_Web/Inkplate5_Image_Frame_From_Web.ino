/**
 **************************************************
 * @file        Inkplate5_Image_Frame_From_Web.ino
 * @brief       Web image frame example using Unsplash random images (Inkplate 5).
 *
 * @details     Demonstrates how to use Inkplate 5 as a simple "image frame"
 *              by downloading an image from the web, rendering it on the
 *              e-paper display, and then entering deep sleep to save power.
 *              The sketch requests a random image (1200x825) from Unsplash,
 *              extracts the final redirected image URL, and draws it on the
 *              Inkplate in 3-bit (grayscale) mode (INKPLATE_3BIT).
 *
 *              Deep sleep restarts the program on every wake-up. The Unsplash
 *              "random" endpoint returns a redirect, so the sketch extracts the
 *              final image URL before downloading and rendering. The sleep
 *              interval in this sketch is set to 15 minutes. Expected output is
 *              a randomly selected image on the Inkplate screen, with the device
 *              sleeping after drawing to reduce power consumption.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 5
 * - Hardware:   Inkplate 5, USB cable (or battery for low-power testing)
 * - Extra:      Stable WiFi Internet connection
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate5"
 *    from Tools -> Board.
 * 2) Enter your WiFi SSID and password (ssid, password) in the sketch.
 * 3) Upload the sketch to Inkplate 5.
 * 4) The board connects to WiFi, fetches a random image URL, and displays it.
 * 5) The device enters deep sleep and wakes periodically to refresh the image.
 *
 * @note        Quick start guide: Inkplate 5 has no dedicated page yet,
 *              see https://docs.soldered.com/inkplate/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2020-07-28
 * @license     GNU GPL V3
 **************************************************/


// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE5
#error "Wrong board selection for this example, please select Soldered Inkplate5 in the boards menu."
#endif

// Include Inkplate library in the sketch
#include "Inkplate.h"

// Create an object on Inkplate library and also set library into 3 Bit (grayscale) mode
Inkplate display(INKPLATE_3BIT);

// ---------------- CHANGE HERE ---------------------:

// WiFi credentials
const char *ssid = ""; // Your WiFi SSID
const char *pass = ""; // Your WiFi password

// Define delay between 2 images in seconds
#define SECS_BETWEEN_IMAGES 30

// Here, enter the topic or topics of the kind of images you want to be displayed
// If you want to enter 2 topics, separate them with a comma without whitespace
char *topic = "animals,city";

// ---------------------------------------------------

void setup()
{
    // Init serial communication
    Serial.begin(115200);

    // Init Inkplate library (you should call this function ONLY ONCE)
    display.begin();

    // Join wifi
    display.connectWiFi(ssid, pass);
    Serial.println("Connected");
    char url[256];
    imageUrl(url);

    // Draw an image on the screen
    Serial.println(display.image.drawJpegFromWeb(url, 0, 0, true, false));
    display.display();

    Serial.println("Going to sleep");

    // Activate wakeup timer
    esp_sleep_enable_timer_wakeup(15ll * 60 * 1000 * 1000);

    // Start deep sleep (this function does not return). Program stops here.
    esp_deep_sleep_start();}

void loop()
{
    // Never here! If you are using deep sleep, the whole program should be in setup() because the board restarts each
    // time. loop() must be empty!
}

// Get random image from web
void imageUrl(char *out)
{
    const char *startUrl = "http://loremflickr.com/1200/825";

    // For HTTPS targets (redirect), we’ll use a secure client.
    WiFiClientSecure secure;
    secure.setInsecure();           // quick test; later replace with proper CA

    HTTPClient http;

    const char* keys[] = {"Location"};
    http.collectHeaders(keys, 1);
    http.setFollowRedirects(HTTPC_DISABLE_FOLLOW_REDIRECTS);
    http.setTimeout(15000);
    http.setUserAgent("Mozilla/5.0");   // some CDNs are picky

    // Use plain begin for the initial http:// request
    if (!http.begin(startUrl)) {
        strcpy(out, startUrl);
        return;
    }

    int code = http.GET();
    Serial.printf("HTTP code: %d\n", code);
    if (code < 0) {
        Serial.printf("HTTP error: %s\n", http.errorToString(code).c_str());
        strcpy(out, startUrl);
        http.end();
        return;
    }

    String loc = http.header("Location");
    if (loc.length() == 0) loc = http.getLocation();
    http.end();

    if (loc.length() == 0) {
        strcpy(out, startUrl);
        return;
    }

    if (loc.startsWith("/"))  loc = String("http://loremflickr.com") + loc;
    if (loc.startsWith("//")) loc = String("http:") + loc;

    // If redirected URL is https://, sanity check we can connect to it
    if (loc.startsWith("https://")) {
        HTTPClient https;
        https.setTimeout(15000);
        https.setUserAgent("Mozilla/5.0");
        if (!https.begin(secure, loc)) {
            Serial.println("https begin failed");
        } else {
            int c2 = https.GET();
            Serial.printf("Redirect HTTPS test code: %d (%s)\n", c2, https.errorToString(c2).c_str());
            https.end();
        }
    }

    strncpy(out, loc.c_str(), 255);
    out[255] = 0;
}
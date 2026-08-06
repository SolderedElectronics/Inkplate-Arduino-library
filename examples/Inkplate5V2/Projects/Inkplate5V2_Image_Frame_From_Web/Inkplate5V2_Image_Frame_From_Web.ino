/**
 **************************************************
 * @file        Inkplate5V2_Image_Frame_From_Web.ino
 * @brief       Web image frame example using Unsplash random images (Inkplate 5v2).
 *
 * @details     Demonstrates how to use Inkplate 5v2 as a simple "image frame"
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
 * - Board:      Soldered Inkplate 5v2
 * - Hardware:   Inkplate 5v2, USB cable (or battery for low-power testing)
 * - Extra:      Stable WiFi Internet connection
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate5v2"
 *    from Tools -> Board.
 * 2) Enter your WiFi SSID and password (ssid, password) in the sketch.
 * 3) Upload the sketch to Inkplate 5v2.
 * 4) The board connects to WiFi, fetches a random image URL, and displays it.
 * 5) The device enters deep sleep and wakes periodically to refresh the image.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/5v2/quick-start-guide/
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
#ifndef ARDUINO_INKPLATE5V2
#error "Wrong board selection for this example, please select Soldered Inkplate5 V2 in the boards menu."
#endif

#include "Inkplate.h"
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

Inkplate display(INKPLATE_3BIT);



const char ssid[] = "your ssid";    // Your WiFi SSID
const char *password = "your password"; // Your WiFi password

void setup()
{
    Serial.begin(115200);
    display.begin();
    display.setTextColor(0);

    // Join wifi
    display.connectWiFi(ssid, password);
    Serial.println("joined wifi");
    char url[256];  // Buffer that will hold the final resolved image url

    // Get the final image URL (resolve HTTP redirect) 
    imageUrl(url);
    Serial.println(url);

    // Download and draw the JPEG image directly from the web
    // Function returns 1 on success and 0 on failure
    Serial.println(display.image.drawJpegFromWeb(url, 0, 0, true, false));
    display.display();

    Serial.println("Going to sleep");

    // Activate wakeup timer
    esp_sleep_enable_timer_wakeup(15ll * 60 * 1000 * 1000);

    // Start deep sleep (this function does not return). Program stops here.
    esp_deep_sleep_start();
}

void loop()
{
    // Never here, as deepsleep restarts esp32
}

void imageUrl(char *out)
{
    const char *startUrl = "http://loremflickr.com/960/540";

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

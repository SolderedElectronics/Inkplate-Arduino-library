/**
 **************************************************
 * @file        Inkplate4TEMPERA_Image_Frame_From_Web.ino
 * @brief       Download a JPEG image from the web over WiFi, render it in
 *              3-bit grayscale, then enter deep sleep for periodic refresh.
 *
 * @details     This example connects Inkplate 4 TEMPERA to WiFi, resolves an
 *              HTTP redirect to obtain the final image URL, downloads a JPEG
 *              image from the internet, and draws it directly to the e-paper
 *              display using the Inkplate image helper.
 *
 *              The display is configured in 3-bit grayscale mode (INKPLATE_3BIT),
 *              which supports 8 intensity levels (0-7). After displaying the
 *              downloaded image, the ESP32 sets a wake-up timer and enters deep
 *              sleep. When the timer expires, the ESP32 restarts from setup(),
 *              causing the image to be downloaded and displayed again - loop()
 *              never runs after esp_deep_sleep_start().
 *
 *              Partial update is not available in grayscale mode, so this example
 *              uses a full refresh via display.display(). HTTPS warning:
 *              secure.setInsecure() disables certificate validation and is for
 *              demo/testing only - for production use, validate TLS properly
 *              (e.g. a CA certificate or certificate pinning that matches the
 *              host). Web images and decoding can be RAM-intensive, so large
 *              JPEGs or complex images may fail to decode depending on available
 *              memory. Network endpoints can change behavior (redirects,
 *              user-agent filtering, rate limits); if downloads fail, check the
 *              Serial log and try a different image source.
 *
 *              Expected output: a 600x600 JPEG image rendered on the display in
 *              3-bit grayscale, with WiFi join status, resolved URL, HTTP status
 *              codes and the draw result on Serial.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 4 TEMPERA
 * - Hardware:   Inkplate 4 TEMPERA, USB-C cable
 * - Extra:      WiFi (2.4 GHz), internet access
 * - Serial:     115200 baud (recommended for debugging)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 4 TEMPERA"
 *    from Tools -> Board.
 * 2) Enter your WiFi SSID and password (ssid, password) in the sketch.
 * 3) Upload the sketch and open the Serial Monitor at 115200 baud.
 * 4) After connecting, the sketch resolves the redirect URL and downloads a
 *    JPEG image, then renders it full-screen.
 * 5) The device enters deep sleep and wakes periodically to refresh the image.
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

// Include Inkplate library in the sketch
#include "Inkplate.h"

// Create an object on Inkplate library
Inkplate display(INKPLATE_3BIT);

// ---------------- CHANGE HERE ---------------------:

const char ssid[] = "";    // Your WiFi SSID
const char *password = ""; // Your WiFi password

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
    const char *startUrl = "http://loremflickr.com/600/600";

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

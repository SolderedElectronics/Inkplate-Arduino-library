/**
 **************************************************
 * @file        Inkplate2_Image_Frame_From_Web.ino
 * @brief       Random web image frame: fetch a random image URL online, draw it
 *              on Inkplate 2, then deep sleep and repeat periodically.
 *
 * @details     This example demonstrates a simple "image frame" workflow on
 *              Inkplate 2. On each boot, the ESP32 connects to WiFi, performs a
 *              GET request to a random image endpoint (Unsplash), parses the
 *              returned HTML to extract the actual image URL, then requests and
 *              renders the image to the e-paper display.
 *
 *              The requested image is constrained to Inkplate 2 resolution
 *              (212x104) and is drawn as a PNG. After the image is shown with a
 *              full refresh, the device enters deep sleep for
 *              SECS_BETWEEN_IMAGES seconds and then wakes to fetch a new random
 *              image. Because deep sleep resets the ESP32, the program always
 *              restarts from setup() on every wake cycle, so keep all logic there
 *              and leave loop() empty.
 *
 *              Web/API behavior can change: this example parses an HTML response
 *              to find an href link, so if the provider changes the response
 *              format, URL extraction may fail and require updating the parser.
 *              Network image decoding and RAM usage depend on image format and
 *              size, so stick to 212x104 sources to reduce memory pressure and
 *              improve speed.
 *
 *              Expected output: a randomly selected image rendered full-screen
 *              (212x104) on the display, the resolved image URL and drawImage()
 *              result code in the Serial Monitor, and "HTTP error" printed on the
 *              display on error.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 2
 * - Hardware:   Inkplate 2, USB cable
 * - Extra:      WiFi connection + Internet access
 * - Serial:     115200 baud (recommended for debugging)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate2"
 *    from Tools -> Board.
 * 2) Enter your WiFi SSID and password (ssid, pass) and set
 *    SECS_BETWEEN_IMAGES (seconds).
 * 3) Upload the sketch to Inkplate 2.
 * 4) On boot, the device downloads a random 212x104 image and displays it.
 * 5) The device enters deep sleep and repeats after SECS_BETWEEN_IMAGES seconds.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/2/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2023-05-23
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Soldered Inkplate2 in the boards menu."
#endif

// Include Inkplate library in the sketch
#include "Inkplate.h"

// Create an object on Inkplate library
Inkplate display;

// ---------------- CHANGE HERE ---------------------:

// WiFi credentials
const char *ssid = ""; // Your WiFi SSID
const char *password = ""; // Your WiFi password

// Define delay between 2 images in seconds
#define SECS_BETWEEN_IMAGES 30

// ---------------------------------------------------

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
    const char *startUrl = "http://loremflickr.com/212/104";

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

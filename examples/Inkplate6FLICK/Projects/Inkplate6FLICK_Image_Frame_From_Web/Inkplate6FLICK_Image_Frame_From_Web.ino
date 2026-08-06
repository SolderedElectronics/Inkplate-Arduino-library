/**
 **************************************************
 * @file        Inkplate6FLICK_Image_Frame_From_Web.ino
 * @brief       Download a JPEG image from the web (following an HTTP redirect),
 *              display it in 3-bit grayscale, then deep-sleep between refreshes.
 *
 * @details     This example turns Inkplate 6FLICK into a simple "web image
 *              frame". It connects to WiFi, resolves an HTTP redirect from a
 *              starting URL, then downloads and renders the final JPEG directly
 *              from the internet using drawJpegFromWeb().
 *
 *              The sketch uses manual redirect handling: it performs an HTTP GET
 *              to the start URL with redirects disabled, reads the Location
 *              header, normalizes relative / protocol-relative redirects, and
 *              then uses the resolved URL for image download.
 *
 *              The display runs in 3-bit grayscale mode (INKPLATE_3BIT), which
 *              is slower and uses more energy than 1-bit BW. After drawing the
 *              image and performing a full refresh, the ESP32 enters deep sleep
 *              for 15 minutes; deep sleep restarts the ESP32 on wake, so setup()
 *              reruns and a new image is fetched each cycle.
 *
 *              This example uses plain HTTP for the redirect source - for HTTPS
 *              endpoints, certificate handling may be required depending on the
 *              server and library configuration. Web image decoding and buffering
 *              can be memory-intensive, so very large images or uncommon JPEG
 *              encodings may fail due to RAM limits.
 *
 *              Expected output: a downloaded image rendered on the e-paper
 *              display (grayscale), with the WiFi join message, resolved URL,
 *              HTTP status code and the return value of drawJpegFromWeb()
 *              (1 = success, 0 = failure) on the Serial Monitor.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6FLICK
 * - Hardware:   Inkplate 6FLICK, USB cable
 * - Extra:      WiFi access (internet connection required)
 * - Serial:     115200 baud (optional; used for debug logs)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate6FLICK"
 *    from Tools -> Board.
 * 2) Enter your WiFi SSID/password in the sketch.
 * 3) Upload the sketch to Inkplate 6FLICK.
 * 4) After boot, the device connects to WiFi, resolves the redirect URL, then
 *    downloads and displays a JPEG image.
 * 5) The device deep-sleeps for 15 minutes and repeats after waking.
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

const char ssid[] = "";    // Your WiFi SSID
const char *password = ""; // Your WiFi password

Inkplate display(INKPLATE_3BIT);

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
    HTTPClient http;

    // Starting URL that always redirects to a random image
    const char *startUrl = "http://loremflickr.com/1024/750";

    // Tell HTTPClient to store the "Location" header sowe can read it later
    const char* keys[] = {"Location"};
    http.collectHeaders(keys, 1);

    // Disable automatic redirect following, because we want to manually read the Location header
    http.setFollowRedirects(HTTPC_DISABLE_FOLLOW_REDIRECTS);

    // Start HTTP connection
    if (!http.begin(startUrl))
    {
        strcpy(out, startUrl);
        return;
    }

    // Perform HTTP GET request
    int code = http.GET();
    Serial.printf("HTTP code: %d\n", code);

    // Try header() first (works when collectHeaders is used)
    String loc = http.header("Location");

    // Fallback to getLocation() (sometimes works even if header() is empty)
    if (loc.length() == 0)
        loc = http.getLocation();

    Serial.print("Location: ");
    Serial.println(loc);

    if (loc.length() == 0)
    {
        strcpy(out, startUrl);
        http.end();
        return;
    }

    // If the path is relative, make it absolute
    if (loc.startsWith("/"))
        loc = String("http://loremflickr.com") + loc;

    // If the redirect is protocol-relative (redirect starts with "//"), add "http:"
    if (loc.startsWith("//"))
        loc = String("http:") + loc;

    strncpy(out, loc.c_str(), 255);
    out[255] = 0;

    http.end();
}
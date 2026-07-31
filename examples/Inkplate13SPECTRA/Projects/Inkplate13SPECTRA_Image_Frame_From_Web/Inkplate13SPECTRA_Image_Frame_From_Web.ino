/*
    Inkplate13_Image_Frame_From_Web example for Soldered Inkplate 13
    For this example you will need only USB cable and Inkplate 13.
    Select "Soldered Inkplate 13" from Tools -> Board menu.
    Don't have "Soldered Inkplate 13" option? Follow our tutorial and add it:
    https://docs.soldered.com/inkplate/13spectra/quick-start-guide/

    This example shows how you can set inkplate to show random pictures from web.

    Want to learn more about Inkplate? Visit https://docs.soldered.com/inkplate/
    Looking to get support? Write on our community forum: https://community.soldered.com/
    26 January 2026 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE13SPECTRA
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
#endif

#include "Inkplate.h"

// Create Inkplate display object
Inkplate display;

//WiFi credentials
const char *ssid = "";     // Your WiFi SSID
const char *password = ""; // Your WiFi password


void setup()
{
    Serial.begin(115200);
    display.begin();
    display.setTextColor(INKPLATE_BLACK);

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
    // Never here! If you are using deep sleep, the whole program should be in setup() because the board restarts each
    // time. loop() must be empty!
}

//Function that handles link redirection and final link parsing
void imageUrl(char *out)
{
    HTTPClient http;

    // Starting URL that always redirects to a random image
    const char *startUrl = "http://loremflickr.com/1600/1200";

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
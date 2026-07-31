/*
  Inkplate13SPECTRA_Show_JPG_With_HTTPClient example for Soldered Inkplate 13SPECTRA
  For this example you will need a USB-C cable, Inkplate 13SPECTRA, and an available WiFi connection.
  Select "Soldered Inkplate 13SPECTRA" from Tools -> Board menu.
  Don't have "Soldered Inkplate 13SPECTRA" option? Follow our tutorial and add it:

  https://docs.soldered.com/inkplate/13spectra/quick-start-guide/

  This example will show you how to display JPG image using HTTPClient.
  Make sure that you entered WiFi credentials and change the image link if you want any other image.

  Want to learn more about Inkplate? Visit https://docs.soldered.com/inkplate/
  21 January 2026 by Soldered Electronics
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE13SPECTRA
#error "Wrong board selection for this example, please select Soldered Inkplate 13SPECTRA in the boards menu."
#endif

// Include needed libraries in the sketch
#include "HTTPClient.h"
#include "Inkplate.h"
#include "WiFi.h"

// Create an object on Inkplate library
Inkplate inkplate;

/**************** CHANGE HERE ******************/

char *ssid = ""; // Your WiFi SSID
char *pass = ""; // Your WiFi password

// Add the URL of the image you want to show on Inkplate
String url = "https://raw.githubusercontent.com/SolderedElectronics/Inkplate-Arduino-library/master/examples/Inkplate10/Advanced/WEB_WiFi/Inkplate10_Show_JPG_With_HTTPClient/image.jpg";

/***********************************************/

void setup()
{
    Serial.begin(115200);   // Init serial communication
    inkplate.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    inkplate.clearDisplay(); // Clear frame buffer of display

    // Let's connect to the WiFi
    // You can see the progress on the Serial Monitor
    Serial.print("Connecting to WiFi");

    // Actually connect to the WiFi network
    WiFi.mode(WIFI_MODE_STA);
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED)
    {
        // Print a dot every half second when connecting
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWiFi OK! Downloading...");

    // Make an object for the HTTP client
    HTTPClient http;
    http.begin(url);

    // Do a get request to get the image
    int httpCode = http.GET();

    // If everything is OK
    if (httpCode == HTTP_CODE_OK)
    {
        // Get the size of the image
        int32_t size = http.getSize();
        int32_t len = size; // Copy whose value we will change, but the original must not be lost

        if (size > 0)
        {
            // Allocate the memory for the image
            uint8_t *buffer = (uint8_t *)ps_malloc(size);
            uint8_t *buffPtr = buffer; // Copy of the buffer pointer so that the original one is not lost

            // Temporary buffer for retrieving parts of the image and storing them in the real buffer
            uint8_t buff[512] = {0};

            // Let's fetch the data
            WiFiClient *stream = http.getStreamPtr(); // We need a stream pointer to know how much data is available

            // Repeat as long as we have a connection and while there is data to read
            while (http.connected() && (len > 0 || len == -1))
            {
                // Get the number of available bytes
                size_t size = stream->available();

                // If there are available bytes, read them
                if (size)
                {
                    // Read available bytes from the stream and store them in the buffer
                    int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
                    memcpy(buffPtr, buff, c);

                    // As we read the data, we subtract the length we read and the remaining length is in the variable
                    // len
                    if (len > 0)
                        len -= c;

                    // Likewise for the buffer pointer
                    buffPtr += c;
                }
                else if (len == -1)
                {
                    len = 0;
                }
            }

            // Draw image into the frame buffer of Inkplate
            inkplate.image.drawJpegFromBuffer(buffer, size, 0, 0, true, false);

            // Free the memory where the image was stored because it is now in the frame buffer
            free(buffer);
        }
        else
        {
          // Show an error message
          inkplate.setCursor(0, 0);
          inkplate.println("Invalid response length " + String(size) + " (HTTP " + String(httpCode) + ")");
        }
    }
    else
    {
      // Show an error message
      inkplate.setCursor(0, 0);
      inkplate.println("HTTP error" + String(httpCode) + "...");
    }

    // Draw image on the screen
    inkplate.display();
}

void loop()
{
  // Nothing
}
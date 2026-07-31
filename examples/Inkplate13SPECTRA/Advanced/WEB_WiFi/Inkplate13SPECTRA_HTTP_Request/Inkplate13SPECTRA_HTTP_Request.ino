/**
 **************************************************
 * @file        Inkplate13SPECTRA_HTTP_Request.ino
 * @brief       Simple HTTP web content fetch example for Soldered Inkplate 13SPECTRA.
 *
 * @details     Shows how to connect to a WiFi network, get data from the
 *              Internet and display that data on the e-paper display. This
 *              example is NOT about parsing HTML data from the Internet - it
 *              will just print the HTML on the screen.
 *
 *              In the quotation marks in the sketch you need to write your WiFi
 *              SSID and WiFi password in order to connect to your WiFi network.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 13SPECTRA
 * - Hardware:   Inkplate 13SPECTRA, USB cable
 * - Extra:      Stable WiFi Internet connection
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 13SPECTRA"
 *    from Tools -> Board.
 * 2) Enter your WiFi SSID and password in the sketch.
 * 3) Upload the sketch to Inkplate 13SPECTRA.
 * 4) The fetched HTML content is printed on the e-paper display.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/13spectra/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered Electronics
 * @date        2026-01-21
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE13SPECTRA
#error "Wrong board selection for this example, please select Soldered Inkplate 13SPECTRA in the boards menu."
#endif

#include "Inkplate.h"   //Include Inkplate library to the sketch
#include <HTTPClient.h> //Include HTTP library to this sketch
#include <WiFi.h>       //Include ESP32 WiFi library to our sketch

#define ssid "" // Name of the WiFi network (SSID) that you want to connect Inkplate to
#define pass "" // Password of that WiFi network

Inkplate inkplate; // Create an object on Inkplate library and also set library into 1 Bit mode (BW)

void setup()
{
  Serial.begin(115200);                            // Begin Serial for debugging
  inkplate.begin();                                 // Init Inkplate library (you should call this function ONLY ONCE)
  inkplate.clearDisplay();                          // Clear frame buffer of display
  inkplate.display();                               // Put clear image on display
  inkplate.setTextSize(4);                          // Set text scaling to four (text will be two times bigger)
  inkplate.setCursor(0, 0);                         // Set print position
  inkplate.setTextColor(INKPLATE_BLACK);            // Set text color to black and background color to white
  Serial.println("Scanning for WiFi networks..."); // Write text

  int n =
      WiFi.scanNetworks(); // Start searching WiFi networks and put the nubmer of found WiFi networks in variable n
  if (n == 0)
  { // If you did not find any network, show the message and stop the program.
    Serial.print("No WiFi networks found!");
    while (true);
  }
  else
  {
    if (n > 10)
        n = 10; // If you did find, print name (SSID), encryption and signal strength of first 10 networks
    for (int i = 0; i < n; i++)
    {
      inkplate.print(WiFi.SSID(i));
      inkplate.print((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? 'O' : '*');
      inkplate.print('\n');
      inkplate.print(WiFi.RSSI(i), DEC);
    }
    inkplate.display();
  }

  inkplate.clearDisplay();         // Clear everything in frame buffer
  inkplate.setCursor(0, 0);        // Set print cursor to new position
  Serial.print("Connecting to "); // Print the name of WiFi network
  Serial.print(ssid);
  WiFi.begin(ssid, pass); // Try to connect to WiFi network
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000); // While it is connecting to network, display dot every second, just to know that Inkplate is
                 // alive.
    Serial.print('.');
  }
  Serial.print("connected"); // If it's connected, notify user

  HTTPClient http;
  if (http.begin("http://example.com/index.html"))
  { // Now try to connect to some web page (in this example www.example.com. And yes, this is a valid Web page :))
    if (http.GET() > 0)
    { // If connection was successful, try to read content of the Web page and display it on screen
      String htmlText;
      htmlText = http.getString();
      inkplate.setTextSize(4);
      inkplate.clearDisplay();
      inkplate.setCursor(0, 0);
      inkplate.print(htmlText);
      inkplate.display();
    }
  }
}

void loop()
{
  // Nothing
}
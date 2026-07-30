/*
   Inkplate13SPECTRA_WiFi_Connect  example for Soldered Inkplate 13SPECTRA
   For this example you will need a micro USB cable, Inkplate 13SPECTRA, and an available WiFi connection.
   Select "Soldered Inkplate 13SPECTRA" from Tools -> Board menu.
   Don't have "Soldered Inkplate 13SPECTRA" option? Follow our tutorial and add it:

   /////////////////////////// INSERT BOARD DEF LINK HERE

   This example will show you how to connect the Inkplate 13SPECTRA to a WiFi network.

   Want to learn more about Inkplate? Visit https://docs.soldered.com/inkplate/
   23 January 2026 by Soldered Electronics
*/

#ifndef ARDUINO_INKPLATE13SPECTRA
#error "Wrong board selection for this example, please select Soldered Inkplate 13SPECTRA in the boards menu."
#endif

#include "Inkplate.h" //Include Inkplate library to the sketch
#include "WiFi.h"     //Include library for WiFi

Inkplate inkplate;
const char ssid[] = "YOUR_SSID_HERE";      // Your WiFi SSID
const char *password = "YOUR_PASSWORD_HERE"; // Your WiFi password

void setup()
{
    inkplate.begin();                      // Init Inkplate library (you should call this function ONLY ONCE)
    inkplate.clearDisplay();               // Clear frame buffer of display
    inkplate.setTextColor(INKPLATE_BLACK); // Set the text color to black
    inkplate.setTextSize(5);               // Set text to be 5 times bigger than classic 5x7 px text
    inkplate.setCursor(80, 500);           // Set position of the text
    WiFi.begin(ssid, password);
    inkplate.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
    }
    inkplate.setCursor(80, 600);
    inkplate.println("Successfully connected to WiFi");
    inkplate.display();
}

void loop()
{
}

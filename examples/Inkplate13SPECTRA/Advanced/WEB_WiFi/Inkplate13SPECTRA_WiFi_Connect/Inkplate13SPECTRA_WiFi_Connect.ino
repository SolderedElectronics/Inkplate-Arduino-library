/**
 **************************************************
 * @file        Inkplate13SPECTRA_WiFi_Connect.ino
 * @brief       WiFi connection example for Soldered Inkplate 13SPECTRA.
 *
 * @details     Shows how to connect the Inkplate 13SPECTRA to a WiFi network.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 13SPECTRA
 * - Hardware:   Inkplate 13SPECTRA, micro USB cable
 * - Extra:      Available WiFi connection
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 13SPECTRA"
 *    from Tools -> Board.
 * 2) Enter your WiFi credentials in the sketch.
 * 3) Upload the sketch to Inkplate 13SPECTRA.
 * 4) The connection status is shown on the e-paper display.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/13spectra/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered Electronics
 * @date        2026-01-23
 **************************************************/

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

/**
 **************************************************
 * @file        Inkplate6FLICK_Qwiic.ino
 * @brief       Qwiic I2C scanner demo for Soldered Inkplate 6FLICK.
 *
 * @details     Demonstrates how to scan the I2C bus on the Inkplate 6FLICK
 *              Qwiic connector and detect connected devices. The example
 *              checks all valid I2C addresses, reports detected devices to
 *              the Serial Monitor, and displays the results on the e-paper
 *              screen. Useful for troubleshooting wiring and verifying
 *              Qwiic/I2C peripherals.
 *
 *              The sketch uses the standard I2C scanning method via
 *              Wire.endTransmission() and shows addresses in hexadecimal
 *              (7-bit I2C addresses), which is handy for debugging sensors,
 *              RTCs, displays and other Qwiic modules. Expected output is a
 *              list of detected I2C addresses in hexadecimal format, or a
 *              message if no devices are detected.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6FLICK
 * - Hardware:   Inkplate 6FLICK, USB cable,
 *               optional Qwiic/I2C device(s) and cable
 * - Libraries:  Inkplate library, Wire (built-in Arduino I2C library)
 * - Serial:     115200 baud
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 6FLICK"
 *    from Tools -> Board.
 * 2) Connect an I2C/Qwiic device to the Inkplate.
 * 3) Upload the sketch.
 * 4) Open Serial Monitor (115200 baud).
 * 5) Detected device addresses appear on the Serial Monitor and on the
 *    Inkplate display.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/6flick/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered Electronics
 * @date        2026-02-26
 * @license     GNU GPL V3
 **************************************************/

// Inkplate 6FLICK - Qwiic I2C Scanner (Serial + Display)
// Open Serial Monitor @ 115200 baud.

#ifndef ARDUINO_INKPLATE6FLICK
#error "Wrong board selection for this example, please select Soldered Inkplate 6FLICK"
#endif

#include <Wire.h>
#include "Inkplate.h"

Inkplate display(INKPLATE_1BIT);

void setup()
{
    Serial.begin(115200);
    delay(200);

    display.begin();
    display.clearDisplay();
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(3);
    display.setCursor(30, 20);
    display.println("Starting I2C scan...");
    display.display();

    // Start I2C (Wire)
    Wire.begin();

    delay(2000);
}

void loop()
{
    // Create some helper variables for making the I2C requests
    byte error, address;
    int nDevices = 0;   // Number of devices

    display.clearDisplay();
 
    // Print a message to the user
    Serial.println("Scanning...");
    display.setCursor(30, 20);
    display.println("Scanning...");

    // Let's loop through all the available addresses
    for (address = 1; address < 127; address++)
    {
        // This sketch uses the return value of endTransmisstion
        // A value of 0 means a device did acknowledge the message
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0)
        {
            // Found a device! 
            // Notify the user
            Serial.print("I2C device found at address 0x");
            display.print("I2C device found at address 0x");
            // Print the hex address
            if (address < 16)
            {
                Serial.print("0");
                display.print("0");
            }
                
            Serial.print(address, HEX);
            Serial.println("  !");
            display.print(address, HEX);
            display.println("  !");
            nDevices++; // count the device
        }
        else if (error == 4)
        {
            // In case there was an error, also notify the user
            Serial.print("Unknown error at address 0x");
            display.print("Unknown error at address 0x");
            if (address < 16)
            {
                Serial.print("0");
                display.print("0");
            }
                
            Serial.println(address, HEX);
            display.print(address, HEX);
        }
    }

    // We just finished checking all possible I2C addresses
    if (nDevices == 0)
    {
        // No devices found? Notify the user
        Serial.println("No I2C devices found\n");
        display.println("No I2C devices found\n");
    }
    else
    {
        Serial.println("done\n");
        display.println("done\n");
    }
    display.display();
    delay(5000); // Wait 5 seconds before the next scan
}

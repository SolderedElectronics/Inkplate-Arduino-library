/**
 **************************************************
 * @file        Inkplate10_Qwiic.ino
 * @brief       I2C (Qwiic) scanner example for Soldered Inkplate 10.
 *
 * @details     Scans the I2C bus for connected Qwiic/I2C devices and displays
 *              detected device addresses both on the Serial Monitor and on
 *              the Inkplate 10 e-paper display. Useful for validating proper
 *              wiring and confirming device communication.
 *
 *              Valid I2C addresses range from 0x01 to 0x7E and the scan repeats
 *              every 5 seconds. Expected output is a list of detected I2C
 *              device addresses on the Inkplate display, with scanning progress
 *              and addresses logged in the Serial Monitor.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 10
 * - Hardware:   Inkplate 10, USB cable, optional Qwiic/I2C device
 * - Extra:      None
 * - Serial:     115200 baud
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate10"
 *    from Tools -> Board.
 * 2) Connect a Qwiic/I2C device to the Inkplate.
 * 3) Upload the sketch to Inkplate 10.
 * 4) Open the Serial Monitor (115200 baud).
 * 5) Detected I2C addresses will be shown on both the display and Serial.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/10/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2026-02-23
 * @license     GNU GPL V3
 **************************************************/

#include "Inkplate.h"
#include "Wire.h"

// Create Inkplate object
Inkplate display(INKPLATE_1BIT);

void setup()
{
    // Initialize Inkplate
    display.begin();
    display.clearDisplay();
    display.setTextSize(2);
    display.setTextColor(BLACK);
    display.setCursor(0, 0);
    display.println("Inkplate I2C Scanner");
    display.display();
    display.expander1.digitalWrite(3,HIGH, true);
    display.expander1.digitalWrite(4,HIGH, true);

    // Initialize I2C and Serial
    //Wire.begin();
    Serial.begin(115200);

    Serial.println("\nSoldered Inkplate I2C Scanner!");
}

void loop()
{
    byte error, address;
    int nDevices = 0;
    int yCursor = 30; // Vertical position for listing devices

    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(2);
    display.println("Scanning I2C...");
    display.setTextSize(1);

    Serial.println("Scanning...");

    for (address = 1; address < 127; address++)
    {
        Wire.beginTransmission(address);
        error = Wire.endTransmission();

        if (error == 0)
        {
            Serial.print("I2C device found at address 0x");
            if (address < 16)
                Serial.print("0");
            Serial.print(address, HEX);
            Serial.println(" !");

            display.setCursor(0, yCursor);
            display.print("Found: 0x");
            if (address < 16)
                display.print("0");
            display.println(address, HEX);

            yCursor += 12;
            nDevices++;
        }
        else if (error == 4)
        {
            Serial.print("Unknown error at address 0x");
            if (address < 16)
                Serial.print("0");
            Serial.println(address, HEX);
        }
    }

    if (nDevices == 0)
    {
        Serial.println("No I2C devices found\n");
        display.setCursor(0, yCursor);
        display.println("No devices found.");
    }
    else
    {
        Serial.println("Done.\n");
    }

    display.display();

    delay(5000);
}
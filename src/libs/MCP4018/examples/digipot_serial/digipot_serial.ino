/**
 **************************************************
 *
 * @file        digipot_serial.ino
 * @brief       This example showe hot to increment or decrement digipot value from Arduino Serial Monitor.
 *
 * Product: Digipot 5k      - solde.red/333046
 *          Digipit 10k     - solde.red/333091
 *          Digipot 50k     - solde.red/333092
 *          Digipot 100k    - solde.red/333093
 *
 * @authors     Borna Biro @ soldered.com
 ***************************************************/

#include "MCP4018-SOLDERED.h" // Include Soldered library for MCP4018 Digipot.

MCP4018_SOLDERED digipot; // Create object for Digipot library.

void setup()
{
    Serial.begin(115200); // Initialize serial communication at 115200 bauds.
    delay(100);           // Wait a little bit for serial.
    Serial.println("Send + to increment wiper position, - to decrement."); // Send info message to serial.
    digipot.begin();                                                       // Initialize Digipot library.
}

void loop()
{
    // Check if there is any new symbol on Serial, if it is, read it!
    if (Serial.available())
    {
        char c = Serial.read();

        // If you received "+" symbol, increment wiper position with respect to GND.
        if (c == '+')
        {
            digipot.increment();
            printDigipotData(digipot.getWiperValue());
        }

        // If you received "-" symbol, decrement wiper position with respect to GND.
        if (c == '-')
        {
            digipot.decrement();
            printDigipotData(digipot.getWiperValue());
        }
    }
}

// Function prints wiper position on Serial Monitor.
void printDigipotData(int _v)
{
    Serial.print("Digipot wiper: ");
    Serial.print(_v, DEC);
    Serial.println("/127");
}
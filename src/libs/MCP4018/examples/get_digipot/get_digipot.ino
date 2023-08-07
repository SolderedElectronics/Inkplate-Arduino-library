/**
 **************************************************
 *
 * @file        get_digipot.ino
 * @brief       This example shows how to set the desired value to digipot and read that value from it.
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
    digipot.begin();      // Initialize Digipot library.
}

void loop()
{
    // Set digipot wiper to 0% (connected to GND).
    digipot.setWiperPercent(0);
    printDigipotData(digipot.getWiperValue());
    delay(2000);

    // Set digipot wiper to 25%.
    digipot.setWiperPercent(25);
    printDigipotData(digipot.getWiperValue());
    delay(2000);

    // Set digipot wiper to 50%.
    digipot.setWiperPercent(50);
    printDigipotData(digipot.getWiperValue());
    delay(2000);

    // Set digipot wiper to 75%.
    digipot.setWiperPercent(75);
    printDigipotData(digipot.getWiperValue());
    delay(2000);

    // Set digipot wiper to 100% (Wiper connected to A pad).
    digipot.setWiperPercent(100);
    printDigipotData(digipot.getWiperValue());
    delay(2000);
}

// Function prints wiper position on Serial Monitor.
void printDigipotData(int _v)
{
    Serial.print("Digipot wiper: ");
    Serial.print(_v, DEC);
    Serial.println('%');
}
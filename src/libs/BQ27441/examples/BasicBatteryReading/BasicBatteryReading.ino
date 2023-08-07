/**
 **************************************************
 *
 * @file        BasicBatteryReading.ino
 * @brief       This example is to show how BQ27441-G1 can be used for basic battery readings.
 * 
 *              To successfully run the sketch:
 *              -Connect the battey to the breakout
 *              -Connect the breakout to your Dasduino board with an easyC cable
 *              -Open the Serial monitor at 115200 baud!
 *              
 *              NOTE: It is IMPORTANT to connect the battery because this module gets power from battery
 *              and will not work without it!
 *
 *              Fuel gauge BQ27441 breakout: solde.red/333065
 *              Dasduino Core: www.solde.red/333037
 *              Dasduino Connect: www.solde.red/333034
 *              Dasduino ConnectPlus: www.solde.red/333033
 *
 * @authors     Robert Peric @ soldered.com
 ***************************************************/

// Include Library
#include "BQ27441-G1-SOLDERED.h"

// Create fuel gauge object with the library
BQ27441 battery;

// Set BATTERY_CAPACITY to the design capacity of your battery in mAh.
const unsigned int BATTERY_CAPACITY = 600;

void setup()
{
    // Begin serial communication
    Serial.begin(115200);

    // Use battery.begin() to initialize the BQ27441-G1A and confirm that it's
    // connected and communicating.
    if (!battery.begin()) // begin() will return true if communication is successful
    {
        // If communication fails, print an error message and loop forever.
        Serial.println("Error: Unable to communicate with BQ27441.");
        Serial.println("  Check wiring and try again.");
        while (1)
            delay(1);
    }
    Serial.println("Connected to BQ27441!");

    // Uset battery.setCapacity(BATTERY_CAPACITY) to set the design capacity
    // of your battery.
    battery.setCapacity(BATTERY_CAPACITY);
}

void loop()
{
    // Read battery stats from the BQ27441-G1A
    unsigned int soc = battery.soc();                   // Read state-of-charge (%)
    unsigned int volts = battery.voltage();             // Read battery voltage (mV)
    int current = battery.current(AVG);                 // Read average current (mA)
    unsigned int fullCapacity = battery.capacity(FULL); // Read full capacity (mAh)
    unsigned int capacity = battery.capacity(REMAIN);   // Read remaining capacity (mAh)
    int power = battery.power();                        // Read average power draw (mW)
    int health = battery.soh();                         // Read state-of-health (%)

    // Now print out those values:
    String toPrint = String(soc) + "% | ";
    toPrint += String(volts) + " mV | ";
    toPrint += String(current) + " mA | ";
    toPrint += String(capacity) + " / ";
    toPrint += String(fullCapacity) + " mAh | ";
    toPrint += String(power) + " mW | ";
    toPrint += String(health) + "%";

    Serial.println(toPrint);

    // Wait a bit
    delay(2000);
}

/**
 **************************************************
 *
 * @file        ExtendedBatteryConfiguration.ino
 * @brief       This example is to show how BQ27441-G1 can be used for extended battery configuration.
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

// Include the required library
#include "BQ27441-G1-SOLDERED.h"

// Set BATTERY_CAPACITY to the design capacity of your battery in mAh.
const uint16_t BATTERY_CAPACITY = 600;

// Lowest operational voltage in mV
const uint16_t TERMINATE_VOLTAGE = 3000;

// Current at which charger stops charging battery in mA
const uint16_t TAPER_CURRENT = 60;

void setup()
{
    // Begin serial communication
    Serial.begin(115200);

    if (!lipo.begin()) // begin() will return true if communication is successful
    {
        // If communication fails, print an error message and loop forever.
        Serial.println("Error: Unable to communicate with BQ27441.");
        Serial.println("  Check wiring and try again.");
        Serial.println("  (Battery must be plugged into Fuel Gauge!)");
        while (1)
            ;
    }
    Serial.println("Connected to BQ27441!");

    Serial.println("Writing gague config");

    // To configure the values below, you must be in config mode
    lipo.enterConfig();

    // Set the battery capacity
    lipo.setCapacity(BATTERY_CAPACITY);

    // Taper Rate = Design Capacity / (0.1 * Taper Current)
    lipo.setTaperRate(10 * BATTERY_CAPACITY / TAPER_CURRENT);

    // Exit config mode to save changes
    lipo.exitConfig();
}

void printBatteryStats()
{
    // Read battery stats from the BQ27441-G1A
    unsigned int soc = lipo.soc();                   // Read state-of-charge (%)
    unsigned int volts = lipo.voltage();             // Read battery voltage (mV)
    int current = lipo.current(AVG);                 // Read average current (mA)
    unsigned int fullCapacity = lipo.capacity(FULL); // Read full capacity (mAh)
    unsigned int capacity = lipo.capacity(REMAIN);   // Read remaining capacity (mAh)
    int power = lipo.power();                        // Read average power draw (mW)
    int health = lipo.soh();                         // Read state-of-health (%)

    // Assemble a string to print
    String toPrint = "[" + String(millis() / 1000) + "] ";
    toPrint += String(soc) + "% | ";
    toPrint += String(volts) + " mV | ";
    toPrint += String(current) + " mA | ";
    toPrint += String(capacity) + " / ";
    toPrint += String(fullCapacity) + " mAh | ";
    toPrint += String(power) + " mW | ";
    toPrint += String(health) + "%";

    // Fast charging allowed
    if (lipo.chgFlag())
        toPrint += " CHG";

    // Full charge detected
    if (lipo.fcFlag())
        toPrint += " FC";

    // Battery is discharging
    if (lipo.dsgFlag())
        toPrint += " DSG";

    // Print the string
    Serial.println(toPrint);
}

void loop()
{
    // Every 1 second print battery status
    printBatteryStats();
    delay(1000);
}

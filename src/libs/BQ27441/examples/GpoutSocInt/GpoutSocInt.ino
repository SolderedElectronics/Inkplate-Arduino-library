/**
 **************************************************
 *
 * @file        BQ27441GpoutSocInt.ino
 * @brief       Demonstrates how to use the BQ27441's SOC_INT function on GPOUT. In this mode
 *              GPOUT will pulse every time the state-of-charge (soc) goes up or down by a
 *              set percentage integer.
 *
 *              To successfully run the sketch:
 *              -Connect the battey to the breakout
 *              -Conncect GPOUT to your Dasduino board (change it below, set to pin 2 by default)
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
const unsigned int BATTERY_CAPACITY = 600;

// Arduino pin connected to BQ27441's GPOUT pin
const int GPOUT_PIN = 2;

// Set the integer percentage change that triggers an interrupt
const int PERCENTAGE_INTERVAL = 1;

void setupBQ27441(void)
{
    pinMode(GPOUT_PIN, INPUT_PULLUP); // Set the GPOUT pin as an input w/ pullup

    // Use lipo.begin() to initialize the BQ27441-G1A and confirm that it's
    // connected and communicating.
    if (!lipo.begin()) // begin() will return true if communication is successful
    {
        // If communication fails, print an error message and loop forever.
        Serial.println("Error: Unable to communicate with BQ27441.");
        Serial.println("  Check wiring and try again.");
        Serial.println("  (Battery must be plugged into Fuel gauge!)");
        while (1)
            ;
    }
    Serial.println("Connected to BQ27441!");

    // In this example, we'll manually enter and exit config mode. By controlling
    // config mode manually, you can set the chip up faster -- completing all of
    // the set up in a single config mode sweep.
    lipo.enterConfig();                     // To configure the values below, you must be in config mode
    lipo.setCapacity(BATTERY_CAPACITY);     // Set the battery capacity
    lipo.setGPOUTPolarity(LOW);             // Set GPOUT to active-low
    lipo.setGPOUTFunction(SOC_INT);         // Set GPOUT to SOC_INT mode
    lipo.setSOCIDelta(PERCENTAGE_INTERVAL); // Set percentage change integer
    lipo.exitConfig();                      // Exit config mode to save changes

    // Use lipo.GPOUTPolarity to read from the chip and confirm the changes
    if (lipo.GPOUTPolarity())
        Serial.println("GPOUT set to active-HIGH");
    else
        Serial.println("GPOUT set to active-LOW");

    // Use lipo.GPOUTFunction to confirm the functionality of GPOUT
    if (lipo.GPOUTFunction())
        Serial.println("GPOUT function set to BAT_LOW");
    else
        Serial.println("GPOUT function set to SOC_INT");

    // Read lipo.sociDelta() to confirm the integer change value
    Serial.println("SOCI Delta: " + String(lipo.sociDelta()));
    Serial.println();

    // Use lipo.pulseGPOUT() to trigger a pulse on GPOUT. This only works
    // in SOC_INT mode.
    Serial.println("Testing GPOUT Pulse");
    lipo.pulseGPOUT();
    int timeout = 10000; // The pulse can take a while to occur. Set max to 10s
    while ((digitalRead(GPOUT_PIN)) && timeout--)
        delay(1); // Wait for GPOUT to go high, or timeout to occur
    if (timeout > 0)
    {
        // If GPOUT pulsed, print success message.
        Serial.print("GPOUT test successful!");
        Serial.println("(" + String(10000 - timeout) + ")");
        Serial.print("GPOUT will pulse whenever the SoC ");
        Serial.println("value changes by SOCI delta.");
        Serial.print("Or when the battery changes from");
        Serial.println(" charging to discharging, or vice-versa.");
        Serial.println();
    }
    else
    {
        // If GPOUT didn't pulse, something went wrong. Print error message
        // and loop forever.
        Serial.println("GPOUT didn't pulse.");
        Serial.print("Make sure it's connected to pin ");
        Serial.print(GPOUT_PIN);
        Serial.println(" and reset.");
        while (1)
            ;
    }
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

    // Print the string
    Serial.println(toPrint);
}

void setup()
{
    // Begin serial communication
    Serial.begin(115200);

    // Do configuration defined by the previous functions
    setupBQ27441();
}

void loop()
{
    // The interrupt is set to active-low. If the GPOUT pin goes low...
    if (digitalRead(GPOUT_PIN) == LOW)
    {
        // ...SOC_INT occurred. Print battery stats.
        printBatteryStats();
    }
}

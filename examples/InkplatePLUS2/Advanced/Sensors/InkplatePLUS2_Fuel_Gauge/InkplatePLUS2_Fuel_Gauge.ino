/*
   InkplatePLUS2_Fuel_Gauge example for Soldered Inkplate PLUS2
   For this example you will need a USB-C cable and Inkplate PLUS2 and a Li-Ion battery.
   Select "Soldered Inkplate PLUS2" from Tools -> Board menu.
   Don't have "Soldered Inkplate PLUS2" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example will show you how to read data from the built-in BQ27441 Fuel Gauge,
   the data will also be displayed on the ePaper screen.

   To run this sketch successfully, connect a Li-Ion battery and set the BATTERY_CAPACITY
   to the capacity of your battery in mAh.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   11 Sep 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATEPLUS2
#error "Wrong board selection for this example, please select Inkplate PLUS2 in the boards menu."
#endif

#include "Inkplate.h"    // Include Inkplate library to the sketch
#include "batteryIcon.h" // Include the battery icon

Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1-bit mode (BW)

// Set BATTERY_CAPACITY to the capacity of your battery in mAh.
// If it's the standard one that comes with Inkplate PLUS2, it's 1200 mAH
const unsigned int BATTERY_CAPACITY = 1200;

// Variable that keeps count on how many times the screen has been partially updated
int numRefreshes = 0;

// How many partial updates we want before doing a full refresh
#define NUM_PARTIAL_UPDATES_BEFORE_FULL_REFRESH 15

// For printing, make an array of the descriptions of the values which are being printed
String infoNames[] = {"State of charge (%): ",      "Voltage (mV): ",    "Avg. current (mA): ", "Full capacity (mAh): ",
                      "Remaining capacity (mAh): ", "Power draw (mW): ", "State of Health (%):"};

// Setup code, runs only once
void setup()
{
    display.begin();   // Init Inkplate library (you should call this function ONLY ONCE)
    display.display(); // Put clear image on display

    display.battery.begin(); // Init the fuel gauge
    // Note: You don't actually need to call display.battery.begin() to enable drawing power from the battery
    // Inkplate will do this automatically, this is just if you want to init the fuel gauge
    // The class 'battery' is just named this way for better readability later on, eg. display.battery.voltage();

    // Set the battery capacity for accurate readings
    display.battery.setCapacity(BATTERY_CAPACITY);

    // Set the text size 3 times larger than default (5x7) for printing
    display.setTextSize(3);
}

void loop()
{
    // Read battery stats from the BQ27441-G1A fuel gauge
    int soc = display.battery.soc();                   // Read state-of-charge (%)
    int volts = display.battery.voltage();             // Read battery voltage (mV)
    int current = display.battery.current(AVG);        // Read average current (mA)
    int fullCapacity = display.battery.capacity(FULL); // Read full capacity (mAh)
    int capacity = display.battery.capacity(REMAIN);   // Read remaining capacity (mAh)
    int power = display.battery.power();               // Read average power draw (mW)
    int health = display.battery.soh();                // Read state-of-health (%)

    // Save to array for printing to display
    int dataFromFuelGauge[] = {soc, volts, current, fullCapacity, capacity, power, health};

    // Let's show everything on the display!
    // First, clear what was previously on the display
    display.clearDisplay();

    // Draw the battery icon
    display.drawImage(batteryIcon, 0, 0, 600, 600);
    // Draw the rectrangle inside the battery depending on the state of charge
    // The width of the rectrangle is proportional to the variable
    display.fillRect(195, 425, 202 * (soc / 100.0F), 95, BLACK);

    // Print the data in order
    for (int i = 0; i < 7; i++)
    {
        // Set the cursor position so it's printed line-by-line
        display.setCursor(30, 30 + 45 * i);
        // Print what the data is and then the number
        display.print(infoNames[i]);
        display.print(dataFromFuelGauge[i]);
    }

    // Update the screen
    if (numRefreshes > NUM_PARTIAL_UPDATES_BEFORE_FULL_REFRESH)
    {                      // Check if you need to do full refresh or you can do partial update
        display.display(); // Do a full refresh
        numRefreshes = 0;  // Reset the counter
    }
    else
    {
        display.partialUpdate(false, true); // Do partial update
        numRefreshes++;                     // Keep track on how many times screen has been partially updated
    }

    // Wait a bit
    delay(2000);
}
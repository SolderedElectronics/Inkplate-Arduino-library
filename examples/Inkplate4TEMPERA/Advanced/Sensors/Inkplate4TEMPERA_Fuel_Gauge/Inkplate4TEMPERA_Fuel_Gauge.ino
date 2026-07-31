/**
 **************************************************
 * @file        Inkplate4TEMPERA_Fuel_Gauge.ino
 * @brief       Reads battery statistics from the on-board BQ27441 fuel gauge
 *              and displays them with a simple battery graphic.
 *
 * @details     This example demonstrates how to use the built-in BQ27441-G1A
 *              fuel gauge on Inkplate 4 TEMPERA. After enabling the fuel gauge
 *              peripheral (disabled by default), the sketch initializes the
 *              battery interface, sets the configured battery capacity (mAh),
 *              and then periodically reads key battery parameters:
 *              - State of charge (SoC)
 *              - Voltage
 *              - Average current
 *              - Full and remaining capacity
 *              - Average power draw
 *              - State of health (SoH)
 *
 *              The values are rendered on the e-paper display in 1-bit BW mode
 *              (INKPLATE_1BIT) along with a large battery icon. A filled
 *              rectangle inside the icon represents the current SoC. The display
 *              is updated every 2 seconds; to reduce flicker and speed up
 *              updates, the sketch performs partial updates most of the time and
 *              forces a full refresh after
 *              NUM_PARTIAL_UPDATES_BEFORE_FULL_REFRESH partial updates. Panel
 *              power is kept enabled during partial updates for stability and
 *              speed (higher power usage).
 *
 *              The fuel gauge is disabled by default. Enabling it improves SoC
 *              accuracy but increases deep sleep current by ~30-50 uA. It must be
 *              configured with the correct battery capacity for meaningful
 *              capacity/SoC values.
 *
 *              Expected output: a battery icon with a fill bar proportional to
 *              the state of charge, plus text lines showing SoC (%), voltage
 *              (mV), average current (mA), full and remaining capacity (mAh),
 *              power (mW) and state of health (%).
 *
 * Requirements:
 * - Board:      Soldered Inkplate 4 TEMPERA
 * - Hardware:   Inkplate 4 TEMPERA, USB-C cable, Li-Ion battery
 * - Extra:      none
 * - Serial:     none
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 4 TEMPERA"
 *    from Tools -> Board.
 * 2) Connect a Li-Ion battery to Inkplate 4 TEMPERA.
 * 3) Set BATTERY_CAPACITY to your battery capacity in mAh (e.g. 1200 mAh for
 *    the typical bundled battery).
 * 4) Upload the sketch.
 * 5) The screen updates every ~2 seconds with live fuel gauge readings.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/4tempera/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2023-09-11
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE4TEMPERA
#error "Wrong board selection for this example, please select Inkplate 4 TEMPERA in the boards menu."
#endif

#include "Inkplate.h"    // Include Inkplate library to the sketch
#include "batteryIcon.h" // Include the battery icon

Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1-bit mode (BW)

// Set BATTERY_CAPACITY to the capacity of your battery in mAh.
// If it's the standard one that comes with Inkplate 4TEMPERA, it's 1200 mAH
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

    // Enable the Fuel Gauge.
    // NOTE!
    // Fuel Gauge is disabled by default. That will impact accuracy of the SoC (State of Charge). You can enable it,
    // but it will have an impact on the deep sleep current (it will add additional 30-50uA).
    display.wakePeripheral(INKPLATE_FUEL_GAUGE); 

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
    display.image.draw(batteryIcon, 0, 0, 600, 600);
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
        numRefreshes = 0;  // reset the counter
    }
    else
    {
        display.partialUpdate(false, true); // Do partial update
        numRefreshes++;                     // Keep track on how many times screen has been partially updated
    }

    // Wait a bit
    delay(2000);
}
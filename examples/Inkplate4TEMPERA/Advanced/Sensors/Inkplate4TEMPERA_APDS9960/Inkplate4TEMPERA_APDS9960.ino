/*
   Inkplate4TEMPERA_APDS9960 example for Soldered Inkplate 4 TEMPERA
   For this example you will need only a USB-C cable and Inkplate 4 TEMPERA.
   Select "Soldered Inkplate 4 TEMPERA" from Tools -> Board menu.
   Don't have "Soldered Inkplate 4 TEMPERA" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example will show you how to read gestures, proximity, color and
   ambient light from the built-in APDS9960 sensor.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   20 Nov 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE4TEMPERA
#error "Wrong board selection for this example, please select Inkplate 4 TEMPERA in the boards menu."
#endif

#include "Inkplate.h"   // Include Inkplate library to the sketch
#include "background.h" // Include background image


Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate library and also set library into 1 Bit mode (BW)

// Display update
#define NUM_UPDATES_BEFORE_FULL_REFRESH 30 // How many partial updates to do before a full refresh
// Variables to help control program flow
bool dataHasChanged = true; // Set to true so initial data is shown
int numUpdates = 0;

// Remember last read values from the sensor so the display can only be updated when there's new values
uint8_t lastProximityMeasured = 0;
uint16_t lastRedLightMeasured = 0;
uint16_t lastGreenLightMeasured = 0;
uint16_t lastBlueLightMeasured = 0;
uint16_t lastAmbientLightMeasured = 0;

// Setup code, runs only once
void setup()
{
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.display();      // Put clear image on display
    display.setTextSize(3); // Set text to be 3 times bigger than classic 5x7 px text
    display.setTextColor(BLACK);

    // Draw the background grid from buffer
    display.drawImage(background, 0, 0, 600, 600, WHITE, BLACK);

    // Let's confiugure the APDS!
    display.wakePeripheral(INKPLATE_APDS9960); // First, wake it from sleep
    // Initialize the APDS
    if (!display.apds9960.init())
    {
        // There's an error with the initialization, inform the user
        display.setCursor(50, 50);
        display.print("Can't init APDS!");
        display.display();

        // Go straight to sleep in this case
        esp_deep_sleep_start();
    }

    // Individual elements of the sensor have to be enabled explicitly for it to work
    // First, enable the proximity sensor without interrupts
    display.apds9960.enableProximitySensor(false);
    display.apds9960.setProximityGain(1); // Set gain to lower so it's less sensitive
    // Enable the gesture sensor as well
    display.apds9960.enableGestureSensor();
    display.apds9960.setGestureGain(0); // Also set gain to lower so it's less sensitive
    // Finally, enable the light sensor
    display.apds9960.enableLightSensor(false);
    // Config is done

    // Show the initial picture on the display with a full refresh
    display.display();
}

// This code runs repeteadly
void loop()
{
    // Make all the readings from the APDS in order
    // Review these functions for details on how it works
    readProximity();
    detectGesture();
    readColor();
    readLight();

    // Update the display if different data was read
    if (dataHasChanged)
    {
        updateDisplay();
    }

    delay(250); // Add a small delay between readings
}

// Detect a swiping gesture in front of the sensor
void detectGesture()
{
    if (display.apds9960.isGestureAvailable())
    {
        // The sensor has detected a gesture, so it should get printed

        dataHasChanged = true; // Set the flag which updates the display

        // Clear previously written data and set the cursor in the correct place
        display.fillRect(323, 72, 263, 118, WHITE);
        display.setCursor(423, 125);
        display.setTextSize(4);

        // Read the gesture
        int gesture = display.apds9960.readGesture();
        switch (gesture)
        {
        // Depending on which gesture was recorded, print different data
        case DIR_UP:
            display.print("Up");
            break;

        case DIR_DOWN:
            display.print("Down");
            break;

        case DIR_LEFT:
            display.print("Left");
            break;

        case DIR_RIGHT:
            display.print("Right");
            break;

        default:
            // Do nothing
            break;
        }
    }
}

// Get data for proximity as an 8-bit value
void readProximity()
{
    // Get the proximity the sensor is reading at thism moment
    // Note the datatype is uint8_t
    uint8_t proximity = 0;
    display.apds9960.readProximity(proximity);

    // If it's different than last time, print it
    if (proximity != lastProximityMeasured)
    {
        dataHasChanged = true; // Set the flag which updates the display

        display.fillRect(21, 72, 249, 118, WHITE);
        display.setTextSize(4);
        display.setCursor(75, 125);
        display.print(proximity);
        lastProximityMeasured = proximity; // Remember new data for later
    }
}

// Read the red, green and blue light values
void readColor()
{

    // Same principle as readProximity, note the data type
    uint16_t redLight = 0;
    uint16_t greenLight = 0;
    uint16_t blueLight = 0;
    display.apds9960.readRedLight(redLight);
    display.apds9960.readGreenLight(greenLight);
    display.apds9960.readBlueLight(blueLight);

    // If any of these has changed from before, print new data
    if (redLight != lastRedLightMeasured || greenLight != lastGreenLightMeasured || blueLight != lastBlueLightMeasured)
    {
        dataHasChanged = true; // Set the flag which updates the display

        // Clear old data in the frame buffer, set the cursors in the correct place and write new data
        display.fillRect(21, 409, 249, 126, WHITE);
        display.setTextSize(3);
        display.setCursor(32, 409);
        display.print("Red: ");
        display.print(redLight);
        display.setCursor(32, 434);
        display.print("Green: ");
        display.print(greenLight);
        display.setCursor(32, 459);
        display.print("Blue: ");
        display.print(blueLight);

        // Remember this for later so we only have to update the display when this changes
        lastRedLightMeasured = redLight;
        lastGreenLightMeasured = greenLight;
        lastBlueLightMeasured = blueLight;
    }
}

// Read the ambient light sensor data for all light
void readLight()
{
    // Same principle as in readColor()
    uint16_t ambientLight = 0;
    display.apds9960.readAmbientLight(ambientLight);

    // If the value has changed, print the data to the screen
    if (ambientLight != lastAmbientLightMeasured)
    {
        dataHasChanged = true; // Set the flag which updates the display
        
        display.fillRect(323, 409, 249, 126, WHITE); // Clear old value
        display.setTextSize(4);
        display.setCursor(413, 425);
        display.print(ambientLight);

        lastAmbientLightMeasured = ambientLight;
    }
}

// This function updates the display with the new data in the frame buffer
// It does partial updates until NUM_UPDATES_BEFORE_FULL_REFRESH is reached
// Then it does a full refresh and resets the coutner
void updateDisplay()
{
    if (numUpdates >= NUM_UPDATES_BEFORE_FULL_REFRESH)
    {
        display.display();
        numUpdates = 0;
    }
    else
    {
        display.partialUpdate(false, true);
        numUpdates++;
    }
    dataHasChanged = false;
}
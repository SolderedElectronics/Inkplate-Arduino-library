/**
 **************************************************
 *
 * @file        ColorSensor.ino
 * @brief       This example reads all data from the on-board APDS-9960 sensor:
 *              - color RGB (red, green, blue)
 *              - proximity
 *              - gesture
 *              and prints updates to the Serial Monitor every 100 ms.
 *    
 *              Product: https://solde.red/333047
 *
 *              Modified by soldered.com
 ***************************************************/

#include "APDS9960-SOLDERED.h"

APDS_9960 APDS;

void setup()
{
    Serial.begin(115200);
    while (!Serial)
        ; // Wait for Serial Monitor to open

    if (!APDS.begin())
    {
        Serial.println("Error initializing APDS-9960 sensor.");
        while (true)
            ; // Stop forever
    }
}

int proximity = 0;
int r = 0, g = 0, b = 0;
unsigned long lastUpdate = 0;

void loop()
{

    // Check if a proximity reading is available.
    if (APDS.proximityAvailable())
    {
        proximity = APDS.readProximity(); //Read proximity
    }

    // Check if a gesture reading is available
    if (APDS.gestureAvailable())
    {
        int gesture = APDS.readGesture(); //Read gesture
        switch (gesture)  //Determine which gesture was captured
        {
        case GESTURE_UP:
            Serial.println("Detected UP gesture");
            break;

        case GESTURE_DOWN:
            Serial.println("Detected DOWN gesture");
            break;

        case GESTURE_LEFT:
            Serial.println("Detected LEFT gesture");
            break;

        case GESTURE_RIGHT:
            Serial.println("Detected RIGHT gesture");
            break;

        default:
            // Ignore
            break;
        }
    }

    // Check if a color reading is available
    if (APDS.colorAvailable())
    {
        APDS.readColor(r, g, b);  //Read colors
    }

    // Print updates every 100 ms
    if (millis() - lastUpdate > 100)
    {
        lastUpdate = millis();
        Serial.print("PR=");
        Serial.print(proximity);
        Serial.print(" RGB=");
        Serial.print(r);
        Serial.print(",");
        Serial.print(g);
        Serial.print(",");
        Serial.println(b);
    }
}

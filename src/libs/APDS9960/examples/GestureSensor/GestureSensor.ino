/**
 **************************************************
 *
 * @file        ColorSensor.ino
 * @brief       This example reads gesture data from the on-board APDS-9960 
 *              sensor and prints any detected gestures to the Serial Monitor.
 *
 *              Gesture directions are as follows:
 *              - UP:    from USB connector towards antenna
 *              - DOWN:  from antenna towards USB connector
 *              - LEFT:  from analog pins side towards digital pins side
 *              - RIGHT: from digital pins side towards analog pins side
 *    
 *              Product: https://solde.red/333047
 *
 *              Modified by soldered.com
 ***************************************************/

#include "APDS9960-SOLDERED.h"

APDS_9960 APDS;

void setup()
{
    Serial.begin(115200); //Begin serial communication with PC
    while (!Serial) //Wait until serial becomes active
        ;

    if (!APDS.begin())  //Begin communication with sensor
    {
        Serial.println("Error initializing APDS-9960 sensor!");
      while(1); //Loop forever if sensor is not available
    }

    // for setGestureSensitivity(..) a value between 1 and 100 is required.
    // Higher values make the gesture recognition more sensitive but less accurate
    // (a wrong gesture may be detected). Lower values makes the gesture recognition
    // more accurate but less sensitive (some gestures may be missed).
    // Default is 80
    // APDS.setGestureSensitivity(80);

    Serial.println("Detecting gestures ...");
}
void loop()
{
    if (APDS.gestureAvailable())
    {
        // a gesture was detected, read and print to Serial Monitor
        int gesture = APDS.readGesture();

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
            // ignore
            break;
        }
    }
}

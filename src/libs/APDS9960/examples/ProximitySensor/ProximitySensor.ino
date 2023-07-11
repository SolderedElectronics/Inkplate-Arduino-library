/**
 **************************************************
 *
 * @file        ProximitySensor.ino
 * @brief       This example reads proximity data from the on-board APDS-9960 sensor of the
 *              Nano 33 BLE Sense and prints the proximity value to the Serial Monitor
 *              every 100 ms.
 *
 *              This example code is in the public domain.
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
    while (!Serial) //Wait for serial to become active
        ;

    if (!APDS.begin())  //Begin communication with sensor
    {
      Serial.println("Error initializing APDS-9960 sensor!");
      while(1);
    }
}

void loop()
{
    // check if a proximity reading is available
    if (APDS.proximityAvailable())
    {
        // read the proximity
        // - 0   => close
        // - 255 => far
        // - -1  => error
        int proximity = APDS.readProximity();

        // print value to the Serial Monitor
        Serial.println(proximity);
    }

    // wait a bit before reading again
    delay(100);
}

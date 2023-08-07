/**
 **************************************************
 *
 * @file        ColorSensor.ino
 * @brief       This example reads color data from the on-board 
 *              APDS-9960 sensor of the and prints the color RGB (red, green, blue)
 *              values to the Serial Monitor once a second.
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
    Serial.begin(115200); //Begin serial communication with PC with 115200 baud rate
    while (!Serial);  // If Serial is not available MCU will loop forever

    if (!APDS.begin())  //Begin communication with sensor
    {
      Serial.println("Error initializing APDS-9960 sensor."); //Print message if sensor is not available
      while(1); //Loop forever if there is problem with sensor
    }
}

void loop()
{
    // check if a color reading is available
    while (!APDS.colorAvailable())
    {
        delay(5); //Wait for color reading to be available
    }
    int r, g, b;  //Initialize variables for color intensities

    // read the color
    APDS.readColor(r, g, b);

    // print the values
    Serial.print("r = ");
    Serial.println(r);
    Serial.print("g = ");
    Serial.println(g);
    Serial.print("b = ");
    Serial.println(b);
    Serial.println();

    // wait a bit before reading again
    delay(1000);
}

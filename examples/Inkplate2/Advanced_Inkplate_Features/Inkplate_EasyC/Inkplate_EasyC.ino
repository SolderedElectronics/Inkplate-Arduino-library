/*
    Inkplate_easyC example for e-radionica.com Inkplate 2
    For this example you will need a micro USB cable, Inkplate 2,
    BMP180 sensor with easyC connector on it: https://www.solde.red/333060
    and a easyC cable: https://e-radionica.com/en/easyc-cable-20cm.html
    Select "Inkplate 2(ESP32)" from Tools -> Board menu.
    Don't have "Inkplate 2(ESP32)" option? Follow our tutorial and add it:
    https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

    This example will show you how you can read temperature, humidity, air pressure and gas data from BMP180.
    In order to compile this example successfuly, you will also need to download and install
    BMP180 library: https://github.com/e-radionicacom/Soldered-BMP180-Temperature-Pressure-Sensor-Arduino-Library.
    If you don't know how to install library you can read our tutorial https://e-radionica.com/en/blog/arduino-library/

    Want to learn more about Inkplate? Visit www.inkplate.io
    Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
    7 April 2022 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Inkplate 2 in the boards menu."
#endif

#include "libs/bmp180/BMP180-SOLDERED.h" //Adafruit library for BMP180 Sensor
#include "Inkplate.h"        //Include Inkplate library to the sketch

#include "thermometer.h"
#include "pressure.h"

Inkplate display; // Create an object on Inkplate library
Bmp_180 myBmp; // Create an object BMP180 library
//(with no arguments sent to constructor, that means we are using I2C communication for BMP180 sensor)

void setup()
{
    Serial.begin(115200); // Initialize Serial communication with PC
    display.begin();        // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay(); // Clear frame buffer of display
    display.setTextColor(INKPLATE2_BLACK, INKPLATE2_WHITE);

    if (!myBmp.begin())
    {   // Init. BMP180 library. e-radionica.com BMP180 sensor board uses 0x76 I2C address for sensor
        Serial.println("Sensor init failed"); //Print message on serial monitor

        display.println("Sensor init failed!"); // Print message on display
        display.println("Check sensor wiring/connection!");
        display.display();
        while (1)
            ;
    }
}

void loop()
{
    double temp, pressure, p0;
    int status = myBmp.startTemperature();
    if (status != 0)
    {
        delay(status); // Delay some time needed for measure
        status = myBmp.getTemperature(temp); //Get temperature
        status = myBmp.startPressure(3); // Start measuring pressure
        delay(status); // Delay some time needed for measure
        status = myBmp.getPressure(pressure, temp);
        // The pressure sensor returns absolute pressure, which varies with altitude.
        // To remove the effects of altitude, use the sealevel function and your current altitude.
        // This number is commonly used in weather reports.
    }

    display.setTextSize(2); // Set text scaling to two (text will be two times bigger than normal)
    display.setTextColor(INKPLATE2_BLACK);
    display.clearDisplay(); // Print out new data
    display.drawImage(thermometer, 20, 8 , 18, 45); //Draw icon
    display.setCursor(60, 22);
    display.print(temp, 2); // Print air temperature
    display.println(" C");

    display.drawImage(pressure_icon, 5, 58 , 45, 45); //Draw icon
    display.setCursor(60, 72);
    display.print(pressure, 2); //Convert to air pressure hPa and print on display
    display.println(" hPa");

    display.display();
    delay(60000); // Wait 60000 miliseconds or 1 minute
}

/*
   Basic_touchscreen_serial example for e-radionica Inkplate 6 plus
   For this example you will need only USB cable and Inkplate 6 plus
   Select "Inkplate 6 plus(ESP32)" from Tools -> Board menu.
   Don't have "Inkplate 6 plus(ESP32)" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   This example shows you how to use Inkplate 6 plus'es touchscreen.
   Once the code is uploaded, open the serial monitor in Arduino IDE and you'll see touchscreen events there.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   11 February 2021 by e-radionica.com
*/

#include "Inkplate.h"
Inkplate display(INKPLATE_1BIT);

void setup()
{
    // put your setup code here, to run once:
    Serial.begin(115200);
    display.begin();
    // Init touchscreen and power it on after init (send false as argument to put it in deep sleep right after init)
    if (display.tsInit(true))
    {
        Serial.println("Touchscreen init ok");
    }
    else
    {
        Serial.println("Touchscreen init fail");
        while (true)
            ;
    }

    // NOTE!!!
    // Touchscreen cooridinates are automatically swapped and adjusted when screen is rotated
    display.setRotation(2);
    display.fillTriangle(10, 10, 20, 40, 40, 20, BLACK);
    display.setTextSize(3);
    display.setCursor(60, 60);
    display.print("(0,0) position");
    display.display();
}

void loop()
{
    // Check if there is any touch detected
    if (display.tsAvailable())
    {
        uint8_t n;
        uint16_t x[2], y[2];

        // See how many fingers are detected (max 2) and copy x and y position of each finger on touchscreen
        n = display.tsGetData(x, y);
        if (n != 0)
        {
            Serial.printf("%d finger%c ", n, n > 1 ? 's' : NULL);
            for (int i = 0; i < n; i++)
            {
                Serial.printf("X=%d Y=%d ", x[i], y[i]);
            }
            Serial.println();
        }
        else
        {
            x[0] = 0;
            x[1] = 0;
            y[0] = 0;
            y[1] = 0;
            Serial.println("Release");
        }
    }
}
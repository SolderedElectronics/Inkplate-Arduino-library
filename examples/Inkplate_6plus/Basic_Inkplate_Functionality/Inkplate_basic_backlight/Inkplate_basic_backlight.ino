#include "Inkplate.h" //Include Inkplate library

Inkplate display(INKPLATE_1BIT); // Create an object on Inkplate class

int b = 31; // Variable that holds intensity of the backlight

void setup()
{
    Serial.begin(115200);    // Set up a serial communication of 115200 baud
    display.begin();         // Init Inkplate library
    display.backlight(true); // Enable backlight circuit
    display.setBacklight(b); // Set backlight intensity
}

void loop()
{
    if (Serial.available()) // Change backlight value by sending "+" sign into serial monitor to increase backlight or
                            // "-" sign to decrese backlight
    {
        bool change = false; // Variable that indicates that backlight value has changed and intessity has to be updated
        char c = Serial.read(); // Read incomming serial data

        if (c == '+' && b < 63) // If is received +, increase backlight
        {
            b++;
            change = true;
        }
        if (c == '-' && b > 0) // If is received -, decrease backlight
        {
            b--;
            change = true;
        }

        if (change) // If backlight valuse has changed, update the intensity and show current value of backlight
        {
            display.setBacklight(b);
            Serial.print("Backlight:");
            Serial.print(b, DEC);
            Serial.println("/63");
        }
    }
}

/*
   Clock example for Soldered Inkplate 2
   For this example you will need only USB cable, Inkplate 2 and a WiFi with stable Internet connection.
   Select "Soldered Inkplate 2" from Tools -> Board menu.
   Don't have "Soldered Inkplate 2" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   This example contains three types of clocks. First type is digital clock
   with 4 digits which displays hours and minutes. Second type is binary clock,
   which also have digits but displayed in binary numbers. Third type is analog
   clock with hands.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   30 March 2022 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Soldered Inkplate 2 in the boards menu."
#endif

#include "Inkplate.h" // Include Inkplate library to the sketch

#include "Network.h" // Our networking functions, declared in Network.cpp

Network network; // Create network object for WiFi and HTTP functions

Inkplate display; // Create Inkplate library object

//---------- CHANGE HERE  -------------:

// Change mode of clock
// 0 is digital clock
// 1 is binary clock
// 2 is analog clock
uint8_t MODE = 1;

// Adjust your time zone, 2 means UTC+2
int timeZone = 2;

// Put in your ssid and password
char ssid[] = "";
char pass[] = "";

//----------------------------------

// Bitmaps for 7 segment display. Converted using Inkplate Image Converter https://inkplate.io/home/image-converter/
#include "includes/eight.h"
#include "includes/five.h"
#include "includes/four.h"
#include "includes/nine.h"
#include "includes/one.h"
#include "includes/seven.h"
#include "includes/six.h"
#include "includes/three.h"
#include "includes/two.h"
#include "includes/zero.h"

#define uS_TO_S_FACTOR 1000000 // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP  300     // How long ESP32 will be in deep sleep (in seconds)

// Array for digital display 7 segment numbers bitmaps
const uint8_t *numbers[] = {zero, one, two, three, four, five, six, seven, eight, nine};

struct tm t; // Structure that contains time info

void setup()
{
    // Begin serial communitcation
    Serial.begin(115200);

    // Initialize inkplate libarry and set text settings
    display.begin();
    display.setTextWrap(true);
    display.setTextColor(INKPLATE2_BLACK, INKPLATE2_WHITE);

    // Our begin function
    network.begin(ssid, pass);

    drawTime(); // Call function drawTime

    display.display(); // Display content from buffer on Inkplate

    // Go to sleep before checking again
    // rtc_gpio_isolate(GPIO_NUM_12);   // Isolate/disable GPIO12 on ESP32 (only to reduce power consumption in sleep)
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
    esp_deep_sleep_start();
}

void loop()
{
    // Never here
}
// Function to draw time
void drawTime()
{
    // Just draw time
    int i = 1;

    // Save current date string, more about it in Network.cpp
    network.getTime(&t, timeZone);

    // Clear the frame buffer of the epaper.
    display.clearDisplay();

    // Choose what type of clock is going to be displayed (Digital, binary or analog).
    // Selection is made by modifiying MODE variable at the start of the example.
    switch (MODE)
    {
    case 0: // Digital clock
        display.drawBitmap(0, 10, numbers[t.tm_hour / 10], 48, 84,
                           INKPLATE2_RED); // Get first number of hours and draw it
        display.drawBitmap(50, 10, numbers[t.tm_hour % 10], 48, 84,
                           INKPLATE2_RED); // Get second number of hours and draw it
        display.drawBitmap(112, 10, numbers[t.tm_min / 10], 48, 84,
                           INKPLATE2_RED); // Get first number of minutes and draw it
        display.drawBitmap(162, 10, numbers[t.tm_min % 10], 48, 84,
                           INKPLATE2_RED); // Get second number of minutes and draw it
        display.fillRect(101, 32, 8, 8, INKPLATE2_BLACK);
        display.fillRect(101, 64, 8, 8, INKPLATE2_BLACK); // Draw rectangles to separate hours and minutes
        break;
    case 1: // Binary clock
        uint8_t buf1, buf2;
        for (int i = 0; i < 4; i++)
        {
            switch (i)
            {
            case 0: // In first iteration draw hours
                buf1 = t.tm_hour / 10;
                buf2 = t.tm_hour % 10;
                break;
            case 1: // In second iteration draw minutes
                buf1 = t.tm_min / 10;
                buf2 = t.tm_min % 10;
                break;
            case 2: // In third iteration draw day
                buf1 = t.tm_mday / 10;
                buf2 = t.tm_mday % 10;
                break;
            case 3: // In fourth iteration draw month
                buf1 = (t.tm_mon + 1) / 10;
                buf2 = (t.tm_mon + 1) % 10;
                break;
            }
            for (int j = 3; j >= 0; j--)
            {
                if (buf2 & (1 << (3 - j))) // Compare numbers bitwise, if they match, draw red circle for that bit, if
                                           // not draw black outline of circle
                {
                    display.fillCircle(30 + 50 * i, 10 + 16 * j, 6, INKPLATE2_RED);
                }
                else
                {
                    display.drawCircle(30 + 50 * i, 10 + 16 * j, 6, INKPLATE2_BLACK);
                }
                if (j > 0) // Do not do this if j is 0
                {
                    if (buf1 & (1 << (3 - j))) // Compare numbers bitwise, if they match, draw red circle for that bit,
                                               // if not draw black outline of circle
                    {
                        display.fillCircle(14 + 50 * i, 10 + 16 * j, 6,
                                           INKPLATE2_RED); // "Turn on" the bit in the clock
                    }
                    else
                    {
                        display.drawCircle(14 + 50 * i, 10 + 16 * j, 6,
                                           INKPLATE2_BLACK); // "Turn off" the bit in the binary clock
                    }
                }
            }
            // This block of code draws informations what which part of screen represents
            display.setCursor(10, 72);
            display.setTextSize(2);
            display.setTextColor(INKPLATE2_BLACK, INKPLATE2_WHITE);
            display.print("HH");
            display.setCursor(62, 72);
            display.print("MM");
            display.setCursor(112, 72);
            display.print("DD");
            display.setCursor(162, 72);
            display.print("MM");
            display.fillRect(45, 21, 5, 5, INKPLATE2_BLACK);
            display.fillRect(45, 44, 5, 5, INKPLATE2_BLACK);
            display.fillRect(145, 58, 5, 5, INKPLATE2_BLACK);

            // Draw number that represents values of circles
            display.setTextSize(1);
            display.setCursor(190, 7);
            display.print("8");
            display.setCursor(190, 23);
            display.print("4");
            display.setCursor(190, 39);
            display.print("2");
            display.setCursor(190, 55);
            display.print("1");

            display.drawLine(100, 0, 100, 104, INKPLATE2_BLACK);

            // Draw number that represents values of circles
            display.setTextSize(1);
            display.setCursor(90, 7);
            display.print("8");
            display.setCursor(90, 23);
            display.print("4");
            display.setCursor(90, 39);
            display.print("2");
            display.setCursor(90, 55);
            display.print("1");
        }
        break;
    case 2: // Analog clock

        // Draw face of the clock (actually outline)
        display.drawCircle(106, 52, 50, 1); // Draw outer circles
        display.drawCircle(106, 52, 51, 1);

        // Draw lines that represents 5, 10, 20, 25, 35, 40, 50, 55 on the clock dial face
        display.drawThickLine(63, 27, 67, 30, 1, 1);
        display.drawThickLine(145, 74, 150, 77, 1, 1);
        display.drawThickLine(63, 77, 67, 75, 1, 1);
        display.drawThickLine(145, 30, 149, 27, 1, 1);
        display.drawThickLine(81, 95, 83, 91, 1, 1);
        display.drawThickLine(129, 13, 131, 9, 1, 1);
        display.drawThickLine(81, 9, 83, 13, 1, 1);
        display.drawThickLine(129, 91, 131, 95, 1, 1);

        // Draw numbers 3, 6, 9, 12 on the dial face.
        display.setTextSize(1);
        display.setCursor(150, 49);
        display.println('3');
        display.setCursor(103, 92);
        display.println('6');
        display.setCursor(58, 49);
        display.println('9');
        display.setCursor(101, 6);
        display.println("12");

        // Draw dot in middle
        display.fillCircle(106, 52, 5, 1);

        // Now calculat the angle of each hand of the clock and draw it on display.
        int x_minute, y_minute, x_hour, y_hour;

        // Calculations for x and y end position of the line (start is in the middle of the clock)
        x_minute = 106 + 40 * (float)sin((t.tm_min / (float)60) * 2 * (float)3.14);
        y_minute = 52 - 40 * (float)cos((t.tm_min / (float)60) * 2 * (float)3.14);

        // Do simillar calculations for hour hand
        x_hour = 106 + 30 * sin((t.tm_hour / (float)12 + t.tm_min / (float)720) * 2 * (float)3.14);
        y_hour = 52 - 30 * cos((t.tm_hour / (float)12 + t.tm_min / (float)720) * 2 * (float)3.14);

        // Draw them on the dispaly!
        display.drawThickLine(106, 52, x_minute, y_minute, INKPLATE2_RED, 2); // Hand for minutes
        display.drawThickLine(106, 52, x_hour, y_hour, INKPLATE2_BLACK, 3);   // Hand for hours
        break;
    }
}

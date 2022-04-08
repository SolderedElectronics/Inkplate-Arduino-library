/*
   Cryptocurrency tracker example for e-radionica.com Inkplate 2
   For this example you will need only USB cable and Inkplate 2.
   Select "Inkplate 2(ESP32)" from Tools -> Board menu.
   Don't have "Inkplate 2(ESP32)" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   This example will show you how you can use Inkplate 6 to display API data.
   Here we use Coingecko API to get latest cryptocurrency prices and display
   them on the Inkplate screen. If you wish to change the currecny, you can
   edit it below.

   IMPORTANT:
   Make sure to change your timezone and wifi credentials below
   Also have ArduinoJSON installed in your Arduino libraries, download here: https://arduinojson.org/

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   30 March 2022 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Inkplate 2 in the boards menu."
#endif

//---------- CHANGE HERE  -------------:

// Adjust your time zone, 2 means UTC+2
int timeZone = 2;

// Put in your ssid and password
char ssid[] = "Soldered";
char pass[] = "dasduino";

//----------------------------------

// Include Inkplate library to the sketch
#include "Inkplate.h"


// Our networking functions, declared in Network.cpp
#include "Network.h"

#include "includes/zero.h"
#include "includes/one.h"
#include "includes/two.h"
#include "includes/three.h"
#include "includes/four.h"
#include "includes/five.h"
#include "includes/six.h"
#include "includes/seven.h"
#include "includes/eight.h"
#include "includes/nine.h"

const uint8_t *numbers[] = {zero, one, two, three, four, five, six, seven, eight, nine};

uint8_t clockMode = 2;

// create object with all networking functions
Network network;

// create display object
Inkplate display;

struct tm t;

void setup()
{
  // Begin serial communitcation, sed for debugging
  Serial.begin(115200);

  // Initial display settings
  display.begin();
  display.setTextWrap(true);
  display.setTextColor(BLACK, WHITE);

  // Welcome screen
  display.setCursor(10, 10); // Set cursor, custom font uses different method for setting cursor
  // You can find more about that here https://learn.adafruit.com/adafruit-gfx-graphics-library/using-fonts
  display.setTextSize(2);
  display.println(F("Welcome to Inkplate 2 Clock example!"));
  display.setCursor(10, 10);
  display.display();
  display.clearDisplay();
  delay(1000);

  // Our begin function
  network.begin();

  drawTime();

  display.display();

  // Go to sleep before checking again
  esp_sleep_enable_timer_wakeup(100000000);
  (void)esp_deep_sleep_start();
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
  network.getTime(&t);
  display.clearDisplay();
  switch (clockMode)
  {
    case 0:
      display.drawBitmap(0, 10, numbers[t.tm_hour / 10], 48, 84, RED);
      display.drawBitmap(50, 10, numbers[t.tm_hour % 10], 48, 84, RED);
      display.drawBitmap(112, 10, numbers[t.tm_min / 10], 48, 84, RED);
      display.drawBitmap(162, 10, numbers[t.tm_min % 10], 48, 84, RED);
      display.fillRect(101, 32, 8, 8, BLACK);
      display.fillRect(101, 64, 8, 8, BLACK);
      break;
    case 1:
      uint8_t buf1, buf2;
      for (int i = 0; i < 4; i++)
      {
        switch (i)
        {
          case 0:
            buf1 = t.tm_hour / 10;
            buf2 = t.tm_hour % 10;
            break;
          case 1:
            buf1 = t.tm_min / 10;
            buf2 = t.tm_min % 10;
            break;
          case 2:
            buf1 = t.tm_mday / 10;
            buf2 = t.tm_mday % 10;
            break;
          case 3:
            buf1 = (t.tm_mon + 1) / 10;
            buf2 = (t.tm_mon + 1) % 10;
            break;
        }
        for (int j = 3; j >= 0; j--)
        {
          if (buf2 & (1 << (3 - j)))
          {
            display.fillCircle(30 + 50 * i , 10 + 16 * j, 6, RED);
          }
          else
          {
            display.drawCircle(30 + 50 * i , 10 + 16 * j, 6, BLACK);
          }
          if (j > 0)
          {
            if (buf1 & (1 << (3 - j)))
            {
              display.fillCircle(14 + 50 * i , 10 + 16 * j, 6, RED);
            }
            else
            {
              display.drawCircle(14 + 50 * i , 10 + 16 * j, 6, BLACK);
            }

          }
        }
        display.setCursor(10, 72);
        display.setTextSize(2);
        display.setTextColor(BLACK, WHITE);
        display.print("HH");
        display.setCursor(62, 72);
        display.print("MM");
        display.setCursor(112, 72);
        display.print("DD");
        display.setCursor(162, 72);
        display.print("MM");
        display.fillRect(45, 21, 5, 5, BLACK);
        display.fillRect(45, 44, 5, 5, BLACK);
        display.fillRect(145, 58, 5, 5, BLACK);

        display.setTextSize(1);
        display.setCursor(190, 7);
        display.print("8");
        display.setCursor(190, 23);
        display.print("4");
        display.setCursor(190, 39);
        display.print("2");
        display.setCursor(190, 55);
        display.print("1");

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
    case 2:

      int min_needle_len;
      int hour_needle_len;
      //this part of code draws clock
      display.drawCircle(106, 52, 50, 1);
      display.drawCircle(106, 52, 51, 1);
      //display.drawThickLine(20,220,40,220,1,2); //draws lines instead of numbers
      //display.drawThickLine(220,20,220,40,1,2);
      //display.drawThickLine(400,220,420,220,1,2);
      //display.drawThickLine(220,400,220,420,1,2);
      display.drawThickLine(63, 27, 67, 30, 1, 1);
      display.drawThickLine(145, 74, 150, 77, 1, 1);
      display.drawThickLine(63, 77, 67, 75, 1, 1);
      display.drawThickLine(145, 30, 149, 27, 1, 1);
      
      display.drawThickLine(81, 95, 84, 91, 1, 1);
      display.drawThickLine(129, 13, 131, 9, 1, 1);
      display.drawThickLine(81, 9, 84, 13, 1, 1);
      display.drawThickLine(129, 91, 131, 95, 1, 1);
      display.setTextSize(1);
      display.setCursor(150, 49);
      display.println('3');
      display.setCursor(103, 92);
      display.println('6');
      display.setCursor(58, 49);
      display.println('9');
      display.setCursor(102, 10);
      display.println("12");
      min_needle_len = 150;
      hour_needle_len = 100;
      display.fillCircle(106, 52, 5, 1);
      //this part of code draws needles and calculates their angles
      int x_minute, y_minute, x_hour, y_hour;
      x_minute = 106 + 25 + 15 * (float) sin((t.tm_min / (float)60) * 2 * (float)3.14);
      y_minute = 52 + 25 - 15 * (float) cos((t.tm_min / (float)60) * 2 * (float)3.14);
      x_hour = 106 + 25 + 15 * sin((t.tm_hour / (float)12 + t.tm_min / (float)720) * 2 * (float)3.14);
      y_hour = 52 + 25 - 15 * cos((t.tm_hour / (float)12 + t.tm_min / (float)720) * 2 * (float)3.14);
      display.drawThickLine(106, 52, x_minute, y_minute, RED, 4); //needle for minutes
      display.drawThickLine(106, 52, x_hour, y_hour, BLACK, 4); //needle for hours

      break;

  }
  // Text settings
}

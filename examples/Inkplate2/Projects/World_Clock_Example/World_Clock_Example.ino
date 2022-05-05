/*
   World clock example for e-radionica.com Inkplate 2
   For this example you will need only USB cable and Inkplate 2.
   Select "Inkplate 2(ESP32)" from Tools -> Board menu.
   Don't have "Inkplate 2(ESP32)" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   This example uses API call to get tim for wanted city and it's timezone.
   Fetched data is in JSON format, and library is used to extract data. To choose
   city just type any part of city's name and it will be automatically found, but if you type
   to few letters, any city containig that letters will be found.

   IMPORTANT:
   Make sure to change your wifi credentials below
   Also have ArduinoJSON installed in your Arduino libraries, download here: https://arduinojson.org/

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   12 April 2022 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Inkplate 2 in the boards menu."
#endif

// Put in your ssid and password
char ssid[] = "";
char pass[] = "";

//----------------------------------

// Include Inkplate library to the sketch
#include "Inkplate.h"


// Our networking functions, declared in Network.cpp
#include "Network.h"
#include "Inter8pt7b.h"

// Create object with all networking functions
Network network;

// Create display object
Inkplate display;

struct tm t;

// Here you can type part of city's and it will be found automatically.
// The more letters you type, the more chance is that your city will be found.
// Refer to Network.h file for full list of cities. Instead of space use underline
// and every word starts with CAPITAL letter and the rest of letters are not capitals.
const char city1[] = "Zag";
const char city2[] = "Kuala_Lu";

// Pointers to store city names
const char *city1_name = NULL, *city2_name = NULL;

void setup()
{
  // Begin serial communitcation, set for debugging
  Serial.begin(115200);

  // Initial display settings
  display.begin();

  // Our begin function
  network.begin();

  network.getData((char*)city1, &t);
  drawTime(17, 1, t.tm_hour > 12 ? 1 : 0, city1_name); // x coordinate, y coordinate, invert colors, pointer to city name
  // Use ternary operator to specify if colors are inverted. If time is AM
  // (less than 12 hours) then colors are not inverted and if 12 hours has past
  // invert colors.
  
  network.getData((char*)city2, &t);
  drawTime(115, 1, t.tm_hour > 12 ? 1 : 0, city2_name);

  display.display();
  
  // Go to sleep before checking again, uncomment if you want to use 
  //esp_sleep_enable_timer_wakeup(100000000); //Go to sleep for 100 million microseconds or 100 seconds
  //(void)esp_deep_sleep_start();
}

void loop()
{
  // Never here
}
// Function to draw time
void drawTime(uint16_t x_pos, uint16_t y_pos, bool color, const char *city_name)
{
  uint16_t w = 80; //Clock width
  // This part of code draws analog clock
  display.drawCircle(x_pos + w / 2, y_pos + w / 2, w / 2, INKPLATE2_BLACK); //Draw outer circles
  display.drawCircle(x_pos + w / 2, y_pos + w / 2, w / 2 + 1, INKPLATE2_BLACK);

  //Draws lines that represents 3, 6 ,9 and 12 hours on the clocks face
  display.drawThickLine(x_pos + w / 2, y_pos        , x_pos + w / 2, y_pos + 5, 1 + color, 2); 
  display.drawThickLine(x_pos + w    , y_pos + w / 2, x_pos + w - 5, y_pos + w / 2, 1 + color, 2);
  display.drawThickLine(x_pos + w / 2, y_pos + w    , x_pos + w / 2, y_pos + w - 5, 1 + color, 2);
  display.drawThickLine(x_pos        , y_pos + w / 2, x_pos + 5    , y_pos + w / 2, 1 + color, 2);

  // Draw lines that represents 5, 10, 20, 25, 35, 40, 50, 55 minutes on the clocks face
  // Lines are calculated from here https://www.desmos.com/calculator/l2mkh2guac
  display.drawThickLine(x_pos + 0.750 * w, y_pos + 0.070 * w, x_pos + 0.725 * w, y_pos + 0.110 * w, 1 + color, 1);
  display.drawThickLine(x_pos + 0.930 * w, y_pos + 0.250 * w, x_pos + 0.895 * w, y_pos + 0.275 * w, 1 + color, 1);
  display.drawThickLine(x_pos + 0.930 * w, y_pos + 0.750 * w, x_pos + 0.895 * w, y_pos + 0.725 * w, 1 + color, 1);
  display.drawThickLine(x_pos + 0.750 * w, y_pos + 0.930 * w, x_pos + 0.725 * w, y_pos + 0.895 * w, 1 + color, 1);
  display.drawThickLine(x_pos + 0.250 * w, y_pos + 0.930 * w, x_pos + 0.275 * w, y_pos + 0.895 * w, 1 + color, 1);
  display.drawThickLine(x_pos + 0.070 * w, y_pos + 0.750 * w, x_pos + 0.111 * w, y_pos + 0.725 * w, 1 + color, 1);
  display.drawThickLine(x_pos + 0.070 * w, y_pos + 0.250 * w, x_pos + 0.111 * w, y_pos + 0.275 * w, 1 + color, 1);
  display.drawThickLine(x_pos + 0.250 * w, y_pos + 0.070 * w, x_pos + 0.275 * w, y_pos + 0.110 * w, 1 + color, 1);

  // Draw filled circle in the middle
  display.fillCircle(x_pos + w / 2, y_pos + w / 2, 5, 1 + color);

  // This part of code draws needles and calculates their angles
  int x_minute, y_minute, x_hour, y_hour;
  x_minute = x_pos + w / 2 + 30 * (float) sin((t.tm_min / (float)60) * 2 * (float)3.14); //
  y_minute = y_pos + w / 2 - 30 * (float) cos((t.tm_min / (float)60) * 2 * (float)3.14);
  x_hour = x_pos + w / 2  + 22 * sin((t.tm_hour / (float)12 + t.tm_min / (float)720) * 2 * (float)3.14);
  y_hour = y_pos + w / 2 - 22 * cos((t.tm_hour / (float)12 + t.tm_min / (float)720) * 2 * (float)3.14);
  
  display.drawThickLine(x_pos + w / 2, y_pos + w / 2, x_minute, y_minute, INKPLATE2_RED - color, 2); // Needle for minutes
  display.drawThickLine(x_pos + w / 2, y_pos + w / 2, x_hour, y_hour, INKPLATE2_BLACK  + color, 3); // Needle for hours
  
  display.setTextSize(1); // Set text size in comparison to original text 5x7
  display.setFont(&Inter8pt7b); // Set customn font
  display.setTextColor(INKPLATE2_BLACK, INKPLATE2_WHITE);
  char *temp_city_name = strstr(city_name , "/") + 1;
  display.setCursor(x_pos + 40 - strlen(temp_city_name) * 5, 100); // Center city name
  
  uint8_t cnt = 0;
  while(*(temp_city_name + cnt) != '\0') // Print city name letter by letter
  {
    if(*(temp_city_name + cnt) != '_') //If current letter is underline symbol, replace it with space
      display.print(*(temp_city_name + cnt)); 
    else
      display.print(" "); // Print space
    cnt++;
  }
}

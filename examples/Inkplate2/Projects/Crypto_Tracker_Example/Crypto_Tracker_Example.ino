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

// OPTIONAL:
// change to a different currency
char currency[] = "bitcoin";
char currencyAbbr[] = "BTC";

// You can find your currency id here:
// https://api.coingecko.com/api/v3/coins

// If it loads weirdly you can search the JSON using ctrl/command+f for
// your crypto by name and then find it's id next to it's name and copy those above

//----------------------------------

// Include Inkplate library to the sketch
#include "Inkplate.h"

// Include fonts used
#include "Fonts/Roboto_Light16.h"
#include "Fonts/Roboto_Light8.h"

// Our networking functions, declared in Network.cpp
#include "Network.h"

// create object with all networking functions
Network network;

// create display object
Inkplate display;

// Delay between API calls in miliseconds
#define DELAY_MS 3 * 60 * 1000

// Used for storing raw price values
double data[64];

// Used to simplify UI design
struct textElement
{
  int x;
  int y;
  const GFXfont *font;
  char *text;
  char align;
  uint8_t text_color;
};

// Variables for storing all displayed data as char arrays
char date[64];
char fromToDate[64];

char dates[8 * 8];
char prices[16 * 16];

char current[16];
char minimum[16];
char maximum[16];

// All months in a year, for finding current date
char months[][12] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
};

// Out UI elements data
textElement elements[] = {
  {10, 25, &Roboto_Light16, currencyAbbr, 0 , BLACK}, {100, 18, &Roboto_Light8, date, 0, BLACK},
  {10, 150, &Roboto_Light8, fromToDate, 0, BLACK},   {10, 40, &Roboto_Light8, "Current price:", 0, BLACK},
  {10, 60, &Roboto_Light8, current, 0, RED},      {130, 40, &Roboto_Light8, "Minimum:", 0, BLACK},
  {130, 60, &Roboto_Light8, minimum, 0, RED},      {130, 80, &Roboto_Light8, "Maximum:", 0, BLACK},
  {130, 100, &Roboto_Light8, maximum, 0, RED}
};

// Our functions declared below setup and loop
void drawAll();
void getCurrencyData();

void setup()
{
  // Begin serial communitcation, sed for debugging
  Serial.begin(115200);

  // Initial display settings
  display.begin();
  display.setTextWrap(false); //Disable text wrapping
  display.setTextColor(0, 7);

  // Welcome screen
  display.setCursor(10, 10); // Set cursor, custom font uses different method for setting cursor
  // You can find more about that here https://learn.adafruit.com/adafruit-gfx-graphics-library/using-fonts
  display.setTextSize(2); //Set size of font in comparison to original 5x7 font
  display.println(F("Welcome to Inkplate 2 cryptocurrency tracker example!"));
  display.setCursor(0, 0);
  display.println(F("Connecting to WiFi..."));
  display.display();
  display.clearDisplay();
  delay(1000);

  // Our begin function
  network.begin();

  while (!network.getData(data)) //Get data and check if data is successfully fetched
  {
    Serial.println("Retrying retriving data!");
    delay(1000);
  }

  // Our main drawing function
  drawAll();
  // Full refresh
  display.display();

  // Go to sleep before checking again
  esp_sleep_enable_timer_wakeup(1000ll * DELAY_MS);
  (void)esp_deep_sleep_start();
}

void loop()
{
  // Never here
}

void getCurrencyData()
{
  // Set min to a very high value, and max to very low, so that any real world data changes it
  double minData = 1e9F;
  double maxData = -1e9F;

  // Find min and max in data
  for (int i = 0; i < 31; ++i)
  {
    minData = min(minData, data[i]);
    maxData = max(maxData, data[i]);
  }

  double span = max(0.3D, (double)abs(maxData - minData));

  // Copy current, min and max data to char arrays to be displayed
  dtostrf(data[30], 8, 2, current);
  strcat(current, "$");
  dtostrf(minData, 8, 2, minimum);
  strcat(minimum, "$");
  dtostrf(maxData, 8, 2, maximum);
  strcat(maximum, "$");

  // Temporary buffer
  char temp[64];

  for (int i = 0; i < 4; ++i)
  {
    dtostrf(minData + (double)i / 4 * span, 5, (maxData < 10.0D ? 3 : 0), temp);
    strncpy(prices + 16 * (3 - i), temp, 16);
  }

  // Find current day in a month
  int day;
  sscanf(date + 3, "%d", &day);

  // Find current month
  int month = 0;
  for (int i = 0; i < 12; ++i)
  {
    if (strncmp(months[i], date, 3) == 0)
      month = i + 1;
  }
}

// Function to draw time
void drawTime()
{
  // Just draw time
  int i = 1;

  // Save current date string, more about it in Network.cpp
  network.getTime(date);

  // Text settings
  display.setTextColor(BLACK, WHITE);
  display.setFont(elements[i].font);
  display.setTextSize(1);

  // 0 is aligned by left bottom corner, 1 by right
  if (elements[i].align == 0)
    display.setCursor((int)(elements[i].x * 0.96), (int)(elements[i].y));

  // Print out text to above set cursor location
  display.print(date);
}

// Our main drawing function
void drawAll()
{
  // Save current date string, more about it in Network.cpp
  network.getTime(date);

  // Find current day from string
  int day;
  sscanf(date + 3, "%d", &day);

  getCurrencyData();

  // Find what month is it numericly and display it
  for (int i = 0; i < 12; ++i)
  {
    if (strncmp(months[i], date, 3) == 0)
      sprintf(fromToDate, "%d.%d. to %d.%d.", day, ((i + 1) % 12 ? i + 1 : 12), day, ((i + 2) % 12 ? i + 2 : 12));
  }

  // Draw our UI elements
  for (int i = 0; i < sizeof(elements) / sizeof(elements[0]); ++i)
  {
    // Text settings
    display.setTextColor(elements[i].text_color, WHITE);
    display.setFont(elements[i].font);
    display.setTextSize(1);

    // 0 is aligned by left bottom corner, 1 by right
    if (elements[i].align == 0)
      display.setCursor((int)(elements[i].x * 0.96), (int)(elements[i].y));
    else if (elements[i].align == 1)
    {
      int16_t x, y;
      uint16_t w, h;

      // Get how much the textx offsets pointer and draw it that much more left
      display.getTextBounds(elements[i].text, 0, 0, &x, &y, &w, &h);

      display.setCursor((int)(elements[i].x * 0.96) - w, (int)(elements[i].y));
    }

    // Print out text to above set cursor location
    display.print(elements[i].text);
  }
}

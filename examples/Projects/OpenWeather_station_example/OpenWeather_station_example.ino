/*
   Weather station example for e-radionica.com Inkplate 6
   For this example you will need only USB cable and Inkplate 6.
   Select "Inkplate 6(ESP32)" from Tools -> Board menu.
   Don't have "Inkplate 6(ESP32)" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   This example will show you how you can use Inkplate 6 to display API data,
   e.g. Metaweather public weather API

   IMPORTANT:
   Make sure to change your desired city, timezone and wifi credentials below
   Also have ArduinoJSON installed in your Arduino libraries

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   28 July 2020 by e-radionica.com

   Code for Moonphase and moon fonts taken from here: https://learn.adafruit.com/epaper-weather-station/arduino-setup
*/

//WiFi Connection required
#include <WiFi.h>

//Required libraries
#include <ArduinoJson.h>
#include "HTTPClient.h"
#include <TimeLib.h>

// ---------- CHANGE HERE  -------------:

// Change to your wifi ssid and password

#include "OpenWeatherOneCall.h"
#define HOMESSID ""
#define HOMEPW ""

// Openweather set up information
#define ONECALLKEY ""

float myLatitude = 0.00000; //I got this from Wikipedia
float myLongitude = 0.00000;

bool metric = true; //<------------------------------TRUE is METRIC, FALSE is IMPERIAL, BLANK is KELVIN

// ----------------------------------

// Include Inkplate library to the sketch
#include "Inkplate.h"

// Including fonts used
#include "Fonts/Roboto_Light_48.h"
#include "Fonts/Roboto_Light_36.h"
#include "Fonts/Roboto_Light_120.h"
#include "Fonts/FreeSans18pt7b.h"
#include "Fonts/FreeSans12pt7b.h"
#include "Fonts/FreeSans9pt7b.h"
#include "Fonts/moon_phases20pt7b.h"
#include "Fonts/moon_phases36pt7b.h"

// Delay between API calls
#define DELAY_MS 59000

// Inkplate object
Inkplate display(INKPLATE_1BIT);

enum alignment { LEFT_BOT, CENTRE_BOT, RIGHT_BOT, LEFT, CENTRE, RIGHT, LEFT_TOP, CENTRE_TOP, RIGHT_TOP };

const char *moonphasenames[29] = {
  "New Moon",
  "Waxing Crescent",
  "Waxing Crescent",
  "Waxing Crescent",
  "Waxing Crescent",
  "Waxing Crescent",
  "Waxing Crescent",
  "Quarter",
  "Waxing Gibbous",
  "Waxing Gibbous",
  "Waxing Gibbous",
  "Waxing Gibbous",
  "Waxing Gibbous",
  "Waxing Gibbous",
  "Full Moon",
  "Waning Gibbous",
  "Waning Gibbous",
  "Waning Gibbous",
  "Waning Gibbous",
  "Waning Gibbous",
  "Waning Gibbous",
  "Last Quarter",
  "Waning Crescent",
  "Waning Crescent",
  "Waning Crescent",
  "Waning Crescent",
  "Waning Crescent",
  "Waning Crescent",
  "Waning Crescent"
};

// Constant to determine when to full update
const int fullRefresh = 30;

// Variable for counting partial refreshes
char refreshes = 0;

// Hich line to start drawing the Dayly forecast
const int dayOffset = 380;

// functions defined below
void alignText(const char align, const char *text, int16_t x, int16_t y);
void drawForecast();
void drawCurrent();
void drawTime();
void drawHourly();
void drawMoon();
float getMoonPhase(time_t tdate);

char Output[200] = {0};

OpenWeatherOneCall OWOC; // Invoke OpenWeather Library
time_t t = now();

void connectWifi() {

  int ConnectCount = 20;

  if (WiFi.status() != WL_CONNECTED)
  {
    while (WiFi.status() != WL_CONNECTED) {
      if (ConnectCount++ == 20)
      {
        Serial.println("Connect WiFi");
        WiFi.begin(HOMESSID, HOMEPW);
        Serial.print("Connecting.");
        ConnectCount = 0;
      }
      Serial.print(".");
      delay(1000);
    }
    Serial.print("\nConnected to: ");
    Serial.println(HOMESSID);
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("Connected WiFi");
  }
} //======================== END WIFI CONNECT =======================

void GetCurrentWeather() {
  //=================================
  // Get the Weather Forecast
  //=================================

  connectWifi();

  Serial.println("Getting weather");
  OWOC.parseWeather(ONECALLKEY, NULL, myLatitude, myLongitude, metric, NULL);
  setTime(OWOC.current.dt);
  t = now();

  Serial.println(OWOC.current.timezone_offset);

  //=================================================
  // Today's results
  //==================l===============================
  sprintf(Output, "%s: %02d:%02d,%02d:%02d-%02d:%02d,%02.01fC,%02.01fC,%04.0fhPa,%02.01f%% Rh,%02.01fC,%03d%%,UV:%02.01f,%02dkm,%02.01f%m/s,%02.01f%m/s,%03ddeg,%02.02fmm,%02.02fmm,id:%03d,%s,%s,%s", \
          dayShortStr(weekday(t)), \
          hour(t), \
          minute(t), \
          hour(OWOC.current.sunrise), \
          minute(OWOC.current.sunrise), \
          hour(OWOC.current.sunset), \
          minute(OWOC.current.sunset), \
          OWOC.current.temp, \
          OWOC.current.feels_like, \
          OWOC.current.pressure, \
          OWOC.current.humidity, \
          OWOC.current.dew_point, \
          OWOC.current.clouds, \
          OWOC.current.uvi, \
          OWOC.current.visibility / 1000, \
          OWOC.current.wind_speed, \
          OWOC.current.wind_gust, \
          OWOC.current.wind_deg, \
          OWOC.current.rain_1h, \
          OWOC.current.snow_1h,
          OWOC.current.id, \
          OWOC.current.main, \
          OWOC.current.description, \
          OWOC.current.icon);
  Serial.println(Output);
  Serial.println("");

  Serial.println("Minutely Forecast:");
  for (int minutey = 0; minutey < (sizeof(OWOC.minutely) / sizeof(OWOC.minutely[0])); minutey++) {
    sprintf(Output, "%02d:%02d:%02.02fmm,", \
            hour(OWOC.minutely[minutey].dt), \
            minute(OWOC.minutely[minutey].dt), \
            OWOC.minutely[minutey].precipitation);
    Serial.print(Output);
  }
  Serial.println("");
  Serial.println("");

  Serial.println("Hourly Forecast:");
  for (int Houry = 0; Houry < (sizeof(OWOC.hourly) / sizeof(OWOC.hourly[0])); Houry++) {
    sprintf(Output, "%02d:%02d:%02.02fC,%02.02fC,%04.0fhPa,%02.01f%% Rh,%02.02fC,%03d%%,%02dkm,%02.01f%m/s,%02.01f%m/s,%03ddeg,%03.00f%%,%02.02fmm,%02.02fmm,%03d,%s,%s,%s", \
            hour(OWOC.hourly[Houry].dt), \
            minute(OWOC.hourly[Houry].dt), \
            OWOC.hourly[Houry].temp, \
            OWOC.hourly[Houry].feels_like, \
            OWOC.hourly[Houry].pressure, \
            OWOC.hourly[Houry].humidity, \
            OWOC.hourly[Houry].dew_point, \
            OWOC.hourly[Houry].clouds, \
            OWOC.hourly[Houry].visibility / 1000, \
            OWOC.hourly[Houry].wind_speed, \
            OWOC.hourly[Houry].wind_gust, \
            OWOC.hourly[Houry].wind_deg, \
            OWOC.hourly[Houry].pop * 100, \
            OWOC.hourly[Houry].rain_1h, \
            OWOC.hourly[Houry].snow_1h, \
            OWOC.hourly[Houry].id, \
            OWOC.hourly[Houry].main, \
            OWOC.hourly[Houry].description, \
            OWOC.hourly[Houry].icon);
    Serial.println(Output);
  }
  Serial.println("");

  Serial.println("7 Day Forecast:");
  for (int y = 0; y < (sizeof(OWOC.forecast) / sizeof(OWOC.forecast[0])); y++) {
    sprintf(Output, "%s:%02d:%02d-%02d:%02d,%02.01fC,%02.01fC,%02.01fC,%02.01fC,%02.01fC,%02.01fC,%02.01fC,%02.01fC,%02.01fC,%02.01fC,%04.0fhPa,%02.01f%% Rh,%02.01fC,%02.01f%m/s,%02.01f%m/s,%03ddeg,%03d%%,UV:%02.01f,%02dkm,%03.0f%%,%02.02fmm,%02.02fmm,%03d,%s,%s,%s", \
            dayShortStr(weekday(OWOC.forecast[y].dt)), \
            hour(OWOC.forecast[y].sunrise), \
            minute(OWOC.forecast[y].sunrise), \
            hour(OWOC.forecast[y].sunset), \
            minute(OWOC.forecast[y].sunset), \
            OWOC.forecast[y].temp_morn, \
            OWOC.forecast[y].temp_day, \
            OWOC.forecast[y].temp_eve, \
            OWOC.forecast[y].temp_night, \
            OWOC.forecast[y].temp_min, \
            OWOC.forecast[y].temp_max, \
            OWOC.forecast[y].feels_like_morn, \
            OWOC.forecast[y].feels_like_day, \
            OWOC.forecast[y].feels_like_eve, \
            OWOC.forecast[y].feels_like_night, \
            OWOC.forecast[y].pressure, \
            OWOC.forecast[y].humidity, \
            OWOC.forecast[y].dew_point, \
            OWOC.forecast[y].wind_speed, \
            OWOC.forecast[y].wind_gust, \
            OWOC.forecast[y].wind_deg, \
            OWOC.forecast[y].clouds, \
            OWOC.forecast[y].uvi, \
            OWOC.forecast[y].visibility / 1000, \
            OWOC.forecast[y].pop * 100, \
            OWOC.forecast[y].rain, \
            OWOC.forecast[y].snow, \
            OWOC.forecast[y].id, \
            OWOC.forecast[y].main, \
            OWOC.forecast[y].description, \
            OWOC.forecast[y].icon);
    Serial.println(Output);
  }

  if (OWOC.alerts.start > 100000)
  {
    //=================================================
    // Alerts
    //==================l===============================
    sprintf(Output, "%s,%s,%02d:%02d - %02d:%02d,%s", \
            OWOC.alerts.sender_name, \
            OWOC.alerts.event, \
            hour(OWOC.alerts.start), \
            minute(OWOC.alerts.start), \
            hour(OWOC.alerts.end), \
            minute(OWOC.alerts.end), \
            OWOC.alerts.description);
    Serial.println(Output);
  }
}

void setup()
{
  // Begin serial and display
  Serial.begin(115200);
  while (!Serial) {
    ;
  }
  Serial.println("Serial Monitor Initialized");

  display.begin();

  // Initial cleaning of buffer and physical screen
  display.clearDisplay();
  display.clean();


  // Welcome screen
  display.setCursor(50, 290);
  display.setTextSize(3);
  display.print(F("Welcome to Wol Inkplate 6 weather example!"));
  display.display();
  Serial.println("Welcome to Wol Inkplate 6 weather example!");
  display.display();

  connectWifi();
}

void loop()
{
  // Clear display
  t = now();

  if ((minute(t) % 30) == 0) // Also returns 0 when time isn't set
  {
    GetCurrentWeather();
    display.clearDisplay();
    drawForecast();
    drawCurrent();
    drawHourly();
    drawTime();
    drawMoon();
    display.display();
  }
  else
  {
    drawTime();
    display.partialUpdate();
  }

  // wait for the turn of the minute before sleeping
  while (second(now()) != 0) {}

  // Go to sleep before checking again
  esp_sleep_enable_timer_wakeup(1000L * DELAY_MS);
  (void)esp_light_sleep_start();
  ++refreshes;
}

// Function for drawing weather info
void alignText(const char align, const char *text, int16_t x, int16_t y)
{
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  switch (align) {
    case CENTRE_BOT:
    case CENTRE:
    case CENTRE_TOP:
      x1 = x - w / 2;
      break;
    case RIGHT_BOT:
    case RIGHT:
    case RIGHT_TOP:
      x1 = x - w;
      break;
    case LEFT_BOT:
    case LEFT:
    case LEFT_TOP:
    default:
      x1 = x;
      break;
  }
  switch (align) {
    case CENTRE_BOT:
    case RIGHT_BOT:
    case LEFT_BOT:
      y1 = y;
      break;
    case CENTRE:
    case RIGHT:
    case LEFT:
      y1 = y + h / 2;
      break;
    case CENTRE_TOP:
    case LEFT_TOP:
    case RIGHT_TOP:
    default:
      y1 = y + h;
      break;
  }
  display.setCursor(x1, y1);
  display.print(text);
}

// Function for drawing weather info
void drawForecast()
{
  int dayPos;
  int xOffset = 10;
  int startDay = 1;
  int numOfDays = (sizeof(OWOC.forecast) / sizeof(OWOC.forecast[0]));
  int dayPitch = E_INK_WIDTH / (numOfDays - startDay);
  for (int day = startDay; day < numOfDays; day++) {
    dayPos = (day - startDay) * dayPitch;
    int textCentre = dayPos + (dayPitch / 2);
    sprintf(Output, "http://openweathermap.org/img/wn/%s@2x.png", OWOC.forecast[day].icon);
    display.drawImage(Output, dayPos + 10, dayOffset, true, true);
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);
    display.setFont(&Roboto_Light_36);
    sprintf(Output, "%s", dayShortStr(weekday(OWOC.forecast[day].dt)));
    alignText(CENTRE_BOT , Output, textCentre, dayOffset + 15);

    display.setFont(&FreeSans9pt7b);
    sprintf(Output, "%02.0fC/%02.0fC", \
            OWOC.forecast[day].temp_min, \
            OWOC.forecast[day].temp_max);
    alignText(CENTRE_BOT , Output, textCentre, dayOffset + 100);

    sprintf(Output, "%02.0f%%", \
            OWOC.forecast[day].pop * 100);
    alignText(CENTRE_BOT , Output, textCentre, dayOffset + 120);

    sprintf(Output, "%02.01fmm", \
            OWOC.forecast[day].rain + OWOC.forecast[day].snow);
    alignText(CENTRE_BOT , Output, textCentre, dayOffset + 140);

    sprintf(Output, "%02d:%02d-%02d:%02d", \
            hour(OWOC.forecast[day].sunrise), \
            minute(OWOC.forecast[day].sunrise), \
            hour(OWOC.forecast[day].sunset), \
            minute(OWOC.forecast[day].sunset)); \
    alignText(CENTRE_BOT , Output, textCentre, dayOffset + 160);

    sprintf(Output, "%04.0fhPa", \
            OWOC.forecast[day].pressure);
    alignText(CENTRE_BOT , Output, textCentre, dayOffset + 180);

    sprintf(Output, "%03.0f%% Rh", \
            OWOC.forecast[day].humidity);
    alignText(CENTRE_BOT , Output, textCentre, dayOffset + 200);
  }
}

// Function for drawing current time
void drawTime()
{
  int textWidth;
  char Output2[200] = {0};

  t = now();
  // Drawing current time

  display.setTextColor(BLACK, WHITE);
  display.setFont(&Roboto_Light_120);
  display.setTextSize(1);

  display.fillRect(360, 0, 440, 170, WHITE);
  sprintf(Output, "%02d:%02d", hour(t), minute(t));
  alignText(CENTRE_BOT , Output, 560, 110);
  Serial.print(Output);
  Serial.print(" ");

  display.setFont(&Roboto_Light_48);
  sprintf(Output, "%s %02d ", \
          dayShortStr(weekday(t)) + 0, \
          day()); \
  sprintf(Output2, "%s %04d", \
          monthShortStr(month(t)) + 0, \
          year());
  strcat(Output, Output2);
  alignText(CENTRE_BOT , Output, 560, 168);

  Serial.println(Output);
}

void drawHourly()
{
  const int yTop = 180;
  const int yHeight = 150;
  const int xLeft = 240;
  const int xWidth = 480;
  const int hoursDisplay = 24;
  const int hourPitch = xWidth / hoursDisplay;
  const float minPrec = 0;
  float maxPrec = 1;
  float maxTemp = -100;
  float minTemp = 100;

  for (int Houry = 0; Houry < hoursDisplay; Houry++) {
    float thisPrec = OWOC.hourly[Houry].rain_1h + OWOC.hourly[Houry].snow_1h;
    if (maxPrec < thisPrec)
      maxPrec = thisPrec;
    if (maxTemp < OWOC.hourly[Houry].temp)
      maxTemp = OWOC.hourly[Houry].temp;
    if (minTemp > OWOC.hourly[Houry].temp)
      minTemp = OWOC.hourly[Houry].temp;
  }

  for (int Houry = 0; Houry <= hoursDisplay; Houry++) {
    display.drawLine( xLeft + (Houry * hourPitch), yTop, xLeft + (Houry * hourPitch), yTop + yHeight, BLACK );
    if (Houry % 2)
    {
      sprintf(Output, "%2d", hour(OWOC.hourly[Houry].dt));
      alignText(CENTRE_TOP, Output, xLeft + (Houry * hourPitch), yTop + yHeight + 2);
    }
  }

  display.drawLine( xLeft, yTop + yHeight, xLeft + xWidth, yTop + yHeight, BLACK );
  display.drawLine( xLeft, yTop, xLeft + xWidth, yTop, BLACK );

  sprintf(Output, "%02.0fC", (minTemp - 0.499));
  alignText(RIGHT, Output, xLeft - 5, yTop + yHeight);
  sprintf(Output, "%02.0fC", (maxTemp + .5));
  alignText(RIGHT, Output, xLeft - 5, yTop);

  sprintf(Output, "%02.0f mm", minPrec);
  alignText(LEFT, Output, xLeft + xWidth + 5, yTop + yHeight);
  sprintf(Output, "%02.0f mm", (maxPrec + .499));
  alignText(LEFT, Output, xLeft + xWidth + 5, yTop);

  float yTempScale = (yHeight / (round(maxTemp + 0.499) - round(minTemp - 0.5)));

  for (int Houry = 0; Houry <= (round(maxTemp + 0.499) - round(minTemp - 0.5)); Houry++) {
    display.drawLine( xLeft, yTop + (Houry * yTempScale), xLeft + xWidth, yTop + (Houry * yTempScale), BLACK );
  }

  for (int Houry = 0; Houry <= (hoursDisplay - 1); Houry++) {
    display.drawThickLine(xLeft + (Houry * hourPitch), yTop + (int) (yTempScale * (round(maxTemp + 0.499) - (OWOC.hourly[Houry].temp ))), \
                          xLeft + ((Houry + 1) * hourPitch), yTop + (int) (yTempScale * (round(maxTemp + 0.499) - (OWOC.hourly[Houry + 1].temp ))), BLACK, 3);
    float yPrecScale = (yHeight / (round(maxPrec + 0.499)));
    float thisPrec = OWOC.hourly[Houry].rain_1h + OWOC.hourly[Houry].snow_1h;
    display.fillRect(xLeft + (Houry * hourPitch) + round(hourPitch / 3), yTop + (int) (yPrecScale * (round(maxPrec + .499) - thisPrec)), \
                     round(hourPitch / 3), (int) (yPrecScale * thisPrec), BLACK);
  }
}

// Current weather drawing function
void drawCurrent()
{
  sprintf(Output, "http://openweathermap.org/img/wn/%s@4x.png", OWOC.current.icon);
  display.drawImage(Output, 0, 0, true, true);

  display.setFont(&Roboto_Light_48);
  sprintf(Output, "%02.01fC", \
          OWOC.current.temp);
  alignText(CENTRE_BOT , Output, 100, 190);

  display.setFont(&Roboto_Light_36);
  sprintf(Output, "%02.0fC/%02.0fC", \
          OWOC.forecast[0].temp_min, \
          OWOC.forecast[0].temp_max);
  alignText(CENTRE_BOT , Output, 100, 230);

  display.setFont(&FreeSans12pt7b);
  sprintf(Output, "%02d:%02d-%02d:%02d", \
          hour(OWOC.current.sunrise), \
          minute(OWOC.current.sunrise), \
          hour(OWOC.current.sunset), \
          minute(OWOC.current.sunset)); \
  alignText(CENTRE_BOT , Output, 100, 260);

  sprintf(Output, "%04.0fhPa", \
          OWOC.current.pressure);
  alignText(CENTRE_BOT , Output, 100, 290);

  sprintf(Output, "%03.0f%% Rh", \
          OWOC.current.humidity);
  alignText(CENTRE_BOT , Output, 100, 320);
}

/*
  return value is percent of moon cycle ( from 0.0 to 0.999999), i.e.:

  0.0: New Moon
  0.125: Waxing Crescent Moon
  0.25: Quarter Moon
  0.375: Waxing Gibbous Moon
  0.5: Full Moon
  0.625: Waning Gibbous Moon
  0.75: Last Quarter Moon
  0.875: Waning Crescent Moon

*/
float getMoonPhase(time_t tdate)
{
  time_t newmoonref = 1263539460; //known new moon date (2010-01-15 07:11)
  // moon phase is 29.5305882 days, which is 2551442.82048 seconds
  float phase = abs( tdate - newmoonref) / (double)2551442.82048;
  phase -= (int)phase; // leave only the remainder
  if (newmoonref > tdate)
    phase = 1 - phase;
  return phase;
}

// Function for drawing the moon
void drawMoon()
{
  const int MoonCentreX = 270;
  const int MoonCentreY = 70;
  const int MoonBox = 35;
  float moonphase = getMoonPhase(now());
  int moonage = 29.5305882 * moonphase;
  //Serial.println("moon age: " + String(moonage));
  // convert to appropriate icon
  display.setFont(&moon_phases36pt7b);
  sprintf(Output, "%c", (char)((int)'A' + (int)(moonage * 25. / 30)));
  alignText(CENTRE , Output, MoonCentreX, MoonCentreY);

  display.setFont(&FreeSans12pt7b);
  sprintf(Output, "%02d days old", moonage);
  alignText(CENTRE_TOP , Output, MoonCentreX, MoonCentreY + MoonBox - 5);
  int currentphase = moonphase * 28. + .5;
  alignText(CENTRE_TOP , moonphasenames[currentphase], MoonCentreX, MoonCentreY + MoonBox + 20);
}

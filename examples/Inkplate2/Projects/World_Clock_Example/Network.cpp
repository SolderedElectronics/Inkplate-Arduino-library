/*
  Network.cpp
  Inkplate 6 Arduino library
  David Zovko, Borna Biro, Denis Vajak, Zvonimir Haramustek @ e-radionica.com
  September 24, 2020
  https://github.com/e-radionicacom/Inkplate-6-Arduino-library

  For support, please reach over forums: forum.e-radionica.com/en
  For more info about the product, please check: www.inkplate.io

  This code is released under the GNU Lesser General Public License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html
  Please review the LICENSE file included with this example.
  If you have any questions about licensing, please contact techsupport@e-radionica.com
  Distributed as-is; no warranty is given.
*/

#include "Network.h"

#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#include "Inkplate.h"

// Must be installed for this example to work
#include <ArduinoJson.h>

// external parameters from our main file
extern char ssid[];
extern char pass[];
extern const char *city1_name;
extern const char *city2_name;

// Get our Inkplate object from main file to draw debug info on
extern Inkplate display;

// Static Json from ArduinoJson library
StaticJsonDocument<30000> doc;

void Network::begin()
{
  // Initiating wifi, like in BasicHttpClient example
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  int cnt = 0;
  Serial.print(F("Waiting for WiFi to connect..."));
  while ((WiFi.status() != WL_CONNECTED))
  {
    Serial.print(F("."));
    delay(1000);
    ++cnt;

    if (cnt == 20)
    {
      Serial.println("Can't connect to WIFI, restarting");
      delay(100);
      ESP.restart();
    }
  }
  Serial.println(F(" connected"));

  // Find internet time
  setTime();
}

// Gets time from ntp server
void Network::getTime(tm *t)
{
  // Get seconds since 1.1.1970.
  time_t nowSecs = time(nullptr) + 3600ULL * timeZone;

  // Used to store time
  struct tm timeinfo;
  gmtime_r(&nowSecs, t);

}

bool Network::getData(char* city, tm *t)
{
  bool f = 0;

  // If not connected to wifi reconnect wifi
  if (WiFi.status() != WL_CONNECTED)
  {
    WiFi.reconnect();

    delay(5000);

    int cnt = 0;
    Serial.println(F("Waiting for WiFi to reconnect..."));
    while ((WiFi.status() != WL_CONNECTED))
    {
      // Prints a dot every second that wifi isn't connected
      Serial.print(F("."));
      delay(1000);
      ++cnt;

      if (cnt == 7)
      {
        Serial.println("Can't connect to WIFI, restart initiated.");
        delay(100);
        ESP.restart();
      }
    }
  }

  // Wake up if sleeping and save inital state
  bool sleep = WiFi.getSleep();
  WiFi.setSleep(false);

  // Http object used to make get request
  HTTPClient http;

  http.getStream().setTimeout(10);
  http.getStream().flush();

  char temp[120];
  for (int i = 0; i < sizeof(cities); i++)
  {
    if (strstr(cities[i], city))
    {
      sprintf(temp, "https://www.timeapi.io/api/Time/current/zone?timeZone=%s", cities[i]);
      Serial.println(cities[i]);
      if(city1_name == NULL)
        city1_name = cities[i];
      else
        city2_name = cities[i];
      break;
    }
  }
  if (sizeof(temp) < 20)
  {
    Serial.println("City not found");
    return 0;
  }

  // Initiate http
  http.begin(temp);

  // Actually do request
  int httpCode = http.GET();
  if (httpCode == 200)
  {
    while (http.getStream().available() && http.getStream().peek() != '{')
      (void)http.getStream().read();

    // Try parsing JSON object
    DeserializationError error = deserializeJson(doc, http.getStream());

    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      f = 1;
    }
    else
    {
      // Set all data got from internet using formatTemp and formatWind defined above
      // This part relies heavily on ArduinoJson library

      Serial.println("Success");

      t->tm_year = doc["year"];
      t->tm_mon = doc["month"];
      t->tm_mday = doc["day"];
      t->tm_hour = doc["hour"];
      t->tm_min = doc["minute"];
      t->tm_sec = doc["seconds"];

      // Save our data to data pointer from main file
      f = 0;
    }
  }
  else if (httpCode == 404)
  {
    // Coin id not found
    display.clearDisplay();
    display.setCursor(10, 10);
    display.setTextSize(2);
    display.println(F("Time has not been fetched!"));
    display.display();
    while (1)
      ;
  }
  else
  {
    f = 1;
  }

  // Clear document and end http
  doc.clear();
  http.end();

  // Return to initial state
  WiFi.setSleep(sleep);

  return !f;
}

// Function for initial time setting ovet the ntp server
void Network::setTime()
{
  // Used for setting correct time
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print(F("Waiting for NTP time sync: "));
  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2)
  {
    delay(500);
    Serial.print(F("."));
    yield();
    nowSecs = time(nullptr);
  }

  Serial.println();

  // Used to store time info
  struct tm timeinfo;
  gmtime_r(&nowSecs, &timeinfo);

  Serial.print(F("Current time: "));
  Serial.print(asctime(&timeinfo));
}

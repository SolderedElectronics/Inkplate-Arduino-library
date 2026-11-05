/**
 **************************************************
 * @file        Inkplate10_Weather.ino
 * @brief       Current weather over Wi-Fi example for Soldered
 *              Inkplate 10.
 *
 * @details     Shows the current weather in a city on the Inkplate 10
 *              using the free Open-Meteo service. The sketch first
 *              looks up the position of the city, then asks for the
 *              weather at that position and reads the JSON answer
 *              with the ArduinoJson library.
 *
 *              Enter the name and the password of your Wi-Fi network
 *              in the two #define lines. Both requests use HTTPS with
 *              client.setInsecure(), which skips the certificate
 *              check; that is fine for public weather data but wrong
 *              for anything private. Expected output is the city, the
 *              temperature and a short description of the weather.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 10
 * - Hardware:   Inkplate 10, USB cable
 * - Extra:      Wi-Fi network, ArduinoJson library, SourceSans3_24.h font file in the sketch folder
 * - Serial:     115200 baud (optional)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered
 *    Inkplate10" from Tools -> Board.
 * 2) Install the ArduinoJson library from the Library Manager.
 * 3) Enter your Wi-Fi name and password, then upload the sketch to
 *    Inkplate 10.
 * 4) The weather appears on the display.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/10/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Borna Zelenika @ soldered.com
 * @date        2026-11-05
 * @license     GNU GPL V3
 **************************************************/

// Pull in the libraries: Inkplate, the font file that sits next to this sketch, Wi-Fi, web requests and JSON
#include "Inkplate.h"
#include "SourceSans3_24.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// The name and the password of your Wi-Fi network
#define ssid ""
#define pass ""

// The city to show the weather for. Use only its name, without spaces.
const char *city = "Osijek";

// Black and white mode: a colour is BLACK or WHITE
Inkplate display(INKPLATE_1BIT);

// Turn the weather code of Open-Meteo into a few words
const char *describe(int code) {
  if (code == 0) return "Clear sky";
  if (code <= 3) return "Partly cloudy";
  if (code <= 48) return "Fog";
  if (code <= 57) return "Drizzle";
  if (code <= 67) return "Rain";
  if (code <= 77) return "Snow";
  if (code <= 82) return "Rain showers";
  if (code <= 86) return "Snow showers";
  return "Thunderstorm";
}

// Stop with a message on the screen
void fail(const char *message) {
  display.println(message);
  display.display();
  while (true);
}

// Ask a web address for JSON and read the answer into the document. Returns false if
// anything goes wrong. The address starts with https, so the connection is encrypted.
// setInsecure() skips checking the server's certificate, which is fine for public weather
// data but not for anything private.
bool getJson(String address, JsonDocument &document) {
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  http.begin(client, address);
  if (http.GET() != 200) {
    http.end();
    return false;
  }

  // The answer is JSON text. Turn it into something you can pick values out of.
  DeserializationError error = deserializeJson(document, http.getString());
  http.end();
  return !error;
}

// setup() runs once, right after the board powers on or resets
void setup() {
  display.begin();
  display.clearDisplay();
  display.setFont(&SourceSans3_24);
  display.setTextColor(BLACK, WHITE);
  display.setTextSize(2);
  display.setCursor(50, 50);

  // Join the Wi-Fi network
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  // Ask Open-Meteo's search for the position of the city
  JsonDocument place;
  if (!getJson("https://geocoding-api.open-meteo.com/v1/search?count=1&name=" + String(city), place)) {
    fail("Could not look up the city");
  }
  if (place["results"].isNull()) {
    fail("City not found");
  }
  float latitude = place["results"][0]["latitude"];
  float longitude = place["results"][0]["longitude"];

  // Ask Open-Meteo for the current weather at that position
  JsonDocument weather;
  String weatherUrl = "https://api.open-meteo.com/v1/forecast?latitude=" + String(latitude, 4) +
                      "&longitude=" + String(longitude, 4) +
                      "&current=temperature_2m,relative_humidity_2m,wind_speed_10m,weather_code&timezone=auto";
  if (!getJson(weatherUrl, weather)) {
    fail("Could not get the weather");
  }

  // Pick out the values we want
  float temperature = weather["current"]["temperature_2m"];
  int humidity = weather["current"]["relative_humidity_2m"];
  float wind = weather["current"]["wind_speed_10m"];
  int code = weather["current"]["weather_code"];

  // Draw the card: one line for each value, all in the same size
  display.clearDisplay();
  display.setTextSize(4);

  display.setCursor(80, 120);
  display.print(city);

  display.setCursor(80, 250);
  display.print("Temperature: ");
  display.print(temperature, 1);
  display.print(" C");

  display.setCursor(80, 380);
  display.print(describe(code));

  display.setCursor(80, 510);
  display.print("Humidity: ");
  display.print(humidity);
  display.print(" %");

  display.setCursor(80, 640);
  display.print("Wind: ");
  display.print(wind, 1);
  display.print(" km/h");

  // Send the frame buffer to the screen
  display.display();
}

// loop() runs over and over. This sketch has nothing to repeat.
void loop() {
}

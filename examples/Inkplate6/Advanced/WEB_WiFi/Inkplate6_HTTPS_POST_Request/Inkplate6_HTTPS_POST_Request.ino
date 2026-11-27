/**
 **************************************************
 * @file        Inkplate6_HTTPS_POST_Request.ino
 * @brief       HTTPS POST request example for Soldered
 *              Inkplate 6.
 *
 * @details     Send the temperature of the Inkplate 6 to a
 *              test server in a POST request, and show the
 *              answer. Connection: none. Needs a Wi-Fi
 *              network.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6
 * - Hardware:   Inkplate 6, USB cable, Wi-Fi network
 * - Extra:      Wi-Fi network and the ArduinoJson library
 * - Serial:     Not required
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate6"
 *    from Tools -> Board.
 * 2) Enter the name and the password of your Wi-Fi network in the
 *    ssid and pass lines at the top of the sketch.
 * 3) Upload the sketch to Inkplate 6.
 * 4) The answer of the server appears on the display.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/6/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Borna Zelenika @ soldered.com
 * @date        2026-11-26
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_ESP32_DEV) && !defined(ARDUINO_INKPLATE6V2)
#error "Wrong board selection for this example, please select e-radionica Inkplate6 or Soldered Inkplate6 in the boards menu."
#endif

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

// The test server. It accepts what you send and answers as if it had saved it, but it keeps nothing.
const char *serverUrl = "https://jsonplaceholder.typicode.com/posts";

// Black and white mode: a colour is BLACK or WHITE
Inkplate display(INKPLATE_1BIT);

// Stop with a message on the screen
void fail(const char *message) {
  display.println(message);
  display.display();
  while (true);
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

  // Read the temperature of the board. It is a whole number of degrees Celsius.
  int temperature = display.readTemperature();

  // Build the message to send: a JSON document with two names and their values
  JsonDocument message;
  message["title"] = "Inkplate temperature";
  message["body"] = String(temperature) + " C";
  String json;
  serializeJson(message, json);

  // Send it with a POST request. The header tells the server that the body is JSON.
  // setInsecure() skips checking the server's certificate, which is fine for a test server
  // but wrong for anything private.
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  http.begin(client, serverUrl);
  http.addHeader("Content-Type", "application/json");
  int status = http.POST(json);

  // A status code below 0 means the request never reached the server
  if (status < 0) {
    http.end();
    fail("Could not reach the server");
  }

  // Read the answer: the server sends back what it received, and adds an id of its own
  JsonDocument answer;
  DeserializationError error = deserializeJson(answer, http.getString());
  http.end();
  if (error) {
    fail("The answer was not JSON");
  }
  int id = answer["id"];

  // Draw the result: one line for each value, all in the same size
  display.clearDisplay();
  display.setTextSize(3);

  display.setCursor(60, 100);
  display.print("POST request");

  display.setCursor(60, 200);
  display.print("Sent: ");
  display.print(temperature);
  display.print(" C");

  display.setCursor(60, 300);
  display.print("Status: ");
  display.print(status);

  display.setCursor(60, 400);
  display.print("Server id: ");
  display.print(id);

  // Send the frame buffer to the screen
  display.display();
}

// loop() runs over and over. This sketch has nothing to repeat.
void loop() {
}

/**
 **************************************************
 * @file        Inkplate13SPECTRA_HTTP_POST_ThingSpeak.ino
 * @brief       HTTP POST request to ThingSpeak from Soldered Inkplate 13SPECTRA.
 *
 * @details     Shows how to connect to a WiFi network and send a POST request
 *              via HTTP. The example uses the ThingSpeak API, a free API that
 *              allows you to store and retrieve data using HTTP, to inspect the
 *              POST requests.
 *
 *              To set up ThingSpeak:
 *              1. Go to ThingSpeak.com and create a free account.
 *              2. Open the Channels tab.
 *              3. Create a new channel.
 *              4. Create the fields you want to use (this example uses 1 field
 *                 called field1 and this name must be used when sending data).
 *              5. Open the channel, go to the API Keys tab and copy your Write
 *                 API Key.
 *              6. Enter your API key in the code below.
 *
 *              When you send a POST request, open your channel and you will see
 *              the graph with your sent data.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 13SPECTRA
 * - Hardware:   Inkplate 13SPECTRA, USB cable
 * - Extra:      Stable WiFi Internet connection, ThingSpeak account and Write
 *               API Key
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 13SPECTRA"
 *    from Tools -> Board.
 * 2) Create the ThingSpeak channel and field1 as described above and copy the
 *    Write API Key.
 * 3) Enter your WiFi credentials and API key in the sketch.
 * 4) Upload the sketch to Inkplate 13SPECTRA.
 * 5) Open your ThingSpeak channel to see the posted data on the graph.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/13spectra/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered Electronics
 * @date        2026-01-21
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE13SPECTRA
#error "Wrong board selection for this example, please select Soldered Inkplate 13SPECTRA in the boards menu."
#endif

// Include needed libraries
#include "Inkplate.h"
#include "WiFi.h"

// Create objects from included libraries
Inkplate display;
WiFiClient client;

// Here you can change the interval of sending POST requests (minimum 15 seconds with a free license)
#define POSTING_INTERVAL_IN_SESCS 20

// Enter your WiFi credentials
const char *ssid = "";
const char *pass = "";

// ThingSpeak settings
char *server = "api.thingspeak.com";
String writeAPIKey = ""; // Enter your Write API Key

// Variable that holds last connection time
unsigned long lastConnectionTime = 0;

void setup()
{
  // Init serial communication
  Serial.begin(115200);

  // Init Inkplate library (you should call this function ONLY ONCE)
  display.begin();

  // Clear frame buffer of display
  display.clearDisplay();

  // Set text color and size
  display.setTextColor(INKPLATE_BLACK);
  display.setTextSize(4);

  // Display a message
  display.printf("HTTP POST request example\n\n");
  display.printf("Open Serial Monitor at \n115200 baud rate to see \nwhat's happening.");
  display.display();

  // Connect to the WiFi network
  WiFi.mode(WIFI_MODE_STA);
  WiFi.begin(ssid, pass);
  Serial.print("Connecting to Wifi ");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
}


void loop()
{
  // Every POSTING_INTERVAL_IN_SESCS seconds make the POST request
  if ((unsigned long)(millis() - lastConnectionTime) > POSTING_INTERVAL_IN_SESCS * 1000LL)
  {
    // Clear frame buffer of display
    display.clearDisplay();

    // Connect the WiFi client to the server via port 80
    if (!client.connect(server, 80))
    {
      // If it fails, print a message, remember time, stop the client and reset the loop
      Serial.println("Connection failed");
      lastConnectionTime = millis();
      client.stop();
      return;
    }
    else
    {
      // If you have any sensor or something else, here you have to put data to send instead of a random number
      int field1Data = random(40);

      // Create data string to send to ThingSpeak
      String data = "field1=" + String(field1Data); // shows how to include additional field data in http post

      // POST data to ThingSpeak
      if (client.connect(server, 80))
      {
        client.println("POST /update HTTP/1.1");
        client.println("Host: api.thingspeak.com");
        client.println("Connection: close");
        client.println("User-Agent: ESP32WiFi/1.1");
        client.println("X-THINGSPEAKAPIKEY: " + writeAPIKey);
        client.println("Content-Type: application/x-www-form-urlencoded");
        client.print("Content-Length: ");
        client.print(data.length());
        client.print("\n\n");
        client.print(data);

        Serial.print("The POST request is done: ");
        Serial.println(data);
        lastConnectionTime = millis();
        delay(250);
      }
    }
    client.stop();
  }
}
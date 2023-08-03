/*
   Inkplate6COLOR_HTTPS_POST_Request example for Soldered Inkplate 6COLOR
   For this example you will need USB cable, Inkplate 6COLOR and stable WiFi Internet connection.
   Select "Soldered Inkplate 6COLOR" from Tools -> Board menu.
   Don't have "Soldered Inkplate 6COLOR" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example will show you how to connect to a WiFi network and send a POST request via HTTPS.
   We will use JSONPlaceholder fake API for testing to see responses when sending post requests.
   For real API, you must specify the API key and maybe some more parameters to work correctly.
   When you send a POST request, on the Serial Monitor you will see a response like on the real
   API but data won't be written on the API.

   NOTE: Open the Serial Monitor at 115200 baud rate to see what's happening!

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   2 February 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
#endif

// Include needed libraries
#include "ArduinoJson.h"
#include "HTTPClient.h"
#include "Inkplate.h"
#include "WiFi.h"
#include <WiFiClientSecure.h>

// Enter your WiFi credentials
const char *ssid = "";
const char *pass = "";

// Specify the API URL to send a POST request
const char *apiUrl = "https://jsonplaceholder.typicode.com/posts";

// Specify the delay time between 2 POST requests in milliseconds
#define DELAY_BETWEEN_REQUESTS 10000

// Create objects from included libraries
WiFiClientSecure client;
HTTPClient http;
Inkplate display; 

void setup()
{
    // Init serial communication
    Serial.begin(115200);

    // Init the display
    display.begin();
    
    // Connect to WiFi
    WiFi.begin(ssid, pass);
    Serial.println("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
    }
    Serial.println();
    Serial.print("Connected to WiFi with IP address ");
    Serial.println(WiFi.localIP());

    // Use https but don't use a certificate
    client.setInsecure();
}

void loop()
{
    // Init HTTP client with secure wifi client and the API URL
    http.begin(client, apiUrl);

    // Add header. In your case maybe you must add more depending on your API
    http.addHeader("Content-Type", "application/json");

    // Create a JSON document for serializing data
    const size_t CAPACITY = JSON_OBJECT_SIZE(1);
    StaticJsonDocument<CAPACITY> doc;

    // Convert the JSON document to a JSON object in order to add data
    JsonObject object = doc.to<JsonObject>();

    // Add data in the JSON object
    object["title"] = "Hello Inkplate";

    // Serialize the JSON object with your data to a JSON document for sending
    char jsonOutput[128];
    serializeJson(doc, jsonOutput);

    // Send the POST request and save returned code
    int httpCode = http.POST(String(jsonOutput));

    // If the code is smaller than 0, something went wrong
    if (httpCode > 0)
    {
        // Get the string returned by the HTTPS request and print the response
        String payload = http.getString();
        Serial.println("Status code: " + String(httpCode));
        Serial.println(payload);

        // End connection
        http.end();
    }
    else
    {
        Serial.println("Error on HTTPS request");
    }

    // Wait for sending next request
    delay(DELAY_BETWEEN_REQUESTS);
}

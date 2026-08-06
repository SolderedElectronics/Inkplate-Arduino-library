/**
 **************************************************
 * @file        Inkplate2_HTTPS_POST_Request.ino
 * @brief       Send HTTPS POST requests over WiFi and print the HTTP response
 *              to the Serial Monitor.
 *
 * @details     This example demonstrates how to connect Inkplate 2 to a WiFi
 *              network and perform an HTTPS POST request using HTTPClient with
 *              WiFiClientSecure. A small JSON payload is created with
 *              ArduinoJson, sent to a test endpoint (JSONPlaceholder), and the
 *              returned HTTP status code and response body are printed to the
 *              Serial Monitor.
 *
 *              For simplicity, TLS certificate validation is disabled via
 *              client.setInsecure(). This is suitable for demos/testing only and
 *              makes the connection vulnerable to MITM attacks. For production
 *              use, validate the server using a root CA certificate or
 *              certificate pinning that matches the target host. JSONPlaceholder
 *              is a fake test API: it returns realistic responses but does not
 *              persist posted data.
 *
 *              The e-paper display is used only to show basic instructions at
 *              startup. The display runs in 1-bit (black/white) mode and is
 *              refreshed once during setup with a full refresh.
 *
 *              Expected output: "HTTPS POST Request example" and a reminder to
 *              open the Serial Monitor on the display; connection logs followed
 *              by repeated blocks containing the HTTP status code and JSON
 *              response in the Serial Monitor.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 2
 * - Hardware:   Inkplate 2, USB cable
 * - Extra:      WiFi connection + Internet access
 * - Serial:     115200 baud
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate2"
 *    from Tools -> Board.
 * 2) Enter your WiFi SSID and password (ssid, pass).
 * 3) Optionally change apiUrl to your HTTPS endpoint, adjust headers/payload and
 *    set DELAY_BETWEEN_REQUESTS (ms).
 * 4) Upload the sketch and open Serial Monitor at 115200 baud.
 * 5) Every DELAY_BETWEEN_REQUESTS milliseconds, the sketch sends an HTTPS POST
 *    request and prints the status code and response payload.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/2/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2023-01-27
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Soldered Inkplate2 in the boards menu."
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

    // Clear the display and print message
    display.clearDisplay();
    display.setTextColor(INKPLATE2_BLACK);
    display.setTextSize(2);
    display.printf("HTTPS POST \nRequest example\n");

    display.setTextSize(1);
    display.printf("\nOpen Serial Monitor at \n115200 baud rate to see \nwhat's happening");
    display.display();

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

/**
 **************************************************
 * @file        Inkplate6COLOR_HTTPS_POST_Request.ino
 * @brief       Connects Inkplate 6COLOR to Wi-Fi and sends periodic HTTPS POST
 *              requests with a JSON payload.
 *
 * @details     This example demonstrates how to use Inkplate 6COLOR for secure
 *              web API communication over HTTPS. The sketch connects to a Wi-Fi
 *              network, builds a small JSON document with ArduinoJson, and
 *              sends that JSON payload to a remote HTTPS endpoint using
 *              HTTPClient with a WiFiClientSecure transport.
 *
 *              The example uses the JSONPlaceholder test API, which is useful
 *              for learning and debugging POST request workflows because it
 *              returns realistic API-style responses without storing real data.
 *              This makes it a safe demo target for testing request formatting,
 *              headers, and response handling.
 *
 *              In this version, the TLS client uses client.setInsecure(), which
 *              disables certificate validation. That is convenient for demos,
 *              but it should not be used in production or security-sensitive
 *              deployments. For real services, use proper certificate validation
 *              or certificate pinning that matches the target host.
 *
 *              The display is not used in this example (no screen updates
 *              occur). JSON payload size affects RAM usage, so larger requests
 *              may require more careful buffer sizing on embedded targets, and
 *              repeated POST intervals should respect the target API's rate
 *              limits and service policies.
 *
 *              Expected output on Serial: Wi-Fi connection progress, the
 *              assigned IP address, the HTTP status code and the JSON response
 *              returned by the test API.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6COLOR
 * - Hardware:   Inkplate 6COLOR, USB cable
 * - Extra:      Wi-Fi connection
 * - Serial:     115200 baud
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 6COLOR"
 *    from Tools -> Board.
 * 2) Enter your Wi-Fi SSID and password in the sketch. Update the HTTPS API URL
 *    if you want to test against a different endpoint, and add API keys,
 *    authentication headers or extra JSON fields if your real service needs
 *    them.
 * 3) Upload the example to Inkplate 6COLOR.
 * 4) Open Serial Monitor at 115200 baud.
 * 5) The board connects to Wi-Fi and initializes a secure HTTP client.
 * 6) In loop(), the sketch builds a JSON payload and sends an HTTPS POST
 *    request to the configured API endpoint.
 * 7) The HTTP status code and returned response body are printed to Serial, and
 *    the process repeats after the configured delay.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/6color/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2023-02-02
 * @license     GNU GPL V3
 **************************************************/
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

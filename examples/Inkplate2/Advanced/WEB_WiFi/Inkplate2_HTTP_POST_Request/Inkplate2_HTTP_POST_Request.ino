/**
 **************************************************
 * @file        Inkplate2_HTTP_POST_Request.ino
 * @brief       Send periodic HTTP POST requests over WiFi to ThingSpeak and
 *              monitor activity via Serial Monitor.
 *
 * @details     This example demonstrates how to connect Inkplate 2 to a WiFi
 *              network and send HTTP POST requests using a raw WiFiClient TCP
 *              connection. The sketch posts data to the ThingSpeak API
 *              (api.thingspeak.com) at a fixed interval. A sample value is
 *              generated with random() and sent as field1 in an
 *              application/x-www-form-urlencoded payload.
 *
 *              The e-paper display is used only for a short startup message.
 *              All request status and troubleshooting output is sent to the
 *              Serial Monitor. The display runs in 1-bit (black/white) mode and
 *              uses only a full refresh (display()).
 *
 *              This example uses plain HTTP on port 80 (no TLS); for secure
 *              transport, use an HTTPS example with WiFiClientSecure and
 *              certificate validation. The payload uses form encoding, so if you
 *              post multiple fields, append them as additional key=value pairs
 *              (e.g. field1=...&field2=...). ThingSpeak rate limits apply, so
 *              choose an interval that complies with your account/channel limits
 *              to avoid rejected updates.
 *
 *              Expected output: "HTTP POST request example" and a reminder to
 *              open the Serial Monitor on the display; WiFi connection logs and
 *              "The POST request is done." messages after each successful POST
 *              in the Serial Monitor; and the field1 graph updating with the
 *              posted values on ThingSpeak.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 2
 * - Hardware:   Inkplate 2, USB cable
 * - Extra:      WiFi connection + ThingSpeak account/channel + Write API Key
 * - Serial:     115200 baud
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate2"
 *    from Tools -> Board.
 * 2) Create a ThingSpeak account and a channel with at least one field.
 * 3) Copy the channel Write API Key and paste it into writeAPIKey.
 * 4) Enter your WiFi SSID and password (ssid, pass) and set
 *    POSTING_INTERVAL_IN_SESCS (seconds).
 * 5) Upload the sketch and open Serial Monitor at 115200 baud.
 * 6) Every POSTING_INTERVAL_IN_SESCS seconds, the device sends a POST request.
 * 7) Open your ThingSpeak channel to see the incoming data plotted over time.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/2/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2023-01-26
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Soldered Inkplate2 in the boards menu."
#endif

// Include needed libraries
#include "Inkplate.h"
#include "WiFi.h"

// Create objects from included libraries
Inkplate display;
WiFiClient client;

// Here you can change the interval of sending POST requests
#define POSTING_INTERVAL_IN_SESCS 10

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
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(1);

    // Display a message
    display.printf("HTTP POST request example\n\n");
    display.printf("Open Serial Monitor at \n115200 baud rate to see \nwhat's happening");
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
    if (millis() - lastConnectionTime > POSTING_INTERVAL_IN_SESCS * 1000LL)
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

                Serial.println("The POST request is done.");
                lastConnectionTime = millis();
                delay(250);
            }
        }
        client.stop();
    }
}

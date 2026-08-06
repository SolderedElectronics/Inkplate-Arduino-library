/**
 **************************************************
 * @file        Inkplate5V2_HTTP_POST_Request.ino
 * @brief       WiFi HTTP POST request example using webhook.site (Inkplate 5v2).
 *
 * @details     Demonstrates how to connect Inkplate 5v2 to a WiFi network and
 *              send periodic HTTP POST requests to webhook.site. This free
 *              online service allows real-time inspection of HTTP requests,
 *              making it useful for testing IoT data transmission.
 *
 *              The example uses plain HTTP (port 80) for simplicity and sends
 *              data in URL-encoded format
 *              (application/x-www-form-urlencoded); replace the example data
 *              with real sensor readings if needed. Expected output is example
 *              information on the Inkplate display, WiFi connection and POST
 *              status in the Serial Monitor, and incoming POST requests visible
 *              on webhook.site every 20 seconds.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 5v2
 * - Hardware:   Inkplate 5v2, USB cable
 * - Extra:      Stable WiFi connection, webhook.site URL
 * - Serial:     115200 baud
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate5v2"
 *    from Tools -> Board.
 * 2) Visit https://webhook.site and copy your unique webhook URL.
 * 3) Paste only the path part (e.g. "/abcd-1234-efgh") into WEBHOOK_PATH.
 * 4) Enter your WiFi credentials (ssid, pass) in the sketch.
 * 5) Upload the sketch to Inkplate 5v2.
 * 6) Open Serial Monitor (115200 baud) to observe connection status.
 * 7) Watch incoming POST requests live on webhook.site.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/5v2/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2026-01-01
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE5V2
#error "Wrong board selection for this example, please select Soldered Inkplate5 V2 in the boards menu."
#endif

// Include needed libraries
#include "Inkplate.h"
#include "WiFi.h"

// Create objects
Inkplate display(INKPLATE_1BIT);
WiFiClient client;

// Interval between POST requests (seconds)
#define POSTING_INTERVAL_IN_SECS 20

// WiFi credentials
const char *ssid = "your ssid";
const char *pass = "your password";

// Webhook.site settings
const char *server = "webhook.site";
const char *WEBHOOK_PATH = "your webhook api"; // e.g. "/abcd-1234-efgh"

// Last connection time
unsigned long lastConnectionTime = 0;

void setup()
{
    Serial.begin(115200);

    // Init Inkplate
    display.begin();
    display.clearDisplay();
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(6);

    display.printf("HTTP POST example\n\n");
    display.printf("Using webhook.site\n\n");
    display.printf("Open Serial Monitor\nat 115200 baud");
    display.display();

    // Connect to WiFi
    WiFi.mode(WIFI_MODE_STA);
    WiFi.begin(ssid, pass);

    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.print("Connected, IP address: ");
    Serial.println(WiFi.localIP());
}

void loop()
{
    if ((unsigned long)(millis() - lastConnectionTime) > POSTING_INTERVAL_IN_SECS * 1000UL)
    {
        display.clearDisplay();

        if (!client.connect(server, 80))
        {
            Serial.println("Connection failed");
            lastConnectionTime = millis();
            client.stop();
            return;
        }

        // Example data (replace with sensor readings if needed)
        int value = random(40);

        // URL-encoded POST body
        String data = "value=" + String(value);

        // Send HTTP POST request
        client.print(String("POST ") + WEBHOOK_PATH + " HTTP/1.1\r\n");
        client.print(String("Host: ") + server + "\r\n");
        client.println("Connection: close");
        client.println("User-Agent: Inkplate-ESP32");
        client.println("Content-Type: application/x-www-form-urlencoded");
        client.print("Content-Length: ");
        client.println(data.length());
        client.println();
        client.print(data);

        Serial.print("POST sent: ");
        Serial.println(data);

        lastConnectionTime = millis();
        delay(250);

        client.stop();
    }
}
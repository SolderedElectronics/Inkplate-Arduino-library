/**
 **************************************************
 * @file        Inkplate6PLUS_HTTP_POST_Request.ino
 * @brief       HTTP POST request demo for Soldered Inkplate 6PLUS.
 *
 * @details     Demonstrates how to connect Inkplate 6PLUS to a Wi-Fi network
 *              and send periodic HTTP POST requests. This example uses the
 *              ThingSpeak API as a simple endpoint for storing and visualizing
 *              posted data. Sent values are printed to the Serial Monitor.
 *
 *              The example posts a random value as field1 - replace it with real
 *              sensor data. It uses plain HTTP on port 80 (not HTTPS), and if a
 *              connection fails the sketch retries on the next interval.
 *              Expected output is basic instructions on the display to open the
 *              Serial Monitor, the Wi-Fi connection status and posted field data
 *              in the Serial Monitor, and the ThingSpeak channel graph updating
 *              with the posted values.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6PLUS
 * - Hardware:   Inkplate 6PLUS, USB cable
 * - Extra:      Wi-Fi connection, ThingSpeak account with a channel and Write
 *               API Key
 * - Serial:     115200 baud
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 6PLUS"
 *    from Tools -> Board.
 * 2) Create a free ThingSpeak account and a channel with at least "field1".
 * 3) Copy the channel Write API Key into writeAPIKey.
 * 4) Enter your Wi-Fi SSID and password in ssid/pass, and set
 *    POSTING_INTERVAL_IN_SESCS (minimum ~15 s for free ThingSpeak).
 * 5) Upload the sketch and open Serial Monitor at 115200 baud.
 * 6) Every interval, the device sends a POST request with field1 data.
 * 7) Open your ThingSpeak channel to view the received data/graph.
 *
 * @note        Quick start guide: Inkplate 6PLUS has no dedicated page yet,
 *              see https://docs.soldered.com/inkplate/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2021-02-11
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_INKPLATE6PLUS) && !defined(ARDUINO_INKPLATE6PLUSV2)
#error                                                                                                                 \
    "Wrong board selection for this example, please select e-radionica Inkplate 6Plus or Soldered Inkplate 6Plus in the boards menu."
#endif

// Include needed libraries
#include "Inkplate.h"
#include "WiFi.h"

// Create objects from included libraries
Inkplate display(INKPLATE_1BIT);
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
    // Uncomment this line if you have a USB Power Only Inkplate6PLUS
    // Must be called before display.begin()!
    //display.setInkplatePowerMode(INKPLATE_USB_PWR_ONLY);
    display.begin();

    // Clear frame buffer of display
    display.clearDisplay();

    // Set text color and size
    display.setTextColor(BLACK, WHITE);
    display.setTextSize(6);

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

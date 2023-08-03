/*
   InkplatePLUS2_HTTP_POST_Request example for Soldered Inkplate PLUS2
   For this example you will need USB-C cable, Inkplate PLUS2 and stable WiFi Internet connection.
   Select "Soldered Inkplate PLUS2" from Tools -> Board menu.
   Don't have "Soldered Inkplate PLUS2" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example will show you how to connect to a WiFi network and send a POST request via HTTP.
   We will use ThingSpeak API to see post requests. It's a free API that allows you to store and retrieve data using
   HTTP.
   1. Go to ThingSpeak.com and create a free account
   2. Open the Channels tab
   3. Create a new channel
   4. Create fields you want to use (this example uses 1 field called field1 and this name must be used when sending data)
   5. Open the channel, go to the API Keys tab and copy your Write API Key
   6. Enter your API key in the code below

   When you send a POST request, open your channel and you will see the graph where is your sent data.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   19 July 2023 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATEPLUS2
#error "Wrong board selection for this example, please select Inkplate PLUS2 in the boards menu."
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
    display.begin();

    // Clear the display and print message
    display.clearDisplay();
    display.setTextColor(BLACK);
    display.setTextSize(3);
    display.setCursor(30, 30);
    display.print("HTTP POST Request example");

    display.setTextSize(2);
    display.setCursor(30,70);
    display.print("Open Serial Monitor at 115200 baud rate");
    display.setCursor(30,100);
    display.print("to see what's happening");
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

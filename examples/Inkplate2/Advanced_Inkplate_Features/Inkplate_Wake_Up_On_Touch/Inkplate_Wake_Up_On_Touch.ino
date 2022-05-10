/*
    Inkplate_Wake_Up_On_Touch example for e-radionica.com Inkplate 2
    For this example you will need USB cable, Inkplate 2, resistor
    and one LED (and some wires and breadboard to connect it).
    Select "Inkplate 2(ESP32)" from Tools -> Board menu.
    Don't have "Inkplate 2(ESP32)" option? Follow our tutorial and add it:
    https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

    This example will show you how you can use wake up on touch feature. 
    You need to specify which pin (only pins which supports wkae on touch)
    will be used and which callback will be called on wake up(optional).
    When woken up, Inkplate will make API call to fetch time and display it on display

    Want to learn more about Inkplate? Visit www.inkplate.io
    Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
    10 May 2022 by soldered.com
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Inkplate 2 in the boards menu."
#endif

#include "Inkplate.h"
#include "Inter16pt7b.h"
#include <HTTPClient.h> //Include HTTP library to this sketch
#include <WiFi.h>       //Include ESP32 WiFi library to our sketch

#define ssid "" // Name of the WiFi network (SSID) that you want to connect Inkplate to
#define pass "" // Password of that WiFi network

#define Threshold 40 // Treshold sets sensitivity
touch_pad_t touchPin;

// Initialize Inkplate object
Inkplate display;

bool flag_wake_up = 0;

void wake_up_callback()
{
    // Here is possible to write some commands for ESP32 to execute
    // on wake up
}

void setup()
{
    // Begin serial communication
    Serial.begin(115200);

    // Initialize Inkplate
    display.begin();

    WiFi.begin(ssid, pass); // Try to connect to WiFi network
    Serial.println("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(1000); // While it is connecting to network, display dot every second, just to know that Inkplate is
    }

    display.setFont(&Inter16pt7b);

    display.setTextSize(1);      // Set text size
    display.setCursor(10, 70);     // Set cursor position
    display.setTextColor(INKPLATE2_BLACK, INKPLATE2_WHITE);

    char *data;
    data = (char*)ps_malloc(100000);  //Allocate array for the stream of data
    if (data != NULL)
    {
        HTTPClient http;
        int m = 0;
        if (http.begin("https://www.timeapi.io/api/Time/current/zone?timeZone=Europe/Zagreb"))
        {   // Now try to connect to some web page (in this example www.example.com. And yes, this is a valid Web page :))
            if (http.GET() == 200)
            {

                // If connection was successful, try to read content of the Web page and display it on screen
                while (http.getStream().available()) //If data available
                {
                    data[m++] = http.getStream().read();  //Store data in data variable
                    delayMicroseconds(5);
                }
                char *time_now = strstr(data, "\"time\"") + 8;  //Data we are fetching here is HTML, so we can find <title> tag
                
                *(time_now + 5) = '\0';
                Serial.println("");
                Serial.println(time_now);
                display.clearDisplay();
                display.setCursor(20, 75);
                display.setTextSize(2);
                display.setTextColor(INKPLATE2_RED, INKPLATE2_WHITE);
                display.print(time_now);
            }
        }
    }

    // Display to screen
    display.display();

    //Setup interrupt on Touch Pad 3 (GPIO15)
    touchAttachInterrupt(T3, wake_up_callback, Threshold);

    //Configure Touchpad as wakeup source
    esp_sleep_enable_touchpad_wakeup();

    // Set pin 14 to be output pin
    Serial.println("Going to sleep..");
    esp_deep_sleep_start();
}

void loop()
{
    digitalWrite(14, HIGH); // Set pin 14 to HIGH state
    delay(1000); // Wait a bit
    digitalWrite(14, LOW); // Set pin 14 to LOW state
    delay(1000); // Wait a bit
}

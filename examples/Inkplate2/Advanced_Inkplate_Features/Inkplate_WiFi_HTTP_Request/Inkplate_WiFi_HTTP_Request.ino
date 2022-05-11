/*
    Inkplate_WiFi_HTTP example for e-radionica.com Inkplate 2
    For this example you will need USB cable, Inkplate 2 and stable WiFi Internet connection
    Select "Inkplate 2(ESP32)" from Tools -> Board menu.
    Don't have "Inkplate 2(ESP32)" option? Follow our tutorial and add it:
    https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

    This example will show you how to connect to WiFi network, get data from Internet and display that data on epaper.
    This example is NOT on to how to parse HTML data from Internet - it will just print HTML on the screen.

    In quotation marks you will need write your WiFi SSID and WiFi password in order to connect to your WiFi network.

    Want to learn more about Inkplate? Visit www.inkplate.io
    Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
    30 March 2022 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Inkplate 2 in the boards menu."
#endif

#include "Inkplate.h"   //Include Inkplate library to the sketch
#include <HTTPClient.h> //Include HTTP library to this sketch
#include <WiFi.h>       //Include ESP32 WiFi library to our sketch

#define ssid "" // Name of the WiFi network (SSID) that you want to connect Inkplate to
#define pass "" // Password of that WiFi network

Inkplate display; // Create an object on Inkplate library and also set library into 1 Bit mode (BW)

void setup()
{
    Serial.begin(115200);                             // Initialize serial communication with PC
    display.begin();                                  // Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay();                           // Clear frame buffer of display
    display.display();                                // Put clear image on display
    display.setTextSize(1);                           // Set text scaling to two (text will be two times bigger)
    display.setCursor(0, 0);                          // Set print position
    display.setTextColor(INKPLATE2_BLACK, INKPLATE2_WHITE);               // Set text color to black and background color to white
    display.println("Scanning for WiFi networks..."); // Write text
    display.display();                                // Send everything to display (refresh display)

    int n = WiFi.scanNetworks(); // Start searching WiFi networks and put the nubmer of found WiFi networks in variable
    // n
    Serial.println("Scanning networks");
    display.setCursor(0, 0);                          // Set print position
    if (n == 0)
    {   // If you did not find any network, show the message and stop the program.
        display.print("No WiFi networks found!");
        display.display();
        while (true)
            ;
    }
    else
    {
        if (n > 10)
            n = 10; // If you did find, print name (SSID), encryption and signal strength of first 10 networks
        for (int i = 0; i < n; i++)
        {
            display.setTextColor(INKPLATE2_BLACK, INKPLATE2_WHITE);               // Set text color to black and background color to white
            display.print(WiFi.SSID(i));
            display.print((WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? 'O' : '*');
            display.print('\n');
            display.setTextColor(INKPLATE2_RED, INKPLATE2_WHITE);               // Set text color to red and background color to white
            display.println(WiFi.RSSI(i), DEC);
        }
        display.display(); // Refresh screen
    }

    display.clearDisplay();          // Clear everything in frame buffer
    display.setCursor(0, 0);         // Set print cursor to new position
    display.print("Connecting to "); // Print the name of WiFi network
    display.print(ssid);
    WiFi.begin(ssid, pass); // Try to connect to WiFi network
    Serial.println("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(1000); // While it is connecting to network, display dot every second, just to know that Inkplate is
    }
    Serial.println("Connected!");
    display.print("connected"); // If it's connected, notify user
    display.display();
    char *data;
    data = (char*)ps_malloc(100000);  //Allocate array for the stream of data
    if (data != NULL)
    {
        HTTPClient http;
        int m = 0;
        if (http.begin("http://example.com/index.html"))
        {   // Now try to connect to some web page (in this example www.example.com. And yes, this is a valid Web page :))
            if (http.GET() == 200)
            {

                // If connection was successful, try to read content of the Web page and display it on screen
                while (http.getStream().available()) //If data available
                {
                    data[m++] = http.getStream().read();  //Store data in data variable
                    delayMicroseconds(5);
                }
                char *title = strstr(data, "<title>");  //Data we are fetching here is HTML, so we can find <title> tag
                display.clearDisplay();
                display.setCursor(0, 10);
                display.setTextColor(INKPLATE2_BLACK, INKPLATE2_WHITE);
                display.println("Name of the site:");
                display.setCursor(10, 30);
                display.setTextColor(INKPLATE2_RED, INKPLATE2_WHITE);
                m = 7;
                Serial.print("Title: ");
                while (title[m] != '<') //Print title until next tag
                {
                    display.print(title[m]);
                    Serial.print(title[m]);
                    m++;
                }
                Serial.println("");
                display.display();
            }
        }
    }

}

void loop()
{
    // Nothing
}
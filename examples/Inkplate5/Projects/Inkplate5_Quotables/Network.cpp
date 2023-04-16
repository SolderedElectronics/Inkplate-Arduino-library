/*
    Network.cpp
    Inkplate 5 Arduino library
    David Zovko, Borna Biro, Denis Vajak, Zvonimir Haramustek @ Soldered
    27 March 2023
    https://github.com/SolderedElectronics/Inkplate-Arduino-library

    For support, please reach over forums: https://forum.soldered.com/
    For more info about the product, please check: www.inkplate.io

    This code is released under the GNU Lesser General Public License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html
    Please review the LICENSE file included with this example.
    If you have any questions about licensing, please contact hello@soldered.com
    Distributed as-is; no warranty is given.
*/

#include "Network.h"

void Network::begin(char *ssid, char *pass)
{
    // Initiating wifi, like in BasicHttpClient example
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);

    int cnt = 0;
    Serial.print(F("Waiting for WiFi to connect..."));
    while ((WiFi.status() != WL_CONNECTED))
    {
        Serial.print(F("."));
        delay(1000);
        ++cnt;

        if (cnt == 20)
        {
            Serial.println("Can't connect to WIFI, restarting");
            delay(100);
            ESP.restart();
        }
    }
    Serial.println(F(" connected"));

}

bool Network::getData(char* text, char* auth, int maxLen)
{
    bool f = 0;

    // If not connected to wifi reconnect wifi
    if (WiFi.status() != WL_CONNECTED)
    {
        WiFi.reconnect();

        delay(5000);

        int cnt = 0;
        Serial.println(F("Waiting for WiFi to reconnect..."));
        while ((WiFi.status() != WL_CONNECTED))
        {
            // Prints a dot every second that wifi isn't connected
            Serial.print(F("."));
            delay(1000);
            ++cnt;

            if (cnt == 7)
            {
                Serial.println("Can't connect to WIFI, restart initiated.");
                delay(100);
                ESP.restart();
            }
        }
    }

    // Wake up if sleeping and save inital state
    bool sleep = WiFi.getSleep();
    WiFi.setSleep(false);

    // Http object used to make get request
    HTTPClient http;

    http.getStream().setTimeout(10);
    http.getStream().flush();

    // Initiate http
    char link[] = "https://api.quotable.io/random";
    http.begin(link);

    // Dynamic Json from ArduinoJson library
    DynamicJsonDocument doc(30000);

    // Actually do request
    int httpCode = http.GET();
    if (httpCode == 200)
    {
        while (http.getStream().available() && http.getStream().peek() != '{')
            (void)http.getStream().read();

        // Try parsing JSON object
        DeserializationError error = deserializeJson(doc, http.getStream());

        if (error)
        {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.c_str());
            f = 1;
        }
        else
        {
            // Set all data got from internet using formatTemp and formatWind defined above
            // This part relies heavily on ArduinoJson library

            Serial.println();
            Serial.println("Success");

            // Save our data to data pointer from main file
            
            const char *buff1 = doc["content"];
            
            // If the quote doesn't fit in the buffer, return 0 and ESP will again do request
            if(strlen(buff1) > maxLen - 1)
            {
                Serial.println("The quote can't fit in the buffer, fetching another one");
                return 0;
            }
   
            strcpy(text, buff1);
            Serial.println();

            const char *buff2 = doc["author"];

            strcpy(auth, buff2);
            
            f = 0;
        }
    }
    else if (httpCode == 404)
    {
        // Coin id not found
        Serial.println(F("Info has not been found!"));
        while (1)
            ;
    }
    else
    {
        f = 1;
    }

    // Clear document and end http
    doc.clear();
    http.end();

    // Return to initial state
    WiFi.setSleep(sleep);

    return !f;
}

/*
Network.cpp
Inkplate 6 Arduino library
David Zovko, Borna Biro, Denis Vajak, Zvonimir Haramustek @ e-radionica.com
September 24, 2020
https://github.com/e-radionicacom/Inkplate-6-Arduino-library

For support, please reach over forums: forum.e-radionica.com/en
For more info about the product, please check: www.inkplate.io

This code is released under the GNU Lesser General Public License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html
Please review the LICENSE file included with this example.
If you have any questions about licensing, please contact techsupport@e-radionica.com
Distributed as-is; no warranty is given.
*/

#include "Network.h"

#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#include "Inkplate.h"

// Must be installed for this example to work
#include <ArduinoJson.h>

// external parameters from our main file
extern char ssid[];
extern char pass[];
extern char channel_id[];
extern char api_key[];

// Get our Inkplate object from main file to draw debug info on
extern Inkplate display;

// Static Json from ArduinoJson library
StaticJsonDocument<30000> doc;

void Network::begin()
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

    // Find internet time
    setTime();
}

// Gets time from ntp server
void Network::getTime(char *timeStr)
{
    // Get seconds since 1.1.1970.
    time_t nowSecs = time(nullptr);

    // Used to store time
    struct tm timeinfo;
    gmtime_r(&nowSecs, &timeinfo);

    // Copies time string into timeStr
    strncpy(timeStr, asctime(&timeinfo) + 4, 12);

    // Setting time string timezone
    int hr = 10 * timeStr[7] + timeStr[8] + timeZone;

    // Better defined modulo, in case timezone makes hours to go below 0
    hr = (hr % 24 + 24) % 24;

    // Adding time to '0' char makes it into whatever time char, for both digits
    timeStr[7] = hr / 10 + '0';
    timeStr[8] = hr % 10 + '0';
}

bool Network::getData(channelInfo *channel)
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

    // WiFiClientSecure object used to make GET request
    WiFiClientSecure client1;

    char request[182];
    sprintf(request, "www.googleapis.com/youtube/v3/channels?part=statistics&id=%s&key=%s", channel_id, api_key);
    int result = getRequest(&client1, "www.googleapis.com",request);
    if(result == 0)
    {
        Serial.println("HTTP Error!");
        Serial.println("Restarting...");
        delay(100);
        ESP.restart();
    } 
    else if(result == 404)
    {
        // Channel was not found via ID 

        Serial.println("Can't find YT channel!");
        display.clearDisplay();
        display.setCursor(50, 230);
        display.setTextSize(2);
        display.println(F("Channel info has not been found!"));
        display.display();
        while(1) ;
    }
    

    DeserializationError error = deserializeJson(doc, client1);

    if (error)
    {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        f = 1;
    }
    else if (doc["items"].size() > 0)
    {
        // Set all data got from internet using formatTemp and formatWind defined above
        // This part relies heavily on ArduinoJson library

        Serial.println("Success");

        channel->total_views = doc["items"][0]["statistics"]["viewCount"].as<unsigned long long>();
        Serial.print("Total views: ");
        Serial.println(channel->total_views);

        channel->subscribers = doc["items"][0]["statistics"]["subscriberCount"].as<unsigned long long>();
        Serial.print("Subscribers: ");
        Serial.println(channel->subscribers);

        channel->video_count = doc["items"][0]["statistics"]["videoCount"].as<int>();
        Serial.print("Videos: ");
        Serial.println(channel->video_count);

        f = 0;
    }

    client1.flush();
    client1.setTimeout(10);
    doc.clear();
    client1.stop();

    // WiFiClientSecure object used to make GET request
    WiFiClientSecure client2;
    
    // To get the channel name, access the 'snippet' YouTube API
    memset(request, 0, 182 * sizeof(char));
    sprintf(request, "www.googleapis.com/youtube/v3/channels?part=snippet&id=%s&key=%s", channel_id, api_key);
    result = getRequest(&client2, "www.googleapis.com",request);
    if(result == 0)
    {
        Serial.println("HTTP Error!");
        Serial.println("Restarting...");
        delay(100);
        ESP.restart();
    } 
    else if(result == 404)
    {
        // Channel was not found via ID 

        Serial.println("Can't find YT channel!");
        display.clearDisplay();
        display.setCursor(50, 230);
        display.setTextSize(2);
        display.println(F("Channel info has not been found!"));
        display.display();
        while(1) ;
    }

    error = deserializeJson(doc, client2);

    if (error)
    {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        f = 1;
    }
    else if (doc["items"].size() > 0)
    {
        // Set all data got from internet using formatTemp and formatWind defined above
        // This part relies heavily on ArduinoJson library

        Serial.println("Success");

        const char *buff = doc["items"][0]["snippet"]["title"];
        strcpy(channel->name, buff);
        Serial.print("Channel name: ");
        Serial.println(channel->name);

        // Save our data to data pointer from main file
        f = 0;
    }

    client2.flush();
    client2.setTimeout(10);
    doc.clear();
    client2.stop();

    // Return to initial state
    WiFi.setSleep(sleep);

    return !f;
}

// Function for initial time setting ovet the ntp server
void Network::setTime()
{
    // Used for setting correct time
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");

    Serial.print(F("Waiting for NTP time sync: "));
    time_t nowSecs = time(nullptr);
    while (nowSecs < 8 * 3600 * 2)
    {
        delay(500);
        Serial.print(F("."));
        yield();
        nowSecs = time(nullptr);
    }

    Serial.println();

    // Used to store time info
    struct tm timeinfo;
    gmtime_r(&nowSecs, &timeinfo);

    Serial.print(F("Current time: "));
    Serial.print(asctime(&timeinfo));
}

/**
 * @brief               Create a HTTPS GET request with a given root url and api call url
 * 
 * @param               WiFiClientSecure * client: pointer to client used in base class
 * 
 * @param               char * _api_root_url: root url of the api (eg. www.api-service.com)
 * 
 * @param               char * _api_call_url: full url of the api call (eg. www.api-service.com/getdata?key=12345)
 * 
 * @returns             0 if there was an error, 404 if not found, 1 if successful
*/
int Network:: getRequest(WiFiClientSecure * client, char * _api_root_url, char * _api_call_url)
{
    // Don't check SSL certificate but still use HTTPS
    client->setInsecure();

    if(!client->connect(_api_root_url,443))
    {
        return 0;
    }

    client->setTimeout(10);
    client->flush();
    client->print("GET ");
    client->print(_api_call_url);
    client->println(" HTTP/1.0");
    client->print("Host: ");
    client->println(_api_root_url);
    client->println("Connection: close");
    client->println();

    while (client->available() == 0)
        ;

    String line = client->readStringUntil('\r');
    if (line != "HTTP/1.0 200 OK")
    {
        return 0;
    }
    else if (line == "HTTP/1.0 404 Not Found")
    {
        return 404;
    }

    while (client->available() && client->peek() != '{')
        (void)client->read();

    return 1;
}

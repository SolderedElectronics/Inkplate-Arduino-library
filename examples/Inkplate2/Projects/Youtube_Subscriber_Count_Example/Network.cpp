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

// Static Json from ArduinoJson library
StaticJsonDocument<5000> doc;

void Network::begin(char * ssid, char * pass)
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

bool Network::getData(channelInfo * channel, char * channel_id, char * api_key, Inkplate * display)
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
        display->clearDisplay();
        display->setCursor(50, 230);
        display->setTextSize(2);
        display->println(F("Channel info has not been found!"));
        display->display();
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
        display->clearDisplay();
        display->setCursor(50, 230);
        display->setTextSize(2);
        display->println(F("Channel info has not been found!"));
        display->display();
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

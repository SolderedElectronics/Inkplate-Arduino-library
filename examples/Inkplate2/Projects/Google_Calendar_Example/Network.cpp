/*
Network.cpp
Inkplate Arduino library
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

        WiFi.reconnect();
        delay(5000);

        if (cnt == 10)
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
void Network::getTime(char *timeStr, long offSet, struct tm *timeinfo, int timeZone)
{
    // Get seconds since 1.1.1970.
    time_t nowSecs = time(nullptr) + (long)timeZone * 3600L + offSet;


    // Used to store time
    gmtime_r(&nowSecs, timeinfo);

    // Copies time string into timeStr
    strcpy(timeStr, asctime(timeinfo));
}

// Function to get all war data from web
bool Network::getData(char *data, char *calendarURL)
{
    // Variable to store fail
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

            WiFi.reconnect();
            delay(5000);

            if (cnt == 10)
            {
                Serial.println("Can't connect to WIFI, restart initiated.");
                delay(100);
                ESP.restart();
            }
        }
    }

    // Begin http by passing url to it
    bool sleep = WiFi.getSleep();
    WiFi.setSleep(false);

    // WiFiClientSecure object used to make GET request
    WiFiClientSecure client;

    int result = getRequest(&client, "calendar.google.com", calendarURL);
    if (result == 0)
    {
        Serial.println("HTTP Error!");
        Serial.println("Restarting...");
        delay(100);
        ESP.restart();
    }

    long n = 0;
    while (client.available())
        data[n++] = client.read();
    data[n++] = 0;
    delayMicroseconds(2);

    client.stop();

    return !f;
}

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

time_t Network::getEpoch()
{
    return time(nullptr);
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
int Network::getRequest(WiFiClientSecure *client, char *_api_root_url, char *_api_call_url)
{
    // Don't check SSL certificate but still use HTTPS
    client->setInsecure();

    if (!client->connect(_api_root_url, 443))
    {
        Serial.println("connection refused");
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

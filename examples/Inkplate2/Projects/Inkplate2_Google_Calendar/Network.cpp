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

/**
 * @brief               Initialize the network object and connect to WiFi
 *
 * @param               char * ssid
 *                      SSID of the WiFi network
 * 
 * @param               char * pass
 *                      WiFi password
 *
 * @return              None
 */
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

/**
 * @brief               Get time from NTP server
 *
 * @param               char * timeStr
 *                      Where to save the string of the fetched time
 * 
 * @param               long offSet
 *                      Time adjustment
 * 
 * @param               struct tm * timeinfo
 *                      Where to store the struct tm of the fetched time
 * 
 * @param               int timeZone
 *                      Timezone adjustment
 * 
 * @return              None
 */
void Network::getTime(char *timeStr, long offSet, struct tm *timeinfo, int timeZone)
{
    // Get seconds since 1.1.1970.
    time_t nowSecs = time(nullptr) + (long)timeZone * 3600L + offSet;

    // Used to store time
    gmtime_r(&nowSecs, timeinfo);

    // Copies time string into timeStr
    strcpy(timeStr, asctime(timeinfo));
}

/**
 * @brief               Get data from Google Calendar
 *
 * @param               char * data
 *                      Where to save the recieved data
 * 
 * @param               char * calendarURL
 *                      Link to the calendar as set by the user in the main file
 * 
 * @return              True if succcessful, false if there was an error
 */
bool Network::getData(char *calendarURL, char *data)
{
    // Variable to store fail
    bool f = 0;

    // If not connected to WiFi, reconnect wifi
    if (WiFi.status() != WL_CONNECTED)
    {
        WiFi.reconnect();

        // Waiting to WiFi connect again
        int cnt = 0;
        Serial.println(F("Waiting for WiFi to reconnect..."));
        while ((WiFi.status() != WL_CONNECTED))
        {
            // Prints a dot every second that wifi isn't connected
            Serial.print(F("."));
            delay(1000);
            ++cnt;

            // If it doesn't connect to wifi in 10 seconds, reset the ESP
            if (cnt == 10)
            {
                Serial.println("Can't connect to WIFI, restart initiated.");
                delay(100);
                ESP.restart();
            }
        }
    }

    // Http object used to make get request
    HTTPClient http;
    http.getStream().setTimeout(10);
    http.getStream().flush();
    http.getStream().setNoDelay(true);

    // Begin http by passing url to it
    http.begin(calendarURL);

    delay(300);

    int attempts = 1;

    // Download data until it's a verified complete download
    // Actually do request
    int httpCode = http.GET();

    if (httpCode == 200)
    {
        long n = 0;

        long now = millis();

        while (millis() - now < DOWNLOAD_TIMEOUT)
        {
            while (http.getStream().available())
            {
                data[n++] = http.getStream().read();
                now = millis();
            }
        }

        data[n++] = 0;

        // If the calendar doesn't contain this string - it's invalid
        if(strstr(data, "END:VCALENDAR") == NULL) f = 1;
    }
    else
    {
        // In case there was another HTTP code, break from the function
        Serial.print("HTTP Code: ");
        Serial.print(httpCode);
        f = 1;
    }

    // end http
    http.end();

    return !f;
}


/**
 * @brief               Set the current time from NTP
 * 
 * @return              None
 */
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
 * @brief               Get the current epoch
 * 
 * @return              time_t of the current epoch
 */
time_t Network::getEpoch()
{
    return time(nullptr);
}

/**
 * @brief               Create a HTTPS GET request
 * 
 * @param               WiFiClientSecure * client
 *                      Pointer to the WiFiClientSecure used in getData
 * 
 * @param               char * _api_root_url
 *                      Root URL of the GET call (eg. www.example.com)
 * 
 * @param               char * _api_call_url
 *                      Resource URL of the GET call (eg. /api/resource/123&param=example)
 *                      View your specific API's documentation for more info
 * 
 * @return              True if successful, false if not
 */
bool Network::getRequest(WiFiClientSecure *client, char *_api_root_url, char *_api_call_url)
{
    // Don't check SSL certificate but still use HTTPS
    client->setInsecure();

    if (!client->connect(_api_root_url, 443))
    {
        Serial.println("connection refused");
        return false;
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
        return false;
    }

    while (client->available() && client->peek() != '{')
        (void)client->read();

    return true;
}
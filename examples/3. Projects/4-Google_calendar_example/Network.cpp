#include "Network.h"

#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

// WiFiMulti object declaration
WiFiMulti WiFiMulti;

void Network::begin()
{
    // Initiating wifi, like in BasicHttpClient example
    WiFi.mode(WIFI_STA);
    WiFiMulti.addAP(ssid, pass);

    Serial.print(F("Waiting for WiFi to connect..."));
    while ((WiFiMulti.run() != WL_CONNECTED))
    {
        // Prints a dot every second that wifi isn't connected
        Serial.print(F("."));
        delay(1000);
    }
    Serial.println(F(" connected"));

    // Find internet time
    setTime();
}

// Gets time from ntp server
void Network::getTime(char *timeStr, long offSet)
{
    // Get seconds since 1.1.1970.
    time_t nowSecs = time(nullptr) + (long)timeZone * 3600L + offSet;

    // Used to store time
    struct tm timeinfo;
    gmtime_r(&nowSecs, &timeinfo);

    // Copies time string into timeStr
    strcpy(timeStr, asctime(&timeinfo));
}

// Function to get all war data from web
bool Network::getData(char *data)
{
    // Variable to store fail
    bool f = 0;

    // Return if wifi isn't connected
    if (WiFi.status() != WL_CONNECTED)
        return 0;

    // Wake up if sleeping and save inital state
    bool sleep = WiFi.getSleep();
    WiFi.setSleep(false);

    // Http object used to make get request
    HTTPClient http;

    http.getStream().setTimeout(10);
    http.getStream().flush();

    // Begin http by passing url to it
    http.begin(calendarURL);

    delay(300);

    // Actually do request
    int httpCode = http.GET();
    if (httpCode == 200)
    {
        long n = 0;
        while (http.getStream().available())
            data[n++] = http.getStream().read();
    }
    else
    {
        f = 1;
    }

    // end http
    http.end();

    // Return to initial state
    WiFi.setSleep(sleep);

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

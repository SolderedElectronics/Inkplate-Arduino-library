/**
 * Network.cpp
 * Inkplate Arduino Library
 * December 21, 2022
 * https://github.com/e-radionicacom/Inkplate-6-Arduino-library
 * For support, please reach over forums: forum.e-radionica.com/en
 * For more info about the product, please check: www.inkplate.io
 * This code is released under the GNU Lesser General Public License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html
 * Please review the LICENSE file included with this example.
 * If you have any questions about licensing, please contact techsupport@e-radionica.com
 * Distributed as-is; no warranty is given.
 */

#include "Network.h"

void Network::begin(char *ssid, char *pass)
{
    Serial.print("Attempting to connect to ");
    Serial.println(ssid);
    WiFi.begin(ssid, pass);

    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        // wait 1 second for re-trying
        delay(1000);
    }

    Serial.println();
    Serial.print("Connected to ");
    Serial.println(ssid);
}

void Network::getData(int *backers, int *pledged)
{
    WiFiClientSecure client;
    client.setInsecure(); // Use HTTPS but don't compare certificate
    client.flush();
    client.setTimeout(10);

    // Http object used to make get request
    HTTPClient http;
    http.getStream().setNoDelay(true);
    http.getStream().setTimeout(1);

    // Initiate http
    http.begin(client, "https://www.kickstarter.com/projects/solderedelectronics/"
                       "inkplate-2-a-easy-to-use-arduino-compatible-e-paper/stats.json?v=1");

    // Actually do request
    int httpCode = http.GET();
    if (httpCode == 200)
    {
        while (http.getStream().available() && http.getStream().peek() != '{')
            (void)http.getStream().read();

        // Try parsing JSON object
        StaticJsonDocument<256> doc;
        DeserializationError error = deserializeJson(doc, http.getStream());

        if (error)
        {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.c_str());
        }
        else
        {
            Serial.println("Data received and deserialized successfully!");
            *pledged = doc["project"]["pledged"];
            *backers = doc["project"]["backers_count"];


            doc.clear();
        }
    }

    // End http
    http.end();
    client.stop();
}

// The function used to set the current time
void Network::setTime(struct tm *timeinfo, int timeZone)
{
    // Used for setting correct time
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");

    time_t nowSecs;

    Serial.print(F("Waiting for NTP time sync: "));
    nowSecs = time(nullptr);
    while (nowSecs < 8 * 3600 * 2)
    {
        // Print a dot every half a second while time is not set
        delay(500);
        Serial.print(F("."));
        yield();
        nowSecs = time(nullptr) + timeZone * 3600;
    }

    Serial.println();

    // Used to store time info
    gmtime_r(&nowSecs, timeinfo);

    Serial.print(F("Current time: "));
    Serial.print(asctime(timeinfo));
}

/*
    Network.cpp
    Inkplate Arduino library

    Karlo Leksic for Soldered.com
    January 2, 2023
    https://github.com/e-radionicacom/Inkplate-6-Arduino-library

    For support, please reach over forums: forum.e-radionica.com/en
    For more info about the product, please check: www.inkplate.io

    This code is released under the GNU Lesser General Public License v3.0:
    https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the LICENSE file included with this example. If you have
    any questions about licensing, please contact techsupport@e-radionica.com Distributed as-is; no warranty is given.
*/

#include "Network.h"

void Network::begin(char *ssid, char *pass)
{
    WiFi.begin(ssid, pass);
    Serial.print("Connecting ");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());
}

int Network::getData(struct forecastWeather *fw, String lat, String lon, String apiKey, int *timezone)
{
    // Allocate memory for data from API
    uint32_t n = 0;
    char *buf = (char *)ps_malloc(100000);
    if (buf == NULL)
    {
        Serial.println("Memory allocation failed");
        while (1)
            ;
    }

    String url = "https://api.openweathermap.org/data/2.5/forecast?lat=" + lat + "&lon=" + lon + "&appid=" + apiKey;

    // Begin HTTP connection and send get request to the open weather
    Serial.println("Loading buffer...");
    HTTPClient http;
    if (http.begin(url) && http.GET() > 0)
    {
        while (http.getStreamPtr()->available())
        {
            char c = http.getStreamPtr()->read();
            buf[n++] = c;
        }
        buf[n] = '\0';
    }

    Serial.println("Buffer load complete!");

    // Deserialize data from api
    DynamicJsonDocument doc(20000);
    DeserializationError error = deserializeJson(doc, buf);
    if (error)
    {
        Serial.println("Deserialization error ");
        free(buf);
        doc.clear();
        http.end();
        while (1)
            ;
    }

    // If the doc doesn't contain key "code", something went wrong
    if (!doc.containsKey("cod"))
    {
        free(buf);
        doc.clear();
        http.end();
        while (1)
            ;
    }

    // Get data count
    int cnt = doc["cnt"];

    // Read data and store them in structures
    for (int i = 0; i < cnt; ++i)
    {
        fw[i].timestamp = doc["list"][i]["dt"];
        float tempK = doc["list"][i]["main"]["temp"];
        fw[i].minTemp = int(doc["list"][i]["main"]["temp_min"]) - 273; // Convert to celsius
        fw[i].maxTemp = int(doc["list"][i]["main"]["temp_max"]) - 273; // Convert to celsius
        strlcpy(fw[i].icon, doc["list"][i]["weather"][0]["icon"] | "01d", sizeof(fw[i].icon) - 1);
    }

    *timezone = doc["city"]["timezone"];

    free(buf);
    doc.clear();
    return cnt;
}

void Network::setTime(int timezone, struct tm *timeinfo)
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
    nowSecs += timezone;
    gmtime_r(&nowSecs, timeinfo);

    Serial.print(F("Current time: "));
    Serial.print(asctime(timeinfo));
}

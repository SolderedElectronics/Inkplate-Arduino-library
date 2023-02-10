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

#include "Inkplate.h"

// Must be installed for this example to work
#include <ArduinoJson.h>

// external parameters from our main file
extern char ssid[];
extern char pass[];
extern char user_task_list_gid[];
extern char user_PAT[];

// Get our Inkplate object from main file to draw debug info on
extern Inkplate display;

// Static Json from ArduinoJson library
DynamicJsonDocument doc(80000);

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
    sprintf(timeStr,"%4d %2d %2d", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday);
    if(timeStr[5] == ' ')
    {
      timeStr[5] = '0';
    }
    if(timeStr[8] == ' ')
    {
      timeStr[8] = '0';
    }
    timeStr[4] = timeStr[7] = '-';
    Serial.println(timeStr);
}

struct task* Network::getData()
{
    bool f = 0;

    struct task *tasks = NULL;
    struct task *curr_task = NULL;

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
    char temp[182], header[256];
    sprintf(temp, "https://app.asana.com/api/1.0/projects/%s/tasks", user_task_list_gid);
    sprintf(header, "Bearer %s", user_PAT);
    http.begin(temp);
    http.addHeader("Authorization", header); 

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
        for (JsonObject data_item : doc["data"].as<JsonArray>())
        {
            if(tasks == NULL)
            {
                tasks =(struct task*)ps_malloc(sizeof(struct task));
                curr_task = tasks;
            }
            else
            {
                curr_task->next = (struct task*)ps_malloc(sizeof(struct task));
                curr_task = curr_task->next;
            }
            strcpy(curr_task->gid, data_item["gid"]);   // "1202207920290418", "1202207459957201", ...
            strcpy(curr_task->name, data_item["name"]); // "Napisati firmware za tester za MQ s easyC", "LoRa ...
            strcpy(curr_task->res_type, data_item["resource_type"]); // "task", "task", "task", "task", ...
            curr_task->next = NULL;
        }
        curr_task = tasks;
    }
    else if (httpCode == 404)
    {
        // Coin id not found
        display.clearDisplay();
        display.setCursor(50, 230);
        display.setTextSize(2);
        display.println(F("Info has not been found!"));
        display.display();
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

    return tasks;
}

void Network::getDue(struct task *task)
{
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
    char temp[182], header[256];
    sprintf(temp, "https://app.asana.com/api/1.0/tasks/%s", task->gid);
    sprintf(header, "Bearer %s", user_PAT);
    http.begin(temp);
    http.addHeader("Authorization", header); 

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
        }
        if(!doc["data"]["due_on"].isNull())
        {
          strcpy(task->due, doc["data"]["due_on"]);
        }
        else
        {
          strcpy(task->due, "No due!\0");
        }
    }
    else if (httpCode == 404)
    {
        // Coin id not found
        display.clearDisplay();
        display.setCursor(50, 230);
        display.setTextSize(2);
        display.println(F("Info has not been found!"));
        display.display();
        while (1)
            ;
    }

    // Clear document and end http
    doc.clear();
    http.end();
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

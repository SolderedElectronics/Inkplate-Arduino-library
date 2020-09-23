#include "Inkplate.h"

#include <ArduinoJson.h>
#include <ArduinoSpotify.h>
#include <ArduinoSpotifyCert.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#include "generatedUI.h"

Inkplate display(INKPLATE_1BIT);

char ssid[] = "";
char password[] = "";

char clientId[] = "";
char clientSecret[] = "";

#define SPOTIFY_REFRESH_TOKEN ""

#define SPOTIFY_MARKET "IE"


char name[128];
char title[128];
char album[128];
char url[128];
int imgW;
bool playing;

int elapsedTime = 0;
int totalTime = 0;
float elapsed = 0;

#define FULLREFRESH 10
int cnt = 0;

const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

WiFiClientSecure client;
ArduinoSpotify spotify(client, clientId, clientSecret, SPOTIFY_REFRESH_TOKEN);

unsigned long delayBetweenRequests = 100; // Time between requests (1 minute)

void setup()
{
    Serial.begin(115200);
    display.begin();

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    Serial.print("Connecting...");

    // Wait for connection
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

    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    client.setCACert(spotify_server_cert);

    Serial.println("Refreshing Access Tokens");
    if (!spotify.refreshAccessToken())
    {
        Serial.println("Failed to get access tokens");
    }

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    display.display();
}

void loop()
{
    if (!display.isConnected())
    {
        Serial.println("Reconnecting...");
        delay(100);
        if (!display.joinAP(ssid, password))
            ESP.restart();
    }

    CurrentlyPlaying currentlyPlaying = spotify.getCurrentlyPlaying(SPOTIFY_MARKET);

    if (!currentlyPlaying.error)
    {
        parsePlayingData(currentlyPlaying);
        updateScreenContent();
    }

    delay(delayBetweenRequests);
}

void parsePlayingData(CurrentlyPlaying currentlyPlaying)
{
    strcpy(name, currentlyPlaying.firstArtistName);
    strcpy(title, currentlyPlaying.trackName);
    strcpy(album, currentlyPlaying.albumName);

    playing = currentlyPlaying.isPlaying;

    int mn = 0;
    for (int i = 1; i < currentlyPlaying.numImages; i++)
        if (abs(currentlyPlaying.albumImages[i].width - 300) + abs(currentlyPlaying.albumImages[i].width - 300) <
            abs(currentlyPlaying.albumImages[mn].width - 300) + abs(currentlyPlaying.albumImages[mn].width - 300))
            mn = i;

    imgW = currentlyPlaying.albumImages[mn].width;
    strcpy(url, currentlyPlaying.albumImages[mn].url);

    elapsed = (float)currentlyPlaying.progressMs / (float)currentlyPlaying.duraitonMs;
    elapsedTime = currentlyPlaying.progressMs / 1000;
    totalTime = currentlyPlaying.duraitonMs / 1000;
}

void updateScreenContent()
{
    display.clearDisplay();

    line1_color = line2_color = playing;
    triangle2_color = !playing;

    strncpy(text1_content, name, min((int)35, (int)strlen(name)));
    strncpy(text2_content, title, min((int)35, (int)strlen(title)));
    strncpy(text3_content, album, min((int)35, (int)strlen(album)));

    text1_content[min((int)35, (int)strlen(name))] = 0;
    text2_content[min((int)35, (int)strlen(title))] = 0;
    text3_content[min((int)35, (int)strlen(album))] = 0;

    if (strlen(name) >= 35)
        strcat(text1_content, "...");
    if (strlen(title) >= 35)
        strcat(text2_content, "...");
    if (strlen(album) >= 35)
        strcat(text3_content, "...");

    int16_t x1 = 0, y1 = 0;
    uint16_t w = 0, h = 0;

    display.setFont(text1_font);
    display.getTextBounds(text1_content, 0, 200, &x1, &y1, &w, &h);
    text1_cursor_x = 400 - w / 2;

    display.setFont(text2_font);
    display.getTextBounds(text2_content, 0, 200, &x1, &y1, &w, &h);
    text2_cursor_x = 400 - w / 2;

    display.setFont(text3_font);
    display.getTextBounds(text3_content, 0, 200, &x1, &y1, &w, &h);
    text3_cursor_x = 400 - w / 2;

    display.drawJpegFromWeb(url, 400 - imgW / 2, 103, 1);

    circle2_center_x = elapsed * (730 - 70) + 70;

    digital_clock2_h = elapsedTime / 60;
    digital_clock2_m = elapsedTime % 60;

    digital_clock5_h = totalTime / 60;
    digital_clock5_m = totalTime % 60;

    struct tm timeinfo;
    if (getLocalTime(&timeinfo))
    {
        digital_clock4_h = timeinfo.tm_hour;
        digital_clock4_m = timeinfo.tm_min;
    }

    mainDraw();

    if (cnt++ % FULLREFRESH == 0)
        display.display();
    else
        display.partialUpdate();
}
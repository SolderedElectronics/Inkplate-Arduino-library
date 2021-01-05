/*
   Spotify_api_example for e-radionica.com Inkplate 6
   For this example you will need only USB cable and Inkplate 6.
   Select "Inkplate 6(ESP32)" from Tools -> Board menu.
   Don't have "Inkplate 6(ESP32)" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   This project shows you how Inkplate 6 can be used to display
   your spotify real time activity.

   To get this working, see https://inkplate.readthedocs.io/en/latest/examples.html#spotify-api
   It is quite complicated, but were sure you'll get it working!

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   3 August 2020 by e-radionica.com
*/

#include "Inkplate.h"

#include <ArduinoJson.h>
#include <ArduinoSpotify.h>
#include <ArduinoSpotifyCert.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

// Include auto generated UI code
#include "generatedUI.h"

// Declare our Inkplate object
Inkplate display(INKPLATE_1BIT);

// Fill these with your credentials

char ssid[] = "";
char password[] = "";

char clientId[] = "";
char clientSecret[] = "";

#define SPOTIFY_REFRESH_TOKEN ""

#define SPOTIFY_MARKET "IE"

// --------------------------------

// Variables for storing data to be rendered to screen
char name[128];
char title[128];
char album[128];
char url[128];
int imgW;
bool playing;

int elapsedTime = 0;
int totalTime = 0;
float elapsed = 0;

// Define how often to do a full screen refresh
#define FULLREFRESH 10
int cnt = 0;

// Adjust these for your timezone
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;
const int daylightOffset_sec = 3600;

// Initiate Spotify API wrapper object
WiFiClientSecure client;
ArduinoSpotify spotify(client, clientId, clientSecret, SPOTIFY_REFRESH_TOKEN);

// Time between requests to Spotify API
unsigned long delayBetweenRequests = 100;

void setup()
{
    // Initialise serial and display objects
    Serial.begin(115200);
    display.begin();

    // Connect to WiFi
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

    // Set Spotify certificate
    client.setCACert(spotify_server_cert);

    Serial.println("Refreshing Access Tokens");
    if (!spotify.refreshAccessToken())
    {
        Serial.println("Failed to get access tokens");
    }

    // Get time from internet
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    // Display blank screen
    display.display();
}

void loop()
{
    // If Inkplate disconnected form WiFi try reconnecting and if that fails reset
    if (!display.isConnected())
    {
        Serial.println("Reconnecting...");
        delay(100);
        if (!display.joinAP(ssid, password))
            ESP.restart();
    }

    // Get currently playing track info
    CurrentlyPlaying currentlyPlaying = spotify.getCurrentlyPlaying(SPOTIFY_MARKET);

    // If all went well display to screen
    if (!currentlyPlaying.error)
    {
        parsePlayingData(currentlyPlaying);
        updateScreenContent();
    }

    delay(delayBetweenRequests);
}

void parsePlayingData(CurrentlyPlaying currentlyPlaying)
{
    // Copy data to global variables
    strcpy(name, currentlyPlaying.firstArtistName);
    strcpy(title, currentlyPlaying.trackName);
    strcpy(album, currentlyPlaying.albumName);

    playing = currentlyPlaying.isPlaying;

    // Find image whose dimensions are closest to 300 by 300
    int mn = 0;
    for (int i = 1; i < currentlyPlaying.numImages; i++)
        if (abs(currentlyPlaying.albumImages[i].width - 300) + abs(currentlyPlaying.albumImages[i].width - 300) <
            abs(currentlyPlaying.albumImages[mn].width - 300) + abs(currentlyPlaying.albumImages[mn].width - 300))
            mn = i;

    // Copy data to global variables
    imgW = currentlyPlaying.albumImages[mn].width;
    strcpy(url, currentlyPlaying.albumImages[mn].url);

    // Copy data to global variables
    elapsed = (float)currentlyPlaying.progressMs / (float)currentlyPlaying.duraitonMs;
    elapsedTime = currentlyPlaying.progressMs / 1000;
    totalTime = currentlyPlaying.duraitonMs / 1000;
}

void updateScreenContent()
{
    // Initally clear the INkplate buffer
    display.clearDisplay();

    // Display playing info
    line1_color = line2_color = playing;
    triangle2_color = !playing;

    // Limit text to 35 chars and add ... if needed
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

    // Functions to center text and image
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

    // Display time
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

    // Draw all rest auto generated UI
    mainDraw();

    // Every FULLREFRESH do a full refresh
    if (cnt++ % FULLREFRESH == 0)
        display.display();
    else
        display.partialUpdate();
}
/**
 **************************************************
 * @file        Inkplate6COLOR_Spotify_Dashboard.ino
 * @brief       Connects to Spotify Web API, fetches album/track data, and
 *              displays it on Inkplate 6COLOR.
 *
 * @details     This example demonstrates how to connect Inkplate 6COLOR to
 *              Spotify's Web API, authenticate using OAuth credentials, parse
 *              the returned JSON data, and render Spotify content on the
 *              e-paper display.
 *
 *              The setup requires a Spotify Developer application, client
 *              credentials, and a refresh token. The refresh token is generated
 *              once using the helper script included with the project, then
 *              reused by the sketch to obtain access tokens for API requests.
 *              Expired or incorrect credentials will prevent data loading.
 *
 *              This example is intended as a dashboard-style integration for
 *              displaying Spotify album, track, or playback-related data on the
 *              colour e-paper panel. Because the workflow depends on web APIs,
 *              JSON parsing, authentication, and image/content fetching, network
 *              reliability and RAM usage must be considered carefully, and API
 *              rate limits, token handling and network errors should be taken
 *              into account during extended or repeated use.
 *
 *              Expected output: Spotify dashboard content such as album/track
 *              information and related visual data, depending on the
 *              implementation in the sketch, plus optional authentication,
 *              network or API debug output on the Serial Monitor.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6COLOR
 * - Hardware:   Inkplate 6COLOR, USB cable
 * - Extra:      WiFi, Spotify account, Spotify Developer app, browser
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 6COLOR"
 *    from Tools -> Board.
 * 2) Create a Spotify Developer application in the Spotify Developer Dashboard,
 *    set the Redirect URI to http://127.0.0.1:8888/callback and enable the
 *    Spotify Web API for the app.
 * 3) Copy the Client ID and Client Secret into the sketch/project config.
 * 4) Open the token helper script in src/spotify-token and fill in the
 *    required CLIENT_ID, CLIENT_SECRET and REDIRECT_URI values.
 * 5) Run the token helper script to generate the refresh token, then copy the
 *    resulting refresh_token into the sketch configuration.
 * 6) Enter your WiFi credentials and upload the sketch to Inkplate 6COLOR.
 * 7) After connecting to WiFi, the board authenticates with the Spotify Web API,
 *    fetches the configured data, and displays it on the e-paper screen.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/6color/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2026-02-17
 * @license     GNU GPL V3
 **************************************************/

#include "src/includes.h"
#include "src/NetworkFunctions.h"
#include "src/Gui.h"


// CREDENTIALS / SECRETS
static const char* WIFI_SSID     = "";
static const char* WIFI_PASSWORD = "";

static const char* SPOTIFY_CLIENT_ID     = "your_spotify_client_id";
static const char* SPOTIFY_CLIENT_SECRET = "your_spotify_client_secret";
static const char* SPOTIFY_REFRESH_TOKEN = "your_spotify_refresh_token";


Inkplate display; 
Gui gui(display);


RTC_DATA_ATTR char lastAlbumId[96] = {0};
RTC_DATA_ATTR bool lastWasNothingPlaying = true;


static void goToSleep(uint32_t seconds)
{
    Serial.printf("Deep sleeping for %u seconds...\n", seconds);
    delay(50);
    esp_sleep_enable_timer_wakeup((uint64_t)seconds * 1000000ULL);
    esp_deep_sleep_start();
}

void setup()
{
    Serial.begin(115200);
    delay(200);

    gui.begin();

    Serial.println("\n--- Inkplate Spotify Album Dashboard ---");

    NetworkFunctions net(
        WIFI_SSID, WIFI_PASSWORD,
        SPOTIFY_CLIENT_ID, SPOTIFY_CLIENT_SECRET,
        SPOTIFY_REFRESH_TOKEN
    );

    if (!net.connectWiFi())
    {
        goToSleep(POLL_SECONDS);
        return;
    }

    String accessToken;
    if (!net.spotifyRefreshAccessToken(accessToken))
    {
        goToSleep(POLL_SECONDS);
        return;
    }

    SpotifyNowPlaying now;
    if (!net.spotifyGetCurrentlyPlaying(accessToken, now))
    {
        goToSleep(POLL_SECONDS);
        return;
    }


    if (!now.isPlaying)
    {
        if (!lastWasNothingPlaying)
        {
            Serial.println("State changed: was playing -> now nothing playing. Updating display.");
            gui.renderNothingPlaying();
            lastWasNothingPlaying = true;
            lastAlbumId[0] = 0;
        }
        else
        {
            Serial.println("Still nothing playing. Skipping display update.");
        }

        goToSleep(POLL_SECONDS);
        return;
    }

    if (now.albumId.length() == 0)
    {
        Serial.println("Playing but albumId empty. Showing Nothing playing UI.");
        if (!lastWasNothingPlaying)
        {
            gui.renderNothingPlaying();
            lastWasNothingPlaying = true;
            lastAlbumId[0] = 0;
        }
        else
        {
            Serial.println("Still in nothing-playing UI state. Skipping display update.");
        }

        goToSleep(POLL_SECONDS);
        return;
    }

    bool albumSame = (strncmp(lastAlbumId, now.albumId.c_str(), sizeof(lastAlbumId) - 1) == 0);

    if (albumSame && !lastWasNothingPlaying)
    {
        Serial.println("Album unchanged since last wake. Skipping e-paper refresh.");
    }
    else
    {
        Serial.println("Album changed (or returning from nothing-playing). Updating display.");
        gui.renderAlbumScreen(now.albumName, now.artistName, now.trackName, now.imageUrl, now.progressMs, now.durationMs);
        Serial.println("prolazi 3");

        strncpy(lastAlbumId, now.albumId.c_str(), sizeof(lastAlbumId) - 1);
        Serial.println("prolazi 4");
        lastAlbumId[sizeof(lastAlbumId) - 1] = 0;
        Serial.println("prolazi 5");
        lastWasNothingPlaying = false;
    }

    goToSleep(POLL_SECONDS);
}

void loop()
{
    // Not used: device deep-sleeps at end of setup()
}


/**
 **************************************************
 * @file        Inkplate13SPECTRA_Spotify_Album.ino
 * @brief       Spotify album dashboard for Soldered Inkplate 13SPECTRA.
 *
 * @details     Demonstrates how to connect your Inkplate 13SPECTRA to Spotify's
 *              Web API service, fetch and parse the received JSON album data and
 *              display it on the e-paper display.
 *
 *              Before you start you need a Spotify account. The following process
 *              needs to be done only once:
 *              - Visit https://developer.spotify.com/ -> Dashboard
 *                * Create a new app, fill out the App name, App description,
 *                  Redirect URIs and APIs used fields.
 *                  Note: Redirect URIs can be a local address, but it needs to be
 *                  in the format http://127.0.0.1:8888/callback
 *                  Note: in the APIs used field select Web API.
 *                * Copy the Client ID and Client Secret and paste them into the
 *                  SPOTIFY_CLIENT_ID and SPOTIFY_CLIENT_SECRET variables.
 *              - Navigate to "src/spotify-token" and open token.js in a code
 *                editor of your choice, then fill out the CLIENT_ID,
 *                CLIENT_SECRET and REDIRECT_URI fields.
 *                * Open a terminal inside this folder and run `node token.js`.
 *                  This sends your API information to Spotify and generates the
 *                  last piece you need to use the Spotify API, the REFRESH_TOKEN.
 *                * Open the login page in a browser of your choosing, search for
 *                  "refresh_token" and copy it into the SPOTIFY_REFRESH_TOKEN
 *                  field.
 *              - You can now use the example!
 *
 * Requirements:
 * - Board:      Soldered Inkplate 13SPECTRA
 * - Hardware:   Inkplate 13SPECTRA, USB cable
 * - Extra:      WiFi Internet connection, Spotify account + developer app,
 *               Node.js for the token script
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 13SPECTRA"
 *    from Tools -> Board.
 * 2) Complete the Spotify developer app setup described above and fill in
 *    SPOTIFY_CLIENT_ID, SPOTIFY_CLIENT_SECRET and SPOTIFY_REFRESH_TOKEN.
 * 3) Enter your WiFi credentials in the sketch.
 * 4) Upload the sketch to Inkplate 13SPECTRA.
 * 5) The current album data is fetched and displayed on the e-paper display.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/13spectra/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2026-02-17
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

        strncpy(lastAlbumId, now.albumId.c_str(), sizeof(lastAlbumId) - 1);
        lastAlbumId[sizeof(lastAlbumId) - 1] = 0;
        lastWasNothingPlaying = false;
    }

    goToSleep(POLL_SECONDS);
}

void loop()
{
    // Not used: device deep-sleeps at end of setup()
}


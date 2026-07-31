/**
 **************************************************
 * @file        Inkplate10_Spotify_Dashboard.ino
 * @brief       Polls Spotify Web API for the currently playing track and shows
 *              an album-style dashboard on Inkplate 10, then deep-sleeps.
 *
 * @details     This example demonstrates how to build a low-power "now playing"
 *              dashboard using Spotify's Web API. On each wake cycle, the ESP32
 *              connects to WiFi, exchanges a stored Spotify refresh token for a
 *              short-lived access token, then queries the "currently playing"
 *              endpoint. The returned JSON is parsed to extract the playback
 *              state, album/artist/track metadata, progress, duration, and an
 *              album art image URL.
 *
 *              The UI is rendered in 3-bit grayscale mode (INKPLATE_3BIT) using
 *              a GUI helper that draws either an album screen (with album art
 *              and track information) or a "nothing playing" screen when
 *              playback is stopped/paused. Partial updates are not supported in
 *              grayscale mode, so any refresh performed is a full refresh. To
 *              reduce unnecessary e-paper refreshes, the sketch stores the last
 *              shown album ID and state in RTC memory (RTC_DATA_ATTR) and skips
 *              the display update if the album has not changed since the
 *              previous wake.
 *
 *              After handling the update decision, the ESP32 enters deep sleep
 *              for POLL_SECONDS. Deep sleep resets the ESP32, so the workflow
 *              repeats from setup() at each poll interval and only
 *              RTC_DATA_ATTR variables persist (lastAlbumId and
 *              lastWasNothingPlaying).
 *
 *              HTTPS handling and JSON parsing are implemented in the src/
 *              network layer; ensure certificate validation is handled
 *              appropriately - if the code uses insecure TLS settings, treat it
 *              as demo-only and prefer proper validation/pinning in production.
 *              Polling too frequently may increase power consumption and may be
 *              subject to Spotify API rate limits, so choose POLL_SECONDS
 *              accordingly. Keep client secrets and refresh tokens private; do
 *              not commit credentials to public repositories.
 *
 *              Expected output: an album dashboard showing album art (downloaded
 *              from a URL), album/artist/track text and playback progress if
 *              music is playing; a dedicated "nothing playing" screen otherwise;
 *              and Serial output indicating whether WiFi/token/API steps
 *              succeeded and whether the display refresh was performed or
 *              skipped.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 10
 * - Hardware:   Inkplate 10, USB cable (battery recommended for deployment)
 * - Extra:      WiFi Internet connection, Spotify account + developer app
 * - Serial:     115200 baud
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate10"
 *    from Tools -> Board.
 * 2) Create a Spotify Developer app and configure Redirect URI as instructed
 *    by the project docs/script (commonly a local callback URL). Set
 *    SPOTIFY_CLIENT_ID and SPOTIFY_CLIENT_SECRET.
 * 3) Use the provided token generation script in src/spotify-token to obtain a
 *    refresh token, then paste it into SPOTIFY_REFRESH_TOKEN.
 * 4) Fill in WiFi credentials (WIFI_SSID, WIFI_PASSWORD), set POLL_SECONDS in
 *    the project configuration (src/) to control the update rate, and upload
 *    the sketch to Inkplate 10.
 * 5) Open Serial Monitor at 115200 baud to observe token refresh/poll logs.
 * 6) The dashboard updates when the currently playing album changes; otherwise
 *    it skips the e-paper refresh to save time and power.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/10/quick-start-guide/
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


Inkplate display(INKPLATE_3BIT); 
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


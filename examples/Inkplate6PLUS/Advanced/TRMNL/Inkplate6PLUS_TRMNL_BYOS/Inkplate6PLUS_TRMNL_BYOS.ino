/**
 **************************************************
 * @file        Inkplate6PLUS_TRMNL_BYOS.ino
 * @brief       TRMNL BYOS client example for Soldered Inkplate 6PLUS.
 *
 * @details     Connects Inkplate 6PLUS to WiFi, registers with a TRMNL-compatible
 *              BYOS server via /api/setup, then polls /api/display on a loop,
 *              drawing whatever image the server returns and deep-sleeping
 *              between refreshes.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 6PLUS
 * - Library:    ArduinoJson (install via Library Manager)
 * - Server:     Terminus (TRMNL's official BYOS server) running via Docker
 *
 * ------------------------------------------------
 * Setting up the BYOS server (Terminus) via Docker
 * ------------------------------------------------
 * Terminus is TRMNL's official self-hosted "Bring Your Own Server" (BYOS)
 * implementation.
 *
 * 1) Install Docker:
 *      macOS:   brew install --cask docker
 *               open -a Docker
 *      Linux:   Use your distro's package manager, e.g.:
 *               sudo apt install docker.io docker-compose-plugin   (Debian/Ubuntu)
 *               sudo systemctl start docker
 *               sudo systemctl enable docker
 *      Windows: Install "Docker Desktop" from docker.com, then launch it
 *               (WSL2 backend required/recommended). Run the commands below
 *               from PowerShell, a WSL2 terminal, or Git Bash.
 *
 *      Then on any OS, confirm it's running:
 *      docker info          // confirms Docker is running
 *
 *      (Prefer a lighter CLI-only runtime instead of the Docker Desktop
 *      GUI app? On macOS/Linux you can use Colima:
 *        brew install colima docker   // macOS
 *        colima start)
 *
 * 2) Quick start (fastest way to try it, NOT for permanent use):
 *      macOS/Linux (bash):
 *        curl https://raw.githubusercontent.com/usetrmnl/terminus/refs/heads/main/scripts/docker/quick.sh | bash
 *
 *      Windows: the script is a bash script and needs a POSIX shell -
 *      run it inside WSL2, or inside Git Bash, using the same command:
 *        curl https://raw.githubusercontent.com/usetrmnl/terminus/refs/heads/main/scripts/docker/quick.sh | bash
 *
 *    This script is NOT idempotent - do not run it more than once, since
 *    your database credentials will differ each time. Once it finishes,
 *    open http://localhost:2300 in a browser and click "Register" to
 *    create your login.
 *
 *    For permanent/production use instead, clone + set up manually
 *    (same commands on macOS, Linux, WSL2, or Git Bash on Windows):
 *      git clone https://github.com/usetrmnl/terminus
 *      cd terminus
 *      bin/setup            // idempotent, safe to re-run
 *
 * 3) Find your server's LAN IP (so the Inkplate can reach it):
 *      macOS:   ipconfig getifaddr en0
 *               (or: ifconfig | grep "inet " | grep -v 127.0.0.1)
 *      Linux:   ip addr show   // look for inet under your active interface
 *               (or: hostname -I)
 *      Windows: ipconfig       // look for "IPv4 Address" under your
 *               active adapter (Wi-Fi or Ethernet)
 *
 *    Make sure this matches the API_URI value Terminus is using
 *    (check the .env file created during setup) - the device and the
 *    server must agree on the exact same host:port.
 *
 * 4) Register your device in the Terminus dashboard:
 *      Devices -> Add Device
 *      - Model: pick the closest match, or create a custom one under
 *        "Models" if your exact Inkplate isn't listed
 *      - MAC Address: your Inkplate's WiFi MAC (see deviceId below,
 *        printed to the e-paper display after WiFi connects)
 *      - Refresh Rate: how often (seconds) the device should poll
 *
 * 5) Point this sketch at your server:
 *      Set BYOS_SERVER below to "http://<server-ip>:2300"
 *      (no trailing slash)
 *
 * 6) Build actual screen content:
 *      Designs   -> create a Liquid/HTML template for what to display
 *      Screens   -> confirm the rendered PNG shows up
 *      Playlists -> add that screen to a playlist
 *      Devices   -> assign the playlist to your device
 *
 * Once all of the above is done, this sketch's doSetup()/doDisplay()
 * calls will fetch and draw whatever screen you've configured.
 *
 * ------------------------------------------------
 * How this example behaves once it's configured
 * ------------------------------------------------
 * First boot: the panel shows a "Connecting to WiFi..." banner followed by
 * the device's MAC address (needed for step 4 above). The sketch registers
 * with the server via /api/setup and stores the returned api_key in RTC
 * memory, where it survives deep sleep - so registration happens only once,
 * not on every wake. Only a full power loss clears the key, and even then
 * the server returns the same key for a known MAC, so the device re-pairs
 * itself automatically.
 *
 * Routine wakes: the device polls /api/display, sending the Access-Token
 * header plus telemetry the server can show (battery voltage, WiFi RSSI,
 * model, panel resolution, and what woke the device). It draws the returned
 * image and deep-sleeps for the refresh_rate the server specifies. If the
 * server is still serving the image that's already on the panel, the sketch
 * skips both the download and the costly e-paper refresh, and the banner is
 * not repainted either - the current image stays intact between wakes.
 *
 * Button refresh: pressing the WAKE button (GPIO36) while the device is
 * asleep wakes it immediately for an on-demand refresh, reported to the
 * server as Update-Source: button.
 *
 * Error recovery: WiFi and API failures never leave the sketch hung. Each
 * failure deep-sleeps and retries on an escalating schedule (see
 * WIFI_RETRY_TIMES / API_RETRY_TIMES below). If WiFi keeps failing, the
 * device eventually sleeps until the WAKE button is pressed so an
 * unreachable access point can't drain the battery; API errors settle into
 * a 15-minute polling fallback. A 202/500 "not ready" status from the
 * server is re-polled after a minute.
 *
 * @note        Quick start guide: Inkplate 6PLUS has no dedicated page yet,
 *              see https://docs.soldered.com/inkplate/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2021-02-11
 * @license     GNU GPL V3
 **************************************************/

#if !defined(ARDUINO_INKPLATE6PLUS) && !defined(ARDUINO_INKPLATE6PLUSV2)
#error "Wrong board selection for this example, please select e-radionica Inkplate 6Plus or Soldered Inkplate 6Plus in the boards menu."
#endif

#include "Inkplate.h"
#include <HTTPClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>

#define ssid "" // Your WiFi SSID
#define pass "" // Your WiFi password
#define BYOS_SERVER "http://YOUR_SERVER_IP:2300"

// WAKE button (same pin as the Inkplate6PLUS_Wake_Up_Button example): pressing
// it during deep sleep wakes the device for an immediate refresh.
#define WAKE_BUTTON GPIO_NUM_36

// How long to wait for a WiFi connection before backing off (ms)
#define WIFI_TIMEOUT_MS 30000

// Escalating backoff schedules (seconds). Once WIFI_RETRY_TIMES is exhausted
// the device sleeps until the WAKE button is pressed so a dead AP can't
// drain the battery; once API_RETRY_TIMES is exhausted it settles at
// API_FALLBACK_SLEEP.
const int WIFI_RETRY_TIMES[] = {60, 180, 300};
const int API_RETRY_TIMES[] = {15, 30, 60};
#define API_FALLBACK_SLEEP 900
#define NOT_READY_SLEEP 60 // short poll while the server reports not-ready (202/500)

Inkplate display(INKPLATE_1BIT);

String deviceId = "";

// Access-Token issued by /api/setup; preserved across deep sleep so the
// device only registers once. Lost only on full power loss, which is fine -
// the server returns the same key for a known MAC address.
RTC_DATA_ATTR char apiKey[64] = "";

// Last server-issued refresh cadence (seconds), reported back on the next
// request via the Refresh-Rate header
RTC_DATA_ATTR int lastRefreshRate = 900;

// Escalating backoff positions, preserved across deep sleep
RTC_DATA_ATTR int wifiRetryCount = 0;
RTC_DATA_ATTR int apiRetryCount = 0;

// Name of the image currently on the panel: e-paper keeps showing it through
// deep sleep, so an unchanged image needs no download or refresh at all
RTC_DATA_ATTR char lastImageName[256] = "";

double batteryVoltage = 0.0; // read before WiFi so radio current doesn't skew it

// Reported to the server via the Update-Source header
const char *wakeupSource()
{
    switch (esp_sleep_get_wakeup_cause())
    {
    case ESP_SLEEP_WAKEUP_TIMER:
        return "timer";
    case ESP_SLEEP_WAKEUP_EXT0:
        return "button";
    default:
        return "powercycle";
    }
}

void setup()
{
    Serial.begin(115200);
    display.begin();

    // Read the battery BEFORE the radio is energized so WiFi current doesn't
    // depress the voltage reported to the server.
    batteryVoltage = display.readBattery();

    // Painting the panel here would wipe the current image between routine
    // refreshes, so only show the connect/pairing banner while the device
    // isn't registered yet (it displays the MAC address needed to add the
    // device on the server).
    bool showBanner = (strlen(apiKey) == 0);
    if (showBanner)
    {
        display.clearDisplay();
        display.setTextSize(2);
        display.setCursor(0, 0);
        display.setTextColor(BLACK, WHITE);
        display.println("Connecting to WiFi...");
        display.display();
    }

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, pass);
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED)
    {
        if (millis() - start > WIFI_TIMEOUT_MS)
        {
            if (showBanner)
            {
                display.println("WiFi failed; will retry with backoff");
                display.partialUpdate();
            }
            wifiErrorSleep(); // deep-sleeps and never returns
        }
        delay(1000);
        if (showBanner)
        {
            display.print('.');
            display.partialUpdate();
        }
    }
    wifiRetryCount = 0; // connected: restart the escalation from the top

    deviceId = WiFi.macAddress(); // TRMNL identifies devices by MAC address

    if (showBanner)
    {
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("Connected. ID: ");
        display.print(deviceId);
        display.display();
    }

    if (strlen(apiKey) == 0)
        doSetup(); // Register with the BYOS server (first boot only)
    doDisplay();   // Fetch and show the first screen
}

void loop()
{
    // Everything happens in setup() + deep sleep below;
    // ESP32 wakes up, runs setup() again, fetches next screen.
}

void doSetup()
{
    HTTPClient http;
    String url = String(BYOS_SERVER) + "/api/setup";

    if (http.begin(url))
    {
        http.addHeader("ID", deviceId);
        int code = http.GET();
        if (code == 200)
        {
            String payload = http.getString();
            Serial.println("Setup response: " + payload);

            JsonDocument doc;
            if (deserializeJson(doc, payload) == DeserializationError::Ok)
            {
                const char *key = doc["api_key"] | "";
                if (strlen(key) > 0)
                {
                    strncpy(apiKey, key, sizeof(apiKey) - 1);
                    apiKey[sizeof(apiKey) - 1] = '\0';
                    Serial.println("Registered with server; api_key stored");
                }
            }
        }
        else
        {
            Serial.printf("Setup request failed, HTTP %d\n", code);
        }
        http.end();
    }
}

// image.draw() picks its decoder from the URL's file extension, so a query
// string or extension-less URL would fail without even downloading; reject
// those up front with a clear log message.
bool hasDrawableExtension(const char *url)
{
    String u(url);
    String ext = u.substring(u.lastIndexOf('.') + 1);
    ext.toLowerCase();
    return ext == "bmp" || ext == "dib" || ext == "png" || ext == "jpg" || ext == "jpeg";
}

void doDisplay()
{
    HTTPClient http;
    String url = String(BYOS_SERVER) + "/api/display";

    if (!http.begin(url))
    {
        Serial.println("http.begin() failed");
        apiErrorSleep(); // deep-sleeps and never returns
    }

    http.addHeader("ID", deviceId);
    if (strlen(apiKey) > 0)
        http.addHeader("Access-Token", apiKey);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("Update-Source", wakeupSource());
    http.addHeader("Refresh-Rate", String(lastRefreshRate));
    http.addHeader("Battery-Voltage", String(batteryVoltage, 2));
    http.addHeader("RSSI", String(WiFi.RSSI()));
    http.addHeader("Model", "inkplate_6plus");
    http.addHeader("Width", String(display.width()));
    http.addHeader("Height", String(display.height()));
    http.addHeader("Image-Cached", strlen(lastImageName) > 0 ? "true" : "false");

    int code = http.GET();
    if (code <= 0 || code >= 400)
    {
        Serial.printf("Display request failed, HTTP %d\n", code);
        http.end();
        apiErrorSleep();
    }

    String payload = http.getString();
    http.end();
    Serial.println("Display response: " + payload);

    JsonDocument doc;
    if (deserializeJson(doc, payload) != DeserializationError::Ok)
    {
        Serial.println("JSON parse failed");
        apiErrorSleep();
    }

    long refreshRate = doc["refresh_rate"].as<long>(); // integer, not string
    if (refreshRate <= 0)
        refreshRate = 900;
    lastRefreshRate = (int)refreshRate;

    // The device API reports state in the "status" body field: 0 = image
    // ready, 202 = registered but no screen assigned yet, 500 = server error.
    int status = doc["status"].as<int>();
    if (status != 0)
    {
        apiRetryCount = 0; // the server answered, so the API path is healthy
        if (status == 202 || status == 500)
        {
            Serial.printf("Server not ready (status %d); polling again shortly\n", status);
            goToSleep(NOT_READY_SLEEP);
        }
        Serial.printf("Unknown status %d; keeping current image\n", status);
        goToSleep(refreshRate);
    }
    apiRetryCount = 0;

    String imageUrl = doc["image_url"].as<String>();
    bool updateFirmware = doc["update_firmware"].as<bool>();

    if (imageUrl.length() == 0)
    {
        Serial.println("No image_url in response");
        apiErrorSleep();
    }

    // Prefer the stable filename as the identity of the image, falling back
    // to the URL
    const char *filename = doc["filename"] | "";
    String imageName = strlen(filename) > 0 ? String(filename) : imageUrl;

    if (imageName == lastImageName)
    {
        // The e-paper still shows this exact image; skip the download and
        // the costly full refresh entirely.
        Serial.println("Image unchanged; skipping redraw");
        goToSleep(refreshRate);
    }

    if (!hasDrawableExtension(imageUrl.c_str()))
    {
        Serial.println("image_url has no drawable extension: " + imageUrl);
        apiErrorSleep();
    }

    display.clearDisplay();
    bool ok = display.image.draw(imageUrl.c_str(), 0, 0, true, false);

    if (!ok)
    {
        display.setCursor(0, 0);
        display.print("Failed to draw image from URL");
        display.display();
        apiErrorSleep(); // lastImageName is untouched, so next wake retries
    }

    display.display();
    strncpy(lastImageName, imageName.c_str(), sizeof(lastImageName) - 1);
    lastImageName[sizeof(lastImageName) - 1] = '\0';

    if (updateFirmware)
    {
        Serial.println("Firmware update flagged - not implemented, skipping.");
    }

    goToSleep(refreshRate);
}

void goToSleep(long seconds)
{
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    esp_sleep_enable_timer_wakeup((uint64_t)seconds * 1000000ULL);
    esp_sleep_enable_ext0_wakeup(WAKE_BUTTON, LOW);
    esp_deep_sleep_start();
}

// Sleep with no timer so only a WAKE press revives the device
void goToSleepButtonOnly()
{
    WiFi.disconnect(true);
    WiFi.mode(WIFI_OFF);
    esp_sleep_enable_ext0_wakeup(WAKE_BUTTON, LOW);
    esp_deep_sleep_start();
}

// Escalating sleep after an API/HTTP/parse failure
void apiErrorSleep()
{
    int seconds;
    if (apiRetryCount < (int)(sizeof(API_RETRY_TIMES) / sizeof(API_RETRY_TIMES[0])))
        seconds = API_RETRY_TIMES[apiRetryCount++];
    else
        seconds = API_FALLBACK_SLEEP;
    goToSleep(seconds);
}

// Escalating sleep after a WiFi connect failure
void wifiErrorSleep()
{
    if (wifiRetryCount >= (int)(sizeof(WIFI_RETRY_TIMES) / sizeof(WIFI_RETRY_TIMES[0])))
        goToSleepButtonOnly();
    goToSleep(WIFI_RETRY_TIMES[wifiRetryCount++]);
}

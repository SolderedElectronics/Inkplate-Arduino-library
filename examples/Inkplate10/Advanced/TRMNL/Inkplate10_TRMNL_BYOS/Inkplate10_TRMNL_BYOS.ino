/**
 **************************************************
 * @file        Inkplate10_TRMNL_BYOS.ino
 * @brief       TRMNL BYOS client example for Soldered Inkplate 10.
 *
 * @details     Connects Inkplate 10 to WiFi, registers with a TRMNL-compatible
 *              BYOS server via /api/setup, then polls /api/display on a loop,
 *              drawing whatever image the server returns and deep-sleeping
 *              between refreshes.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 10
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
 * @author      Soldered
 * @date        2021-02-11
 * @license     GNU GPL V3
 **************************************************/

#if !defined(ARDUINO_INKPLATE10) && !defined(ARDUINO_INKPLATE10V2)
#error "Wrong board selection for this example, please select e-radionica Inkplate10 or Soldered Inkplate10 in the boards menu."
#endif

#include "Inkplate.h"
#include <HTTPClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>

#define ssid "" // Your WiFi SSID
#define pass "" // Your WiFi password
#define BYOS_SERVER "http://YOUR_SERVER_IP:2300"

// WAKE button (same pin as the Inkplate10_Wake_Up_Button example): pressing
// it during deep sleep wakes the device for an immediate refresh.
#define WAKE_BUTTON GPIO_NUM_36

Inkplate display(INKPLATE_1BIT);

String deviceId = "";

// Access-Token issued by /api/setup; preserved across deep sleep so the
// device only registers once. Lost only on full power loss, which is fine -
// the server returns the same key for a known MAC address.
RTC_DATA_ATTR char apiKey[64] = "";

// Last server-issued refresh cadence (seconds), reported back on the next
// request via the Refresh-Rate header
RTC_DATA_ATTR int lastRefreshRate = 900;

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

    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(0, 0);
    display.setTextColor(BLACK, WHITE);
    display.println("Connecting to WiFi...");
    display.display();

    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        display.print('.');
        display.partialUpdate();
    }

    deviceId = WiFi.macAddress(); // TRMNL identifies devices by MAC address

    display.clearDisplay();
    display.setCursor(0, 0);
    display.print("Connected. ID: ");
    display.print(deviceId);
    display.display();

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

void doDisplay()
{
    HTTPClient http;
    String url = String(BYOS_SERVER) + "/api/display";

    if (http.begin(url))
    {
        http.addHeader("ID", deviceId);
        if (strlen(apiKey) > 0)
            http.addHeader("Access-Token", apiKey);
        http.addHeader("Content-Type", "application/json");
        http.addHeader("Update-Source", wakeupSource());
        http.addHeader("Refresh-Rate", String(lastRefreshRate));
        http.addHeader("Battery-Voltage", String(batteryVoltage, 2));
        http.addHeader("RSSI", String(WiFi.RSSI()));
        http.addHeader("Model", "inkplate_10");
        http.addHeader("Width", String(display.width()));
        http.addHeader("Height", String(display.height()));

        int code = http.GET();
        if (code > 0)
        {
            String payload = http.getString();
            Serial.println("Display response: " + payload);

            JsonDocument doc;
            DeserializationError err = deserializeJson(doc, payload);

            if (err)
            {
                Serial.println("JSON parse failed");
                http.end();
                return;
            }

            String imageUrl = doc["image_url"].as<String>();
            long refreshRate = doc["refresh_rate"].as<long>(); // integer, not string
            bool updateFirmware = doc["update_firmware"].as<bool>();

            if (refreshRate <= 0)
                refreshRate = 900;
            lastRefreshRate = (int)refreshRate;

            http.end();

            display.clearDisplay();
            bool ok = display.image.draw(imageUrl.c_str(), 0, 0, true, false);

            if (!ok)
            {
                display.setCursor(0, 0);
                display.print("Failed to draw image from URL");
            }

            display.display();

            if (updateFirmware)
            {
                Serial.println("Firmware update flagged - not implemented, skipping.");
            }

            goToSleep(refreshRate);
        }
        http.end();
    }
}

void goToSleep(long seconds)
{
    esp_sleep_enable_timer_wakeup((uint64_t)seconds * 1000000ULL);
    esp_sleep_enable_ext0_wakeup(WAKE_BUTTON, LOW);
    esp_deep_sleep_start();
}

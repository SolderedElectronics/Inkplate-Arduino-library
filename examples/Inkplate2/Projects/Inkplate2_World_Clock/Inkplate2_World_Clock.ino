/**
 **************************************************
 * @file        Inkplate2_World_Clock.ino
 * @brief       Two-city analog world clock for Inkplate 2 using TimeAPI over
 *              WiFi, refreshed from deep sleep.
 *
 * @details     Draws two analog clock faces side by side, one per configured
 *              city (city1 and city2), with the hour hand in black and the minute
 *              hand in red, plus an AM/PM label and the city name underneath.
 *
 *              On boot the sketch runs a short display self-test, connects to
 *              WiFi, prints the current NTP time to Serial, fetches the timezone
 *              list from TimeAPI and then requests the local time for each city
 *              through the NetworkFunctions helper. If both city lookups fail, a
 *              "No city time data!" message is shown instead of a blank screen.
 *              After drawing, the ESP32 sleeps for TIME_TO_SLEEP seconds (300 s
 *              by default) and repeats on wake; a failed WiFi connection sleeps
 *              for DELAY_WIFI_RETRY_SECONDS and retries.
 *
 *              Deep sleep restarts the ESP32 on every wake, so all logic lives in
 *              setup() and loop() stays empty. The display uses the Inkplate 2
 *              tri-color palette (BLACK/WHITE/RED) with a full refresh.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 2
 * - Hardware:   Inkplate 2, USB cable
 * - Extra:      WiFi connection + Internet access (TimeAPI + NTP)
 * - Serial:     115200 baud (recommended for debugging)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate2"
 *    from Tools -> Board.
 * 2) Enter your WiFi SSID/password (ssid, pass).
 * 3) Set city1 and city2 to the cities you want to track, and optionally adjust
 *    TIME_TO_SLEEP and DELAY_WIFI_RETRY_SECONDS.
 * 4) Upload the sketch and open Serial Monitor at 115200 baud.
 * 5) Both clock faces are drawn, then the board deep-sleeps and refreshes every
 *    TIME_TO_SLEEP seconds.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/2/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @license     GNU GPL V3
 **************************************************/
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Soldered Inkplate2 in the boards menu."
#endif

#include "Inkplate.h"
#include "NetworkFunctions.h"
#include "SourceSansPro_Regular6pt7b.h"
#include "SourceSansPro_Regular8pt7b.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define uS_TO_S_FACTOR 1000000ULLULL
#define TIME_TO_SLEEP  300
#define DELAY_WIFI_RETRY_SECONDS 10

NetworkFunctions network;
Inkplate display;

char ssid[] = "";
char pass[] = "";

int hours;
int minutes;

const char city1[] = "Zagreb";
const char city2[] = "Lima";

void drawTime(uint16_t x_pos, uint16_t y_pos, bool pm, const char *city);

void setup()
{
    Serial.begin(115200);
    display.begin();

    // --- DISPLAY SELF-TEST (so we know the panel can refresh) ---
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(INKPLATE2_BLACK, INKPLATE2_WHITE);
    display.setCursor(5, 15);
    display.print("DISPLAY OK - starting...");
    display.drawRect(0, 0, display.width(), display.height(), INKPLATE2_BLACK);
    display.display();
    delay(2000); // let the refresh finish before WiFi / sleep

    // --- WIFI ---
    if (!display.connectWiFi(ssid, pass, WIFI_TIMEOUT, true))
    {
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print(F("Unable to connect to "));
        display.println(F(ssid));
        display.println(F("Please check SSID and PASS!"));
        display.display();

        esp_sleep_enable_timer_wakeup((uint64_t)DELAY_WIFI_RETRY_SECONDS * uS_TO_S_FACTOR);
        esp_deep_sleep_start();
    }

    // IMPORTANT: init NetworkFunctions TLS client
    network.begin(ssid, pass);

    // NTP debug
    time_t nowSec;
    struct tm timeInfo;
    display.getNTPEpoch(&nowSec);
    gmtime_r(&nowSec, &timeInfo);
    Serial.print(F("Current time: "));
    Serial.print(asctime(&timeInfo));

    // --- TIMEZONES ---
    if (!network.getAllCities())
    {
        Serial.println("Failed to fetch timezone list from TimeAPI.");

        display.clearDisplay();
        display.setCursor(0, 0);
        display.println("TimeAPI error!");
        display.println("Can't fetch timezones.");
        display.display();
        delay(3000);

        esp_sleep_enable_timer_wakeup((uint64_t)TIME_TO_SLEEP * uS_TO_S_FACTOR);
        esp_deep_sleep_start();
    }

    // --- DRAW CLOCKS ---
    display.clearDisplay(); // CRITICAL: start from clean buffer

    bool ok1 = network.getData((char *)city1, &hours, &minutes);
    Serial.printf("%s: %s\n", city1, ok1 ? "OK" : "FAIL");
    if (ok1)
    {
        Serial.printf("%s time %02d:%02d\n", city1, hours, minutes);
        drawTime(17, 1, hours >= 12, city1);
    }

    bool ok2 = network.getData((char *)city2, &hours, &minutes);
    Serial.printf("%s: %s\n", city2, ok2 ? "OK" : "FAIL");
    if (ok2)
    {
        Serial.printf("%s time %02d:%02d\n", city2, hours, minutes);
        drawTime(115, 1, hours >= 12, city2);
    }

    // If both failed, show message instead of a blank screen
    if (!ok1 && !ok2)
    {
        display.setCursor(5, 20);
        display.setFont(&SourceSansPro_Regular8pt7b);
        display.setTextColor(INKPLATE2_BLACK, INKPLATE2_WHITE);
        display.print("No city time data!");
    }

    display.display();
    delay(5000); // IMPORTANT: give e-ink time before deep sleep

    Serial.println("Going to sleep, bye!");
    esp_sleep_enable_timer_wakeup((uint64_t)TIME_TO_SLEEP * uS_TO_S_FACTOR);
    esp_deep_sleep_start();
}

void loop() {}

void drawTime(uint16_t x_pos, uint16_t y_pos, bool pm, const char *city)
{
    char *city_name = network.getFullCityName((char *)city);
    uint16_t w = 80;

    float xStart[12], yStart[12], xEnd[12], yEnd[12];

    display.drawCircle(x_pos + w / 2, y_pos + w / 2, w / 2, INKPLATE2_BLACK);
    display.drawCircle(x_pos + w / 2, y_pos + w / 2, w / 2 + 1, INKPLATE2_BLACK);

    display.drawThickLine(x_pos + w / 2, y_pos, x_pos + w / 2, y_pos + 5, INKPLATE2_BLACK, 2);
    display.drawThickLine(x_pos + w, y_pos + w / 2, x_pos + w - 5, y_pos + w / 2, INKPLATE2_BLACK, 2);
    display.drawThickLine(x_pos + w / 2, y_pos + w, x_pos + w / 2, y_pos + w - 5, INKPLATE2_BLACK, 2);
    display.drawThickLine(x_pos, y_pos + w / 2, x_pos + 5, y_pos + w / 2, INKPLATE2_BLACK, 2);

    for (int i = 0; i < 12; i++)
    {
        float angle_rad = 30.0f * i * (float)M_PI / 180.0f;

        xStart[i] = x_pos + (cos(angle_rad) * w / 2) + w / 2;
        yStart[i] = y_pos + (sin(angle_rad) * w / 2) + w / 2;

        xEnd[i] = x_pos + (cos(angle_rad) * (w * 0.85f) / 2) + w / 2;
        yEnd[i] = y_pos + (sin(angle_rad) * (w * 0.85f) / 2) + w / 2;

        display.drawThickLine(xStart[i], yStart[i], xEnd[i], yEnd[i], INKPLATE2_BLACK, 1);
    }

    int x_minute = x_pos + w / 2 + 30 * sin((minutes / 60.0f) * 2.0f * (float)M_PI);
    int y_minute = y_pos + w / 2 - 30 * cos((minutes / 60.0f) * 2.0f * (float)M_PI);

    float h = (hours % 12) / 12.0f + minutes / 720.0f;
    int x_hour = x_pos + w / 2 + 22 * sin(h * 2.0f * (float)M_PI);
    int y_hour = y_pos + w / 2 - 22 * cos(h * 2.0f * (float)M_PI);

    display.drawThickLine(x_pos + w / 2, y_pos + w / 2, x_minute, y_minute, INKPLATE2_RED, 2);
    display.drawThickLine(x_pos + w / 2, y_pos + w / 2, x_hour, y_hour, INKPLATE2_BLACK, 3);

    display.fillCircle(x_pos + w / 2, y_pos + w / 2, 5, INKPLATE2_BLACK);

    display.setTextSize(1);
    display.setFont(&SourceSansPro_Regular8pt7b);
    display.setTextColor(INKPLATE2_BLACK, INKPLATE2_WHITE);

    const char *nameToPrint = city_name ? city_name : city;
    const char *slash = strstr(nameToPrint, "/");
    const char *temp_city_name = slash ? (slash + 1) : nameToPrint;

    display.setCursor(x_pos + 40 - strlen(temp_city_name) * 5, 100);
    for (uint8_t cnt = 0; temp_city_name[cnt] != '\0'; cnt++)
        display.print(temp_city_name[cnt] == '_' ? ' ' : temp_city_name[cnt]);

    display.setCursor(x_pos + 32, y_pos + 62);
    display.setFont(&SourceSansPro_Regular6pt7b);
    display.print(pm ? "PM" : "AM");
}
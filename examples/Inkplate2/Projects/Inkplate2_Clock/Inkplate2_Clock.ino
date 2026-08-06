/**
 **************************************************
 * @file        Inkplate2_Clock.ino
 * @brief       Multi-style clock demo (digital, binary, or analog) using NTP
 *              time sync, then deep sleep between updates.
 *
 * @details     This example demonstrates three different clock renderings on
 *              Inkplate 2:
 *              - Digital clock: 4 large 7-segment digits (HH:MM) drawn from
 *                bitmap assets.
 *              - Binary clock: hours, minutes, day, and month shown as binary
 *                bits using filled/outlined circles.
 *              - Analog clock: clock face with hour and minute hands.
 *
 *              On boot, the sketch connects to WiFi and fetches the current time
 *              from an NTP server (epoch/time structure). The selected clock
 *              mode is chosen by the MODE variable. After drawing the clock
 *              into the framebuffer, the sketch performs a full refresh and
 *              then enters deep sleep for TIME_TO_SLEEP seconds (default: 5
 *              minutes) to save power. Because deep sleep resets the ESP32,
 *              the program restarts from setup() on every wake cycle, so keep
 *              logic in setup() and leave loop() empty.
 *
 *              Display mode is 1-bit with the Inkplate 2 tri-color palette
 *              (BLACK/WHITE/RED) and this sketch uses a full refresh (display())
 *              for each update cycle. Bitmap digits for the digital clock must be
 *              present and generated in an Inkplate-compatible format (typically
 *              via Soldered Image Converter). Time is obtained from NTP on each
 *              wake, so if WiFi is unavailable the clock cannot update (add
 *              fallback/RTC handling for production use).
 *
 *              Expected output: MODE 0 (digital) shows large HH:MM digits in red
 *              with a black colon separator; MODE 1 (binary) shows four columns
 *              with HH, MM, DD and MM (month) as binary circles with labels and
 *              bit value markers; MODE 2 (analog) shows a clock face with a black
 *              hour hand and red minute hand. On WiFi failure an error message is
 *              shown, followed by a short sleep and retry.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 2
 * - Hardware:   Inkplate 2, USB cable
 * - Extra:      WiFi connection + Internet access (NTP)
 * - Serial:     115200 baud (recommended for debugging)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate2"
 *    from Tools -> Board.
 * 2) Enter your WiFi SSID/password (ssid, pass) and set your local timeZone
 *    (UTC offset).
 * 3) Choose the clock style by setting MODE (0 = digital, 1 = binary,
 *    2 = analog), and optionally TIME_TO_SLEEP (seconds) and
 *    DELAY_WIFI_RETRY_SECONDS.
 * 4) Upload the sketch to Inkplate 2 and open Serial Monitor at 115200 baud.
 * 5) The clock is drawn and the device enters deep sleep.
 * 6) The device wakes every TIME_TO_SLEEP seconds, updates the time, redraws,
 *    and goes back to sleep.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/2/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2022-03-30
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Soldered Inkplate2 in the boards menu."
#endif

#include "Inkplate.h" // Include Inkplate library to the sketch

#include "NetworkFunctions.h" // Our networking functions, declared in Network.cpp

NetworkFunctions network; // Create network object for WiFi and HTTP functions

Inkplate display; // Create Inkplate library object

//---------- CHANGE HERE  -------------:

// Change mode of clock
// 0 is digital clock
// 1 is binary clock
// 2 is analog clock
uint8_t MODE = 1;

// Adjust your time zone, 2 means UTC+2
int timeZone = 2;

// Put in your ssid and password
char ssid[] = "";
char pass[] = "";
#define DELAY_WIFI_RETRY_SECONDS 10
//----------------------------------

// Bitmaps for 7 segment display. Converted using Inkplate Image Converter https://tools.soldered.com/tools/image-converter/
#include "includes/eight.h"
#include "includes/five.h"
#include "includes/four.h"
#include "includes/nine.h"
#include "includes/one.h"
#include "includes/seven.h"
#include "includes/six.h"
#include "includes/three.h"
#include "includes/two.h"
#include "includes/zero.h"

#define uS_TO_S_FACTOR 1000000ULL // Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP  300     // How long ESP32 will be in deep sleep (in seconds)

// Array for digital display 7 segment numbers bitmaps
const uint8_t *numbers[] = {zero, one, two, three, four, five, six, seven, eight, nine};

time_t timeEpoch; // Variable to store epoch

struct tm t; // Structure that contains time info

void setup()
{
    // Begin serial communitcation
    Serial.begin(115200);

    // Initialize inkplate libarry and set text settings
    display.begin();
    display.setTextWrap(true);
    display.setTextColor(INKPLATE2_BLACK, INKPLATE2_WHITE);

    // Our begin function
    if (!display.connectWiFi(ssid, pass, WIFI_TIMEOUT, true))
    {
        //Can't connect to netowrk
        // Clear display for the error message
        display.clearDisplay();
        // Set the font size;
        display.setTextSize(1);
        // Set the cursor positions and print the text.
        display.setCursor(0, 0);
        display.print(F("Unable to connect to "));
        display.println(F(ssid));
        display.println(F("Please check SSID and PASS!"));
        // Display the error message on the Inkplate and go to deep sleep
        display.display();
        esp_sleep_enable_timer_wakeup(1000L * DELAY_WIFI_RETRY_SECONDS);
        (void)esp_deep_sleep_start();
    }

    // Get the correct epoch from NTP server
    Serial.println("Getting time...");
    display.getNTPEpoch(&timeEpoch, timeZone);
    Serial.println(gmtime_r(&timeEpoch, &t));
    
    drawTime(); // Call function drawTime

    display.display(); // Display content from buffer on Inkplate

    // Go to sleep before checking again
    // rtc_gpio_isolate(GPIO_NUM_12);   // Isolate/disable GPIO12 on ESP32 (only to reduce power consumption in sleep)
    esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR); // Activate wake-up timer
         
    esp_deep_sleep_start();       // Put ESP32 into deep sleep. Program stops here.
}

void loop()
{
    // Never here! If you are using deep sleep, the whole program should be in setup() because the board restarts each
    // time. loop() must be empty!
}
// Function to draw time
void drawTime()
{
    // Just draw time
    int i = 1;

    // Save current date string, more about it in Network.cpp
    network.getTime(&t, timeZone);

    // Clear the frame buffer of the epaper.
    display.clearDisplay();

    // Choose what type of clock is going to be displayed (Digital, binary or analog).
    // Selection is made by modifiying MODE variable at the start of the example.
    switch (MODE)
    {
    case 0: // Digital clock
        display.drawBitmap(0, 10, numbers[t.tm_hour / 10], 48, 84,
                           INKPLATE2_RED); // Get first number of hours and draw it
        display.drawBitmap(50, 10, numbers[t.tm_hour % 10], 48, 84,
                           INKPLATE2_RED); // Get second number of hours and draw it
        display.drawBitmap(112, 10, numbers[t.tm_min / 10], 48, 84,
                           INKPLATE2_RED); // Get first number of minutes and draw it
        display.drawBitmap(162, 10, numbers[t.tm_min % 10], 48, 84,
                           INKPLATE2_RED); // Get second number of minutes and draw it
        display.fillRect(101, 32, 8, 8, INKPLATE2_BLACK);
        display.fillRect(101, 64, 8, 8, INKPLATE2_BLACK); // Draw rectangles to separate hours and minutes
        break;
    case 1: // Binary clock
        uint8_t buf1, buf2;
        for (int i = 0; i < 4; i++)
        {
            switch (i)
            {
            case 0: // In first iteration draw hours
                buf1 = t.tm_hour / 10;
                buf2 = t.tm_hour % 10;
                break;
            case 1: // In second iteration draw minutes
                buf1 = t.tm_min / 10;
                buf2 = t.tm_min % 10;
                break;
            case 2: // In third iteration draw day
                buf1 = t.tm_mday / 10;
                buf2 = t.tm_mday % 10;
                break;
            case 3: // In fourth iteration draw month
                buf1 = (t.tm_mon + 1) / 10;
                buf2 = (t.tm_mon + 1) % 10;
                break;
            }
            for (int j = 3; j >= 0; j--)
            {
                if (buf2 & (1 << (3 - j))) // Compare numbers bitwise, if they match, draw red circle for that bit, if
                                           // not draw black outline of circle
                {
                    display.fillCircle(30 + 50 * i, 10 + 16 * j, 6, INKPLATE2_RED);
                }
                else
                {
                    display.drawCircle(30 + 50 * i, 10 + 16 * j, 6, INKPLATE2_BLACK);
                }
                if (j > 0) // Do not do this if j is 0
                {
                    if (buf1 & (1 << (3 - j))) // Compare numbers bitwise, if they match, draw red circle for that bit,
                                               // if not draw black outline of circle
                    {
                        display.fillCircle(14 + 50 * i, 10 + 16 * j, 6,
                                           INKPLATE2_RED); // "Turn on" the bit in the clock
                    }
                    else
                    {
                        display.drawCircle(14 + 50 * i, 10 + 16 * j, 6,
                                           INKPLATE2_BLACK); // "Turn off" the bit in the binary clock
                    }
                }
            }
            // This block of code draws informations what which part of screen represents
            display.setCursor(10, 72);
            display.setTextSize(2);
            display.setTextColor(INKPLATE2_BLACK, INKPLATE2_WHITE);
            display.print("HH");
            display.setCursor(62, 72);
            display.print("MM");
            display.setCursor(112, 72);
            display.print("DD");
            display.setCursor(162, 72);
            display.print("MM");
            display.fillRect(45, 21, 5, 5, INKPLATE2_BLACK);
            display.fillRect(45, 44, 5, 5, INKPLATE2_BLACK);
            display.fillRect(145, 58, 5, 5, INKPLATE2_BLACK);

            // Draw number that represents values of circles
            display.setTextSize(1);
            display.setCursor(190, 7);
            display.print("8");
            display.setCursor(190, 23);
            display.print("4");
            display.setCursor(190, 39);
            display.print("2");
            display.setCursor(190, 55);
            display.print("1");

            display.drawLine(100, 0, 100, 104, INKPLATE2_BLACK);

            // Draw number that represents values of circles
            display.setTextSize(1);
            display.setCursor(90, 7);
            display.print("8");
            display.setCursor(90, 23);
            display.print("4");
            display.setCursor(90, 39);
            display.print("2");
            display.setCursor(90, 55);
            display.print("1");
        }
        break;
    case 2: // Analog clock

        // Draw face of the clock (actually outline)
        display.drawCircle(106, 52, 50, 1); // Draw outer circles
        display.drawCircle(106, 52, 51, 1);

        // Draw lines that represents 5, 10, 20, 25, 35, 40, 50, 55 on the clock dial face
        display.drawThickLine(63, 27, 67, 30, 1, 1);
        display.drawThickLine(145, 74, 150, 77, 1, 1);
        display.drawThickLine(63, 77, 67, 75, 1, 1);
        display.drawThickLine(145, 30, 149, 27, 1, 1);
        display.drawThickLine(81, 95, 83, 91, 1, 1);
        display.drawThickLine(129, 13, 131, 9, 1, 1);
        display.drawThickLine(81, 9, 83, 13, 1, 1);
        display.drawThickLine(129, 91, 131, 95, 1, 1);

        // Draw numbers 3, 6, 9, 12 on the dial face.
        display.setTextSize(1);
        display.setCursor(150, 49);
        display.println('3');
        display.setCursor(103, 92);
        display.println('6');
        display.setCursor(58, 49);
        display.println('9');
        display.setCursor(101, 6);
        display.println("12");

        // Draw dot in middle
        display.fillCircle(106, 52, 5, 1);

        // Now calculat the angle of each hand of the clock and draw it on display.
        int x_minute, y_minute, x_hour, y_hour;

        // Calculations for x and y end position of the line (start is in the middle of the clock)
        x_minute = 106 + 40 * (float)sin((t.tm_min / (float)60) * 2 * (float)3.14);
        y_minute = 52 - 40 * (float)cos((t.tm_min / (float)60) * 2 * (float)3.14);

        // Do simillar calculations for hour hand
        x_hour = 106 + 30 * sin((t.tm_hour / (float)12 + t.tm_min / (float)720) * 2 * (float)3.14);
        y_hour = 52 - 30 * cos((t.tm_hour / (float)12 + t.tm_min / (float)720) * 2 * (float)3.14);

        // Draw them on the dispaly!
        display.drawThickLine(106, 52, x_minute, y_minute, INKPLATE2_RED, 2); // Hand for minutes
        display.drawThickLine(106, 52, x_hour, y_hour, INKPLATE2_BLACK, 3);   // Hand for hours
        break;
    }
}

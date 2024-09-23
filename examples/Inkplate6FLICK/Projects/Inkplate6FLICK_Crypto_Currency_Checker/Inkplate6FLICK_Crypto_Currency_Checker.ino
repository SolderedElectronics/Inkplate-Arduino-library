/*
   Inkplate6FLICK_Crypto_Currency_Checker example for Soldered Inkplate 6FLICK
   For this example you will need only USB cable and Inkplate 6FLICK.
   Select "Soldered Inkplate 6FLICK" from Tools -> Board menu.
   Don't have "Soldered Inkplate 6FLICK" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example will show you how you can use Inkplate 6FLICK to display API data.
   Here we use Coingecko API to get latest cryptocurrency prices and display
   them on the Inkplate screen. Simply tap the currency you want to see!

   IMPORTANT:
   Have ArduinoJSON installed in your Arduino libraries, download here: https://arduinojson.org/

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   15 March 2024 by Soldered

   In order to convert your images into a format compatible with Inkplate
   use the Soldered Image Converter available at:
   https://github.com/SolderedElectronics/Soldered-Image-Converter/releases
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE6FLICK
#error "Wrong board selection for this example, please select Soldered Inkplate 6 FLICK"
#endif

#include "Crypto_UI.h"
#include "Icons/bnc.h"
#include "Icons/btc.h"
#include "Icons/doge.h"
#include "Icons/dollar.h"
#include "Icons/eth.h"
#include "Icons/euro.h"
#include "Icons/gbp.h"
#include "Icons/tether.h"
#include "Icons/xrp.h"
#include "Inkplate.h"

// Our networking functions, declared in Network.cpp
#include "Network.h"
// create object with all networking functions
Network network;

Inkplate display(INKPLATE_1BIT);

//---------- CHANGE HERE  -------------:

#define COIN_ICON_X 320
#define COIN_ICON_Y 50

// Adjust your time zone, 2 means UTC+2
int timeZone = 2;

// Put in your ssid and password
char ssid[] = "";
char pass[] = "";
#define DELAY_WIFI_RETRY_SECONDS 5
// OPTIONAL:
// change to a different currency
char currency[32] = "bitcoin";
char currencyAbbr[32] = "BTC";

// Used for storing raw price values
double data[3];

// Variables for storing all displayed data as char arrays
char date[64];

void setTime();

void setup()
{
    Serial.begin(115200);

    display.begin(); // Initialize Inkplate object
    display.clearDisplay();
    mainDraw();
    drawCurrencySigns();
    drawCoinIcons();
    display.display();

    // Initialize touchscreen
    if (!display.tsInit(true))
    {
        Serial.println("Touchscreen init failed!");
    }

    // Try connecting to a WiFi network.
    // Parameters are network SSID, password, timeout in seconds and whether to print to serial.
    // If the Inkplate isn't able to connect to a network stop further code execution and print an error message.
    if (!display.connectWiFi(ssid, pass, WIFI_TIMEOUT, true))
    {
        // Can't connect to netowrk
        // Clear display for the error message
        display.clearDisplay();
        // Set the font size;
        display.setTextSize(3);
        // Set the cursor positions and print the text.
        display.setCursor((display.width() / 2) - 200, display.height() / 2);
        display.print(F("Unable to connect to "));
        display.println(F(ssid));
        display.setCursor((display.width() / 2) - 200, (display.height() / 2) + 30);
        display.println(F("Please check ssid and pass!"));
        // Display the error message on the Inkplate and go to deep sleep
        display.display();
        esp_sleep_enable_timer_wakeup(1000L * DELAY_WIFI_RETRY_SECONDS);
        (void)esp_deep_sleep_start();
    }

    // After connecting to WiFi we need to get internet time from NTP server
    setTime();

    getCoinPrices();

    drawAll();
}

void loop()
{
    keysEvents();
}

// Function for getting time from NTP server
void setTime()
{
    // Structure used to hold time information
    struct tm timeInfo;
    time_t nowSec;
    // Fetch current time in epoch format and store it
    display.getNTPEpoch(&nowSec);
    // This loop ensures that the NTP time fetched is valid and beyond a certain threshold
    while (nowSec < 8 * 3600 * 2)
    {
        delay(500);
        yield();
        display.getNTPEpoch(&nowSec);
    }
    gmtime_r(&nowSec, &timeInfo);
    Serial.print(F("Current time: "));
    Serial.print(asctime(&timeInfo));
}

void keysEvents()
{
    if (display.touchInArea(30, 610, 400, 80)) // Refresh Screen
    {
        display.clearDisplay();
        setBlackButton();
        mainDraw();
        drawCurrencySigns();
        drawCoinIcons();
        display.display();
    }

    if (display.touchInArea(600, 10, 400, 80)) // Bitcoin
    {
        strcpy(currency, "bitcoin");
        strcpy(currencyAbbr, "BTC");

        drawAll();
        getCoinPrices();
        drawAll();
    }

    if (display.touchInArea(600, 110, 400, 80)) // Ethereum
    {
        strcpy(currency, "ethereum");
        strcpy(currencyAbbr, "ETH");

        drawAll();
        getCoinPrices();
        drawAll();
    }

    if (display.touchInArea(600, 210, 400, 80)) // Binance
    {
        strcpy(currency, "binance");
        strcpy(currencyAbbr, "BNB");

        drawAll();
        getCoinPrices();
        drawAll();
    }

    if (display.touchInArea(600, 310, 400, 80)) // XRP
    {
        strcpy(currency, "ripple");
        strcpy(currencyAbbr, "XRP");

        drawAll();
        getCoinPrices();
        drawAll();
    }

    if (display.touchInArea(600, 410, 400, 80)) // Dogecoin
    {
        strcpy(currency, "dogecoin");
        strcpy(currencyAbbr, "DOGE");

        drawAll();
        getCoinPrices();
        drawAll();
    }

    if (display.touchInArea(600, 510, 400, 80)) // Tether
    {
        strcpy(currency, "tether");
        strcpy(currencyAbbr, "USDT");

        drawAll();
        getCoinPrices();
        drawAll();
    }

    if (display.touchInArea(600, 650, 400, 80)) // Refresh price for currently selected coin
    {
        getCoinPrices();
        drawAll();
    }
}

void getCoinPrices()
{
    text17_content = "Updating..";
    drawAll();
    while (!network.getData(data))
    {
        Serial.println("Retrying retriving data!");
        delay(1000);
    }
    network.getTime(date);

    text18_content = date;
    text17_content = "";
    text7_content = currencyAbbr;
    text9_content = data[0];
    text11_content = data[1];
    text13_content = data[2];

    drawCurrencySigns();
}

void drawCurrencySigns()
{
    display.drawImage(dollar, 280, 235, 48, 48);
    display.drawImage(euro, 280, 335, 48, 48);
    display.drawImage(gbp, 280, 435, 48, 48);
}

void drawAll()
{
    display.clearDisplay();
    setBlackButton();
    mainDraw();
    drawCurrencySigns();
    drawCoinIcons();
    display.partialUpdate();
}

void setBlackButton()
{
    rect0_fill = -1;
    rect1_fill = -1;
    rect2_fill = -1;
    rect3_fill = -1;
    rect4_fill = -1;
    rect5_fill = -1;

    text1_color_text = BLACK;
    text1_color_bg = WHITE;
    text2_color_text = BLACK;
    text2_color_bg = WHITE;
    text3_color_text = BLACK;
    text3_color_bg = WHITE;
    text4_color_text = BLACK;
    text4_color_bg = WHITE;
    text5_color_text = BLACK;
    text5_color_bg = WHITE;
    text6_color_text = BLACK;
    text6_color_bg = WHITE;

    if (strcmp(currency, "bitcoin") == 0)
    {
        rect0_fill = 1;
        text1_color_text = WHITE;
        text1_color_bg = BLACK;
    }
    else if (strcmp(currency, "ethereum") == 0)
    {
        rect1_fill = 1;
        text2_color_text = WHITE;
        text2_color_bg = BLACK;
    }
    else if (strcmp(currency, "binance") == 0)
    {
        rect2_fill = 1;
        text3_color_text = WHITE;
        text3_color_bg = BLACK;
    }
    else if (strcmp(currency, "ripple") == 0)
    {
        rect3_fill = 1;
        text4_color_text = WHITE;
        text4_color_bg = BLACK;
    }
    else if (strcmp(currency, "dogecoin") == 0)
    {
        rect4_fill = 1;
        text5_color_text = WHITE;
        text5_color_bg = BLACK;
    }
    else if (strcmp(currency, "tether") == 0)
    {
        rect5_fill = 1;
        text6_color_text = WHITE;
        text6_color_bg = BLACK;
    }
}

void drawCoinIcons()
{
    if (strcmp(currency, "bitcoin") == 0)
    {
        display.drawImage(btc, COIN_ICON_X, COIN_ICON_Y, btc_w, btc_h);
    }
    else if (strcmp(currency, "ethereum") == 0)
    {
        display.drawImage(eth, COIN_ICON_X, COIN_ICON_Y, eth_w, eth_h);
    }
    else if (strcmp(currency, "binance") == 0)
    {
        display.drawImage(bnc, COIN_ICON_X, COIN_ICON_Y, bnc_w, bnc_h);
    }
    else if (strcmp(currency, "ripple") == 0)
    {
        display.drawImage(xrp, COIN_ICON_X, COIN_ICON_Y, xrp_w, xrp_h);
    }
    else if (strcmp(currency, "dogecoin") == 0)
    {
        display.drawImage(doge, COIN_ICON_X, COIN_ICON_Y, doge_w, doge_h);
    }
    else if (strcmp(currency, "tether") == 0)
    {
        display.drawImage(tether, COIN_ICON_X, COIN_ICON_Y, tether_w, tether_h);
    }
}
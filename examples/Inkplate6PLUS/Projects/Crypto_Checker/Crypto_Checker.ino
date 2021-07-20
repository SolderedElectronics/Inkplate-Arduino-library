// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE6PLUS
#error "Wrong board selection for this example, please select Inkplate 6 plus in the boards menu."
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
char ssid[] = "e-radionica.com";
char pass[] = "croduino";

// OPTIONAL:
// change to a different currency
char currency[] = "bitcoin";
char currencyAbbr[] = "BTC";


// Used for storing raw price values
double data[3];

// Variables for storing all displayed data as char arrays
char date[64];

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

    network.begin();

    getCoinPrices();

    drawAll();
}

void loop()
{
    keysEvents();
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

        currency = "bitcoin";
        currencyAbbr = "BTC";

        drawAll();
        getCoinPrices();
        drawAll();
    }

    if (display.touchInArea(600, 110, 400, 80)) // Ethereum
    {
        currency = "ethereum";
        currencyAbbr = "ETH";

        drawAll();
        getCoinPrices();
        drawAll();
    }

    if (display.touchInArea(600, 210, 400, 80)) // Binance
    {
        currency = "binancecoin";
        currencyAbbr = "BNB";

        drawAll();
        getCoinPrices();
        drawAll();
    }

    if (display.touchInArea(600, 310, 400, 80)) // XRP
    {
        currency = "ripple";
        currencyAbbr = "XRP";

        drawAll();
        getCoinPrices();
        drawAll();
    }

    if (display.touchInArea(600, 410, 400, 80)) // Dogecoin
    {
        currency = "dogecoin";
        currencyAbbr = "DOGE";

        drawAll();
        getCoinPrices();
        drawAll();
    }

    if (display.touchInArea(600, 510, 400, 80)) // Tether
    {
        currency = "tether";
        currencyAbbr = "USDT";

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

    if (currency == "bitcoin")
    {
        rect0_fill = 1;
        text1_color_text = WHITE;
        text1_color_bg = BLACK;
    }
    else if (currency == "ethereum")
    {
        rect1_fill = 1;
        text2_color_text = WHITE;
        text2_color_bg = BLACK;
    }
    else if (currency == "binancecoin")
    {
        rect2_fill = 1;
        text3_color_text = WHITE;
        text3_color_bg = BLACK;
    }
    else if (currency == "ripple")
    {
        rect3_fill = 1;
        text4_color_text = WHITE;
        text4_color_bg = BLACK;
    }
    else if (currency == "dogecoin")
    {
        rect4_fill = 1;
        text5_color_text = WHITE;
        text5_color_bg = BLACK;
    }
    else if (currency == "tether")
    {
        rect5_fill = 1;
        text6_color_text = WHITE;
        text6_color_bg = BLACK;
    }
}

void drawCoinIcons()
{
    if (currency == "bitcoin")
    {
        display.drawImage(btc, COIN_ICON_X, COIN_ICON_Y, btc_w, btc_h);
    }
    else if (currency == "ethereum")
    {
        display.drawImage(eth, COIN_ICON_X, COIN_ICON_Y, eth_w, eth_h);
    }
    else if (currency == "binancecoin")
    {
        display.drawImage(bnc, COIN_ICON_X, COIN_ICON_Y, bnc_w, bnc_h);
    }
    else if (currency == "ripple")
    {
        display.drawImage(xrp, COIN_ICON_X, COIN_ICON_Y, xrp_w, xrp_h);
    }
    else if (currency == "dogecoin")
    {
        display.drawImage(doge, COIN_ICON_X, COIN_ICON_Y, doge_w, doge_h);
    }
    else if (currency == "tether")
    {
        display.drawImage(tether, COIN_ICON_X, COIN_ICON_Y, tether_w, tether_h);
    }
}
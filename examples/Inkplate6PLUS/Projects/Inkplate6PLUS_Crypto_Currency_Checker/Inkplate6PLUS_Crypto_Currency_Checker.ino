// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_INKPLATE6PLUS) && !defined(ARDUINO_INKPLATE6PLUSV2)
#error "Wrong board selection for this example, please select e-radionica Inkplate 6PLUS or Soldered Inkplate 6PLUS in the boards menu."
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

// OPTIONAL:
// change to a different currency
char currency[32] = "bitcoin";
char currencyAbbr[32] = "BTC";

// Used for storing raw price values
double data[3];

// Variables for storing all displayed data as char arrays
char date[64];

void setup()
{
    Serial.begin(115200);
    // Uncomment this line if you have a USB Power Only Inkplate6PLUS
    //display.setInkplatePowerMode(INKPLATE_USB_PWR_ONLY);
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
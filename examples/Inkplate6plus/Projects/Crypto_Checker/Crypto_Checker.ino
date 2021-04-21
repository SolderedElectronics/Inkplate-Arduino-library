// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE6PLUS
    #error "Wrong board selection for this example, please select Inkplate 6 plus in the boards menu."
#endif

#include "Inkplate.h"
#include "Crypto_UI.h"

// Our networking functions, declared in Network.cpp
#include "Network.h"
// create object with all networking functions
Network network;

Inkplate display(INKPLATE_1BIT);

//---------- CHANGE HERE  -------------:

// Adjust your time zone, 2 means UTC+2
int timeZone = 2;

// Put in your ssid and password
char *ssid = "e-radionica.com";
char *pass = "croduino";

// OPTIONAL:
// change to a different currency
char *currency = "bitcoin";
char *currencyAbbr = "BTC";

// You can find your currency id here:
// https://api.coingecko.com/api/v3/coins

// If it loads weirdly you can search the JSON using ctrl/command+f for
// your crypto by name and then find it's id next to it's name and copy those above

//----------------------------------

// Used for storing raw price values
double data[3];

// Variables for storing all displayed data as char arrays
char date[64];
char fromToDate[64];

char dates[8 * 8];
char prices[16 * 16];

char current[16];
char minimum[16];
char maximum[16];


// Our functions declared below setup and loop
void drawGraph();
void drawAll();


void setup()
{
    Serial.begin(115200);
    display.begin(); // Initialize Inkplate object
    display.clearDisplay();
    mainDraw();
    display.display();

    // Initialize touchscreen
    if (!display.tsInit(true))
    {
        Serial.println("Touchscreen init failed!");
    }

    network.begin();

    getCoinPrices();

    display.clearDisplay();
    mainDraw();
    display.partialUpdate();
}

void loop()
{
    keysEvents();
}

void keysEvents()
{
    if (display.inRect(30, 610, 400, 80))//Refresh Screen
    {
        display.clearDisplay();
        mainDraw();
        display.display();
    }

    if (display.inRect(600, 10, 400, 80))//Bitcoin
    {
        
        currency = "bitcoin";
        currencyAbbr = "BTC";
        getCoinPrices();

        display.clearDisplay();
        mainDraw();
        display.partialUpdate();
    }

    if (display.inRect(600, 110, 400, 80))//Ethereum
    {
        currency = "ethereum";
        currencyAbbr = "ETH";
        getCoinPrices();
        
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();
    }

    if (display.inRect(600, 210, 400, 80))//Binance
    {
        currency = "binancecoin";
        currencyAbbr = "BNB";
        getCoinPrices();
        
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();
    }

    if (display.inRect(600, 310, 400, 80))//XRP
    {
        currency = "ripple";
        currencyAbbr = "XRP";
        getCoinPrices();
        
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();
    }

    if (display.inRect(600, 410, 400, 80))//Dogecoin
    {
        currency = "dogecoin";
        currencyAbbr = "DOGE";
        getCoinPrices();
        
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();
    }

    if (display.inRect(600, 510, 400, 80))//Tether
    {
        currency = "tether";
        currencyAbbr = "USDT";
        getCoinPrices();
        
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();
    }

    if (display.inRect(600, 650, 400, 80))//Refresh price for currently selected coin
    {
        getCoinPrices();
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();
    }
}

void getCoinPrices()
{
    text17_content = "Updating..";
    display.clearDisplay();
    mainDraw();
    display.partialUpdate();
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
}
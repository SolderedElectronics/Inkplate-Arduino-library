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
double data[64];

// Used to simplify UI design
struct textElement
{
    int x;
    int y;
    const GFXfont *font;
    char *text;
    char align;
};

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
    // Initialize touchscreen
    if (!display.tsInit(true))
    {
        Serial.println("Touchscreen init failed!");
    }
    // Call main draw function defined below
    mainDraw();
    display.display();
}

void loop()
{
    keysEvents();
}

void keysEvents()
{
    if (display.inRect(600, 610, 400, 80))//Refresh
    {
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();
    }

    if (display.inRect(600, 10, 400, 80))//Bitcoin
    {
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();
    }

    if (display.inRect(600, 110, 400, 80))//Ethereum
    {
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();
    }

    if (display.inRect(600, 210, 400, 80))//Binance
    {
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();
    }

    if (display.inRect(600, 310, 400, 80))//XRP
    {
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();
    }

    if (display.inRect(600, 410, 400, 80))//Dogecoin
    {
        display.clearDisplay();
        mainDraw();
        display.partialUpdate();
    }

    if (display.inRect(600, 510, 400, 80))//Tether
    {
        display.clearDisplay();
        mainDraw();
        display.display();
    }
}
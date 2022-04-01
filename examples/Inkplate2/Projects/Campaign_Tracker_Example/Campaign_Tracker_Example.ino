/*
   Campaing tracker example for e-radionica.com Inkplate 2
   For this example you will need only USB cable and Inkplate 2.
   Select "Inkplate 2(ESP32)" from Tools -> Board menu.
   Don't have "Inkplate 2 (ESP32)" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   This example will show you how you can use Inkplate 6 to display html data.
   This example gets html data from crowdsource campaing and displays them on Inkplate screen.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   30 March 2022 by Soldered
*/

#include "Inkplate.h"
#include "generatedUI.h"

#define DELAY_MS 60000 * 60
//#define URL      "https://www.crowdsupply.com/byte-mix-labs/microbyte"
#define URL "https://www.crowdsupply.com/soldered/inkplate-6color"

Inkplate display;

RTC_DATA_ATTR unsigned refreshes = 0;

uint32_t n;
char *buf;

String textInTag(const char *tag, const char *tagEnd, int dt = 1);

void setup()
{
    Serial.begin(115200);
    display.begin();

    if (refreshes == 0)
    {
        // Welcome screen
        display.setCursor(20, 50);
        display.setTextSize(2);
        display.drawTextWithShadow(0, 20,"Welcome to Inkpl-", RED, BLACK);
        display.drawTextWithShadow(0, 40,"ate Crowdsupply ", RED, BLACK);
        display.drawTextWithShadow(0, 60,"tracker example!", RED, BLACK);
        //display.display();

        display.clearDisplay();
        //delay(5000);
    }

    while (!display.joinAP("e-radionica.com", "croduino"))
    { 
        Serial.println("Connecting to wifi");
    }

    buf = (char *)ps_malloc(100000);

    HTTPClient http;
    if (http.begin(URL) && http.GET() > 0)
    {
        while (http.getStreamPtr()->available())
        {
            char c = http.getStreamPtr()->read();
            buf[n++] = c;
            delayMicroseconds(5);
        }
        buf[n] = 0;
    }
    Serial.println("Buffer load complete!");

    text1_content = textInTag("<h1>", "</h1>");
    text2_content = textInTag("<h4>", "</h4>");
    text3_content = textInTag("<h3>", "</h3>");
    text4_content = textInTag("<span><sup>$</sup>", "</span>");
    text7_content = textInTag("of&nbsp;<span><sup>$</sup>", "</span>");

    int j = 0;
    String s = textInTag("<div class=\"factoids\">", "</div>", 3);
    Serial.println(s);
    String dummy;
    String *arr[] = {&dummy, &dummy, &dummy 
    ,&text17_content, &text17_content, &text17_content};
    for (int i = 0; i < 6; ++i)
    {
        while (isspace(s[j++]))
            ;
        --j;
        while (!isspace(s[j]) && j < s.length())
            *(arr[i]) += s[j++];
    }
    
    mainDraw();
    display.display();

    ++refreshes;

    free(buf);
    // Go to sleep
    esp_sleep_enable_timer_wakeup(1000LL * DELAY_MS);
    (void)esp_deep_sleep_start();
}

void loop()
{
    // Never here
}

String textInTag(const char *tag, const char *tagEnd, int dt)
{
    String r;
    char *start = strstr(buf, tag) + strlen(tag);
    char *end = start - 1;
    while (dt--)
        end = strstr(end + 1, tagEnd);

    int d = 0;
    for (char *t = start; t < end; ++t)
    {
        if (*t == '<')
            ++d;
        if (d == 0 && *t != '\n')
        {
            r += *t;
        }
        if (*t == '>')
            --d;
    }

    // Hacky solution:

    r.replace("&#34;", "\"");
    r.replace("&nbsp;", " ");

    r.replace("raised", "");
    r.replace("goal", "");
    r.replace("Funded!", "");
    r.replace("funded", "");
    r.replace(" on", "");

    r.replace("updates", "");

    if (r.indexOf("hours left") != -1)
    {
        r.replace("hours left", "");
        text17_content = "hours left";
    }
    if (r.indexOf("days left") != -1)
    {
        r.replace("days left", "");
        text17_content = "days left";
    }

    r.replace("backers", "");

    r.replace("Subscribe to Updates", "");

    r.trim();

    if (r.indexOf("$") != -1)
        r = r.substring(r.indexOf("$") + 1);

    return r;
}

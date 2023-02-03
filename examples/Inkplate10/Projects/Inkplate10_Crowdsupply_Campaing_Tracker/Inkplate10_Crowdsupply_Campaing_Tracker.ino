/*
   Inkplate10_Crowdsupply_Campaing_Tracker example for Soldered Inkplate 10
   For this example you will need only USB cable and Inkplate 10.
   Select "e-radionica Inkplate10" or "Soldered Inkplate10" from Tools -> Board menu.
   Don't have "e-radionica Inkplate10" or "Soldered Inkplate10" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example will show you how you can use Inkplate 10 to display html data.
   This example gets html data from crowdsource campaing and displays them on Inkplate screen.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   8 December 2022 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_INKPLATE10) && !defined(ARDUINO_INKPLATE10V2)
#error "Wrong board selection for this example, please select e-radionica Inkplate10 or Soldered Inkplate10 in the boards menu."
#endif

#include "Inkplate.h"
#include "generatedUI.h"

#define DELAY_MS 60000 * 60
#define URL "https://www.crowdsupply.com/soldered/inkplate-10"

// Enter your Wi-Fi credentials
#define ssid ""
#define password ""

Inkplate display(INKPLATE_1BIT);

RTC_DATA_ATTR unsigned refreshes = 0;

uint32_t n;
char *buf;

String textInTag(const char *tag, const char *tagEnd, int dt = 1);

void setup()
{
    Serial.begin(115200);
    display.begin();

    Serial.print("Connecting to wifi");
    while (!display.joinAP(ssid, password))
    {
        Serial.print('.');
        delay(1000);
    }

    buf = (char *)ps_malloc(100000);

    HTTPClient http;
    if (http.begin(URL) && http.GET() > 0)
    {
        while (http.getStreamPtr()->available())
        {
            char c = http.getStreamPtr()->read();
            buf[n++] = c;
            delayMicroseconds(100);
            
        }
        buf[n] = 0;
    }
    Serial.println("Buffer load complete!");

    text1_content = textInTag("<h1>", "</h1>");
    text3_content = textInTag("<h3 class=\"project-teaser\">", "</h3>");
    text4_content = textInTag("<p class=\"project-pledged\">", "</p>");
    text7_content = textInTag("<p class=\"project-goal\">", "</p>");
    text11_content = textInTag("<span class=\"status-bar-left\">", "</span>");
    
    int percent;
    text11_content.replace(",", "");
    sscanf(text11_content.c_str(), "%d%%", &percent);

    if (percent < 100 && percent > 0)
    {
        float per = (float)(percent / 100.00);
        int diff = line0_end_x - line0_start_x;
        Serial.println(per);
        Serial.println(diff);
        line0_end_x = line0_start_x + (diff * per);
    }
    else if (percent >= 100)
    {
    }
    else
    {
        line0_end_x = line0_start_x;
    }

    text13_content = textInTag("<a href=\"/soldered/inkplate-10/updates\">", "</a>");
    text14_content = textInTag("<a href=\"/soldered/inkplate-10/crowdfunding\">", "</a>");
    text17_content = textInTag("<a href=\"/soldered/inkplate-10/backers\">", "</a>");

    text19_content += textInTag("<span class=\"badge badge-calendar bg-secondary float-end\">", "</span>");

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

    int d = 0, rc = 0;
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

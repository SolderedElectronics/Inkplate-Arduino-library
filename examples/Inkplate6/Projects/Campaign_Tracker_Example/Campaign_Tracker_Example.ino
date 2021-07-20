/*
   Campaing tracker example for e-radionica.com Inkplate 6
   For this example you will need only USB cable and Inkplate 6.
   Select "Inkplate 6(ESP32)" from Tools -> Board menu.
   Don't have "Inkplate 6(ESP32)" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   This example will show you how you can use Inkplate 6 to display html data.
   This example gets html data from crowdsource campaing and displays them on Inkplate screen.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   28 July 2020 by e-radionica.com
*/

#include "Inkplate.h"
#include "generatedUI.h"

#define DELAY_MS 60000 * 60
//#define URL      "https://www.crowdsupply.com/byte-mix-labs/microbyte"
#define URL "https://www.crowdsupply.com/e-radionica/inkplate-10"

Inkplate display(INKPLATE_1BIT);

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
        display.setCursor(70, 270);
        display.setTextSize(2);
        display.print(F("Welcome to Inkplate Crowdsupply tracker example!"));
        display.display();

        display.clearDisplay();
        delay(5000);
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
        }
        buf[n] = 0;
    }
    Serial.println("Buffer load complete!");

    text1_content = textInTag("<h1 class=\"mobile-break project-title\">", "</h1>");
    text2_content = textInTag("<h4 class=\"mobile-break tiny-text project-organization-name\">", "</h4>");
    text3_content = textInTag("<h3 class=\"project-teaser\">", "</h3>");
    text4_content = textInTag("<p class=\"project-pledged\">", "</p>");
    text7_content = textInTag("<p class=\"project-goal\">", "</p>");
    text11_content = textInTag("<div class=\"status-bar status-bar-primary\">", "</span>");
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

    int j = 0;
    String s = textInTag("<div class=\"factoids\">", "</div>", 3);
    Serial.println(s);
    String *arr[] = {&text13_content, &text14_content, &text15_content,
                     &text17_content, &text17_content, &text17_content};
    for (int i = 0; i < 6; ++i)
    {
        while (isspace(s[j++]))
            ;
        --j;
        while (!isspace(s[j]) && j < s.length())
            *(arr[i]) += s[j++];
    }

    text19_content = textInTag("<div class=\"subscribe-cta\">", "</div>");

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
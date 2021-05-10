#include "Inkplate.h"
#include "generatedUI.h"

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE6PLUS
    #error "Wrong board selection for this example, please select Inkplate 6PLUS in the boards menu."
#endif

#define DELAY_MS 60000 * 60
#define URL      "https://www.crowdsupply.com/byte-mix-labs/microbyte"

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
        display.setCursor(70, 370);
        display.setTextSize(3);
        display.print(F("Welcome to Inkplate Crowdsupply tracker example!"));
        display.display();

        display.clearDisplay();
        delay(5000);
    }

    if (!display.joinAP("e-radionica.com", "croduino"))
        ESP.restart();

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

    text1_content = textInTag("<h1 class=\"mobile-break project-title\">", "</h1>");
    text2_content = textInTag("<h4 class=\"mobile-break tiny-text project-organization-name\">", "</h4>");
    text3_content = textInTag("<h3 class=\"project-teaser\">", "</h3>");
    text4_content = textInTag("<p class=\"project-pledged\">", "</p>");
    text7_content = textInTag("<p class=\"project-goal\">", "</p>");
    text11_content = textInTag("<div class=\"status-bar progress-bar-nontext-past-hump\">", "</div>");

    int percent;
    sscanf(text11_content.c_str(), "%d%", &percent);
    line0_end_x = map(min(percent, 100), 0, 100, 75, 435);

    int j = 0;
    String s = textInTag("<div class=\"factoids\">", "</div>", 3);
    String *arr[] = {&text13_content, &text14_content, &text15_content};
    for (int i = 0; i < 3; ++i)
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

    int d = 0, i = 0, rc = 0;
    for (char *t = start; t < end; ++t)
    {
        if (*t == '<')
            ++d;
        if (d == 0 && *t != '\n')
            r += *t;
        if (*t == '>')
            --d;
    }

    // Hacky solution:

    r.replace("&#34;", "\"");
    r.replace("&nbsp;", " ");

    r.replace("raised", "");
    r.replace("goal", "");
    r.replace("Funded!", "");

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
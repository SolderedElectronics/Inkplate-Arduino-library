/*
   Inkplate6COLOR_Crowdsupply_Campaing_Tracker example for Soldered Inkplate 6COLOR
   For this example you will need only USB cable and Inkplate 6COLOR.
   Select "Soldered Inkplate 6COLOR" from Tools -> Board menu.
   Don't have "Soldered Inkplate 6COLOR" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example will show you how you can use Inkplate 6COLOR to display html data.
   This example gets html data from crowdsource campaing and displays them on Inkplate screen.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   8 December 2022 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATECOLOR
#error "Wrong board selection for this example, please select Soldered Inkplate 6COLOR in the boards menu."
#endif

#include "Inkplate.h"    // Include Inkplate library to the sketch
#include "generatedUI.h" // include generated UI

// Change here to your wifi ssid and pass and the url to display info for
#define ssid "Soldered" // Name of the WiFi network (SSID) that you want to connect Inkplate to
#define pass "dasduino" // Password of that WiFi network

#define DELAY_MS 60000 * 60                                             // Delay between fetching data
#define URL      "https://www.crowdsupply.com/soldered/inkplate-6color" // Link to the Inkplate 6COLOR crowdsupply campaign

// Create an object on Inkplate library
Inkplate display;

uint32_t n; // Variable for counting position in the buffer
char *buf;  // Pointer to the buffer

// Declaration of the function used later
String textInTag(const char *tag, const char *tagEnd, int dt = 1);

void setup()
{
    Serial.begin(115200); // Init serial communication
    display.begin();      // Init Inkplate library (you should call this function ONLY ONCE)

    // Connect to WiFi
    while (!display.joinAP(ssid, pass))
    {
        Serial.print('.');
        delay(1000);
    }

    // Allocate buffer to download Crowdsupply webpage
    buf = (char *)ps_malloc(100000);

    // Create an HTTPclient object and do the GET request
    HTTPClient http;
    if (http.begin(URL) && http.GET() > 0)
    {
        // If we have available data, read it and save it to the buffer
        while (http.getStreamPtr()->available())
        {
            char c = http.getStreamPtr()->read();
            buf[n++] = c;
            delayMicroseconds(100);
        }

        // Add the end of the string
        buf[n] = 0;
    }
    Serial.println("Buffer load complete!");

    // Get the values for each field.
    // When we did a get request, we got HTML as a response because there is no API on this web page.
    // If you open the source of the page, you will see the HTML code. Here in the function we pass tags we are
    // interested in and get the value between the tags as a string.
    text1_content = textInTag("<h1>", "</h1>");                          // Separate the product name
    text3_content = textInTag("<h3 class=\"project-teaser\">", "</h3>"); // Separate the project teaser
    text4_content = textInTag("<p class=\"project-pledged\">", "</p>");  // Separate the amount of pledged
    text7_content = textInTag("<p class=\"project-goal\">", "</p>");     // Separate the goal
    text11_content = textInTag("<div class=\"status-bar status-bar-primary\">", "</span>"); // Separate the percentage

    int percent;
    text11_content.replace(",", "");
    sscanf(text11_content.c_str(), "%d%%", &percent);

    // Draw parcent slider
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

    text13_content =
        textInTag("<a href=\"/soldered/inkplate-6color/updates\">", "</a>"); // Separate the number of updates
    text14_content =
        textInTag("<a href=\"/soldered/inkplate-6color/crowdfunding\">", "</a>"); // Separate the date when founded
    text17_content =
        textInTag("<a href=\"/soldered/inkplate-6color/backers\">", "</a>"); // Separate the number of backers

    text19_content += textInTag("<span class=\"badge badge-calendar bg-secondary float-end\">",
                                "</span>"); // Separate the last update date

    // Display the content on the screen
    mainDraw();
    display.display();

    // Free the buffer
    free(buf);

    // Go to sleep
    esp_sleep_enable_timer_wakeup(1000LL * DELAY_MS);
    
    // Put the panel in the deep sleep
    display.setPanelDeepSleep(0);

    // Start deep sleep (this function does not return). Program stops here.
    esp_deep_sleep_start();
}

void loop()
{
    // Never here! If you are using deep sleep, the whole program should be in setup() because the board restarts each
    // time. loop() must be empty!
}

/**
 * @brief       The function that returns content between HTML tags as a string.
 *
 * @param const char *tag
 *        The starting tag from which we want to get the value
 *
 * @param const char *tagEnd
 *        The ending tag from which we want to get the value
 *
 * @param int dt
 *        How many end tags (*tagEnd) want to skip. Default is 1
 *
 * @return      String value between passed tags
 */
String textInTag(const char *tag, const char *tagEnd, int dt)
{
    String r;                                     // String for result
    char *start = strstr(buf, tag) + strlen(tag); // Pointer to the beginning of the text after the tag
    char *end = start - 1;                        // Set tag end one address before start pointer
    while (dt--)
    {
        // Search where is the "tagEnd" starting from the last "end" which is the beginning for the first time
        // (*end =  start - 1) and repeat it "dt" times
        end = strstr(end + 1, tagEnd);
    }

    // Remove "<" and ">" from the result string
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

    // Remove certain words to get only the data we want
    // Hacky solution:
    r.replace("&#34;", "\"");
    r.replace("&nbsp;", " ");

    r.replace("raised", "");
    r.replace("goal", "");
    r.replace("Funded!", "");
    r.replace("funded", "");
    r.replace(" on", "");

    r.replace("updates", "");

    // Remove the following from the result string if it exists and put it in the separate string that displays on
    // the screen
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

    // Remove extraneous whitespace characters at the beginning or the end of the string
    r.trim();

    // If there is a dollar sign, the result is the string after that
    if (r.indexOf("$") != -1)
        r = r.substring(r.indexOf("$") + 1);

    return r;
}

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

uint32_t n;
char *buf;

String textInTag(const char *tag, const char *tagEnd, int dt = 1);

void setup()
{
  Serial.begin(115200); //Initialize UART communication with PC
  display.begin();  //Initialize e-paper

  // Welcome screen
  display.setCursor(20, 50);
  display.setTextSize(2);
  display.drawTextWithShadow(0, 20, "Welcome to Inkpl-", RED, BLACK); //Print text with shadow
  display.drawTextWithShadow(0, 40, "ate Crowdsupply ", RED, BLACK);
  display.drawTextWithShadow(0, 60, "tracker example!", RED, BLACK);
  display.display();

  display.clearDisplay(); //Clear display buffer
  delay(5000);

  while (!display.joinAP("", "")) //Connect to WiFi, first argument is SSID, second is password
  {
    Serial.println("Connecting to wifi");
  }

  buf = (char *)ps_malloc(100000);

  HTTPClient http;
  if (http.begin(URL) && http.GET() > 0) //Check if ESP got answer from server and if data is available
  {
    while (http.getStreamPtr()->available()) //Check if there is data incoming
    {
      char c = http.getStreamPtr()->read(); //Read data and save it in buffer
      buf[n++] = c;
      delayMicroseconds(5); //Make small delay before getting next data
    }
    buf[n] = 0;
  }
  Serial.println("Buffer load complete!");

  text1_content = textInTag("<h1>", "</h1>"); //Find text beetwen tags
  text2_content = textInTag("<h4>", "</h4>"); //Find text beetwen tags
  text3_content = textInTag("<h3>", "</h3>"); //Find text beetwen tags
  text4_content = textInTag("<span><sup>$</sup>", "</span>"); //Find text beetwen tags
  text7_content = textInTag("of&nbsp;<span><sup>$</sup>", "</span>"); //Find text beetwen tags

  int j = 0;
  String s = textInTag("<div class=\"factoids\">", "</div>", 3); //Find text beetwen tags
  Serial.println(s); //Print on serial monitor string
  String dummy;
  String *arr[] = {&dummy, &dummy, &dummy
                   , &text17_content, &text17_content, &text17_content
                  };
  for (int i = 0; i < 6; ++i)
  {
    while (isspace(s[j++])); //Check if symbol in string is whitespace
    --j;
    while (!isspace(s[j]) && j < s.length()) //String s contains 6 substrings, this function saves substrings in arr array
      *(arr[i]) += s[j++];
  }

  mainDraw();// Call function to draw UI
  display.display(); //Display content of display buffer on display

  free(buf);  // Free dynamically allocated memory; if memory is not freed, it may overflow (Almost always will, but compiler 
              // will not show error for overflowing memory)
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
  char *start = strstr(buf, tag) + strlen(tag); //Find pointer to opened tag
  char *end = start - 1; // End of opened tag
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

  // Hacky solution to remove excess text

  r.replace("&#34;", "\""); // Find text and replace it with nothing
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

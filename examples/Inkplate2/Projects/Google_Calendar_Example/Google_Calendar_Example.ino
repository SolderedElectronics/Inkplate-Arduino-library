/*
   3-Google_calendar_example for e-radionica.com Inkplate 2
   For this example you will need only USB cable and Inkplate 2.
   Select "Inkplate 2(ESP32)" from Tools -> Board menu.
   Don't have "Inkplate 2(ESP32)" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   This project shows you how Inkplate 2 can be used to display
   events in your Google Calendar using their provided API

   For this to work you need to change your timezone, wifi credentials and your private calendar url
   which you can find following these steps:

    1. Open your google calendar
    2. Click the 3 menu dots of the calendar you want to access at the bottom of left hand side
    3. Click 'Settings and sharing'
    4. Navigate to 'Integrate Calendar'
    5. Take the 'Secret address in iCal format'

   (https://support.google.com/calendar/thread/2408874?hl=en)

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   3 August 2020 by e-radionica.com
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Inkplate 2 in the boards menu."
#endif

// Include Inkplate library to the sketch
#include "Inkplate.h"

// Including fonts
#include "Fonts/Roboto_Regular8.h"

// Includes
#include "Network.h"
#include <algorithm>
#include <ctime>

// CHANGE HERE ---------------

char ssid[] = "";
char pass[] = "";
char calendarURL[] = "";
int timeZone = 2;

//---------------------------

// Delay between API calls
#define DELAY_MS 4 * 60000

// Initiate out Inkplate object
Inkplate display;

// Our networking functions, see Network.cpp for info
Network network;

// Variables for time and raw event info
char date[64];
char *data;

// Struct for storing calender event info
struct entry
{
    char name[128];
    char time[128];
    char location[128];
    int day = -1;
    time_t timeStampEnd;
    time_t timeStampStart;
};

// Here we store calendar entries
int entriesNum = 0;
entry entries[128];

// All our functions declared below setup and loop
void drawInfo();
void drawTime();
void drawGrid();
void getToFrom(char *dst, char *from, char *to, int *day, int *timeStampEnd);
bool drawEvent(entry *event, int day, int beginY, int maxHeigth, int *heigthNeeded);
int cmp(const void *a, const void *b);
void drawData();

void setup()
{
    Serial.begin(115200);

    data = (char *)ps_malloc(2000000LL);

    // Initial display settings
    display.begin();
    
    display.setTextWrap(false);
    display.setTextColor(BLACK, WHITE);

    // Welcome screen
    display.setTextSize(2);
    display.drawTextWithShadow(0, 20,"Welcome to Inkpl-", RED, BLACK);
    display.drawTextWithShadow(0, 40,"ate 2 Google", RED, BLACK);
    display.drawTextWithShadow(0, 60,"Calendar example!", RED, BLACK);
    display.display();

    delay(5000);
    network.begin();

    // Keep trying to get data if it fails the first time
    while (!network.getData(data))
    {
        Serial.println("Failed getting data, retrying");
        delay(1000);
    }

    // Initial screen clearing
    display.clearDisplay();

    getEvents();
    drawEvent();

    display.display();

    // Go to sleep before checking again
    esp_sleep_enable_timer_wakeup(1000L * DELAY_MS);
    (void)esp_deep_sleep_start();
}

void loop()
{
    // Never here
}


void drawEvent()
{
  uint8_t next_event = 0;
  time_t curr_timestamp = network.getEpoch();
  while(entries[next_event].timeStampEnd < curr_timestamp && entriesNum > next_event)
  {
    next_event++;
  }
  if(next_event == entriesNum)
  {
    display.setTextSize(1);
    display.setCursor(5,20);
    display.setFont(&Roboto_Regular8);
    display.print("No upcoming events.");
  }
  else
  {
    display.setTextSize(1);
    display.setCursor(5,8);
    display.setFont(&Roboto_Regular8);
    display.print("Next event starts at:");
    display.setCursor(10,32);
    display.setTextColor(RED,WHITE);
    display.print(asctime(localtime(&(entries[next_event].timeStampStart))));
    display.setCursor(5,52);
    display.setTextColor(BLACK,WHITE);
    display.print("Lasting: ");
    display.setTextColor(RED,WHITE);
    display.print(entries[next_event].time);
    display.setCursor(5,72);
    display.setTextColor(BLACK,WHITE);
    display.print("Event name: ");
    display.setTextColor(RED,WHITE);
    display.print(entries[next_event].name);

    /*display.setCursor(10,50);
    display.print("Event name: ");
    display.print(entries[next_event].name);*/
  }
}

// Format event times, example 13:00 to 14:00
void getToFrom(char *dst, char *from, char *to, int *day, time_t *timeStampEnd, time_t *timeStampStart)
{
    // ANSI C time struct
    struct tm ltm = {0}, ltm2 = {0};
    char temp[128], temp2[128];
    strncpy(temp, from, 16);
    temp[16] = 0;

    // https://github.com/esp8266/Arduino/issues/5141, quickfix
    memmove(temp + 5, temp + 4, 16);
    memmove(temp + 8, temp + 7, 16);
    memmove(temp + 14, temp + 13, 16);
    memmove(temp + 16, temp + 15, 16);
    temp[4] = temp[7] = temp[13] = temp[16] = '-';

    // time.h function
    strptime(temp, "%Y-%m-%dT%H-%M-%SZ", &ltm);

    // create start and end event structs
    struct tm event, event2;
    time_t epoch = mktime(&ltm) + (time_t)timeZone * 3600L;
    gmtime_r(&epoch, &event);
    strncpy(dst, asctime(&event) + 11, 5);

    dst[5] = '-';

    strncpy(temp2, to, 16);
    temp2[16] = 0;

    // Same as above

    // https://github.com/esp8266/Arduino/issues/5141, quickfix
    memmove(temp2 + 5, temp2 + 4, 16);
    memmove(temp2 + 8, temp2 + 7, 16);
    memmove(temp2 + 14, temp2 + 13, 16);
    memmove(temp2 + 16, temp2 + 15, 16);
    temp2[4] = temp2[7] = temp2[13] = temp2[16] = '-';

    strptime(temp2, "%Y-%m-%dT%H-%M-%SZ", &ltm2);

    time_t epoch2 = mktime(&ltm2) + (time_t)timeZone * 3600L;
    gmtime_r(&epoch2, &event2);
    strncpy(dst + 6, asctime(&event2) + 11, 5);

    dst[11] = 0;

    char day0[64], day1[64], day2[64];

    // Find UNIX timestamps for next days to see where to put event
    network.getTime(day0, 0);
    network.getTime(day1, 24 * 3600);
    network.getTime(day2, 48 * 3600);

    *timeStampEnd = epoch2;
    *timeStampStart = epoch;

    // Getting the time from our function in Network.cpp
    network.getTime(temp);
    if (strncmp(day0, asctime(&event), 10) == 0)
        *day = 0;
    else if (strncmp(day1, asctime(&event), 10) == 0)
        *day = 1;
    else if (strncmp(day2, asctime(&event), 10) == 0)
        *day = 2;
    else // event not in next 3 days, don't display
        *day = -1;
}

// Struct event comparison function, by timestamp, used for qsort later on
int cmp(const void *a, const void *b)
{
    entry *entryA = (entry *)a;
    entry *entryB = (entry *)b;

    return (entryA->timeStampEnd - entryB->timeStampEnd);
}

// Main data drawing data
void getEvents()
{
    long i = 0;
    long n = strlen(data);

    // reset count
    entriesNum = 0;

    // Search raw data for events
    while (i < n && strstr(data + i, "BEGIN:VEVENT"))
    {
        // Find next event start and end
        i = strstr(data + i, "BEGIN:VEVENT") - data + 12;
        char *end = strstr(data + i, "END:VEVENT");

        if (end == NULL)
            continue;

        // Find all relevant event data
        char *summary = strstr(data + i, "SUMMARY:") + 8;
        char *location = strstr(data + i, "LOCATION:") + 9;
        char *timeStart = strstr(data + i, "DTSTART:") + 8;
        char *timeEnd = strstr(data + i, "DTEND:") + 6;

        if (summary && summary < end)
        {
            strncpy(entries[entriesNum].name, summary, strchr(summary, '\n') - summary);
            entries[entriesNum].name[strchr(summary, '\n') - summary] = 0;
        }
        if (location && location < end)
        {
            strncpy(entries[entriesNum].location, location, strchr(location, '\n') - location);
            entries[entriesNum].location[strchr(location, '\n') - location] = 0;
        }
        if (timeStart && timeStart < end && timeEnd < end)
        {
            getToFrom(entries[entriesNum].time, timeStart, timeEnd, &entries[entriesNum].day,
                      &entries[entriesNum].timeStampEnd, &entries[entriesNum].timeStampStart);
        }
        ++entriesNum;
    }

    // Sort entries by time
    qsort(entries, entriesNum, sizeof(entry), cmp);
}

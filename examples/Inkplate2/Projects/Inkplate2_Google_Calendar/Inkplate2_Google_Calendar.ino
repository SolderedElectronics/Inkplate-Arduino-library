/*
    Inkplate2_Google_Calendar example for Soldered Inkplate 2
    For this example you will need only USB cable and Inkplate 2.
    Select "Soldered Inkplate2" from Tools -> Board menu.
    Don't have "Soldered Inkplate2" option? Follow our tutorial and add it:
    https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

    This project shows you how Soldered Inkplate 2 can be used to display
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
    Looking to get support? Write on our forums: https://forum.soldered.com/
    29 March 2022 by Soldered
*/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Soldered Inkplate2 in the boards menu."
#endif

#include "Inkplate.h" // Include Inkplate library to the sketch

// Including fonts
#include "Fonts/SourceSansPro_Regular8pt7b.h"

// Includes
#include "Network.h"
#include <algorithm>
#include <ctime>

// Delay between API calls
#define DELAY_MS 4 * 60000 // 4 minutes times 60000 miliseconds in minute
#define DELAY_WIFI_RETRY_SECONDS 10

Inkplate display; // Initiate out Inkplate object

Network network; // Our networking functions, see Network.cpp for info

// Enter your WiFi SSID and password
char ssid[] = "";
char pass[] = "";

// Google calendar URL
char calendarURL[] = "";

// Set the timezone (in this case +2 hours)
int timeZone = 2;

// Struct for holding time and date data
struct tm timeinfo;

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

const char months[][4] = {{"JAN"}, {"FEB"}, {"MAR"}, {"APR"}, {"MAY"}, {"JUN"},
                          {"JUL"}, {"AUG"}, {"SEP"}, {"OCT"}, {"NOV"}, {"DEC"}};

// All our functions declared below setup and loop
void drawInfo();
void drawTime();
void drawGrid();
void getToFrom(char *dst, char *from, char *to, int *day, int *timeStampEnd);
bool drawEvent(entry *event, int day, int beginY, int maxHeigth, int *heigthNeeded);
int cmp(const void *a, const void *b);
void drawData();

/**
 * @brief               Setup function, runs only once
 * 
 * @return              None
*/
void setup()
{
    Serial.begin(115200); // Initialize UART communication with PC

    data = (char *)ps_malloc(2000000LL); // Allocate 2 million chars from PSRAM for data buffer

    // Initial display settings
    display.begin();

    display.setTextWrap(false);
    display.setTextColor(INKPLATE2_BLACK, INKPLATE2_WHITE);

    // Connect Inkplate to the WiFi network
    // Try connecting to a WiFi network.
    // Parameters are network SSID, password, timeout in seconds and whether to print to serial.
    // If the Inkplate isn't able to connect to a network stop further code execution and print an error message.
    if (!display.connectWiFi(ssid, pass, WIFI_TIMEOUT, true))
    {
        //Can't connect to netowrk
        // Clear display for the error message
        display.clearDisplay();
        // Set the font size;
        display.setTextSize(1);
        // Set the cursor positions and print the text.
        display.setCursor(0, 0);
        display.print(F("Unable to connect to "));
        display.println(F(ssid));
        display.println(F("Please check SSID and PASS!"));
        // Display the error message on the Inkplate and go to deep sleep
        display.display();
        esp_sleep_enable_timer_wakeup(1000L * DELAY_WIFI_RETRY_SECONDS);
        (void)esp_deep_sleep_start();
    }

    // After connecting to WiFi we need to get internet time from NTP server
    time_t nowSec;
    struct tm timeInfo;
    // Fetch current time in epoch format and store it
    display.getNTPEpoch(&nowSec);
    gmtime_r(&nowSec, &timeInfo);
    Serial.print(F("Current time: "));
    Serial.print(asctime(&timeInfo));

    // Get the data from Google Calendar
    // Repeat attempts until data is fully downloaded
    Serial.println("Getting data... ");
    while (!network.getData(calendarURL, data))
    {
        delay(1000);
    }

    // Initial screen clearing
    display.clearDisplay();

    getEvents(); // Call function
    drawEvent(); // Call function

    display.display(); // Show data on display

    // Go to sleep before checking again
    // rtc_gpio_isolate(GPIO_NUM_12);   // Isolate/disable GPIO12 on ESP32 (only to reduce power consumption in sleep)
    esp_sleep_enable_timer_wakeup(1000L * DELAY_MS); // Enable wake up while sleep is enabled after 4 minutes
                                                     //  1000L * is here because function accepts microseconds
     
    esp_deep_sleep_start();       // Put ESP32 into deep sleep. Program stops here
}

/**
 * @brief               Main loop function, doesn't run 
 * 
 * @return              None
*/
void loop()
{
    // Never here! If you are using deep sleep, the whole program should be in setup() because the board restarts each
    // time. loop() must be empty!
}

/**
 * @brief               Functions for drawing the date and two next events
 * 
 * @return              None
*/
void drawEvent()
{
    uint8_t next_event = 0;
    time_t curr_timestamp = network.getEpoch(); // Get current time in epoch format

    // Find next event and reject all events that already happened
    while (entries[next_event].timeStampEnd < curr_timestamp && entriesNum > next_event)
    {
        next_event++;
    } 

    // Next part of code draws the UI

    // Draw the rectangles for the date and the two events which are displayed
    display.drawRect(4, 40, 48, 55, INKPLATE2_BLACK);
    display.drawRect(72, 4, 133, 46, INKPLATE2_BLACK);
    display.drawRect(72, 55, 133, 46, INKPLATE2_BLACK);

    // Print the time
    display.setFont(&SourceSansPro_Regular8pt7b);
    display.setCursor(10, 22);
    timeinfo.tm_hour < 10 ? display.print("0") : 0;
    display.print(timeinfo.tm_hour);
    display.print(":");
    timeinfo.tm_min < 10 ? display.print("0") : 0;
    display.print(timeinfo.tm_min);

    // Print the date and month
    timeinfo.tm_mday < 10 ? display.setCursor(22, 62) : display.setCursor(18, 62);
    display.print(timeinfo.tm_mday);
    display.setCursor(11, 85);
    display.setTextColor(INKPLATE2_RED, INKPLATE2_WHITE);
    display.print(months[timeinfo.tm_mon]);
    display.setTextColor(INKPLATE2_BLACK, INKPLATE2_WHITE);

    // Print next two events in the calendar

    if (next_event <= entriesNum)
    {
        display.setCursor(78, 18);
        entries[next_event].name[14] = '\0';
        display.print(entries[next_event].name);
        display.setCursor(82, 38);
        display.print(entries[next_event].time);
        next_event++;
    }

    if (next_event <= entriesNum)
    {
        display.setCursor(78, 70);
        entries[next_event].name[14] = '\0';
        display.print(entries[next_event].name);
        display.setCursor(82, 90);
        display.print(entries[next_event].time);
    }
}

/**
 * @brief               Functions for formatting the event times
 * 
 * @param               char * dst
 *                      Destination buffer where to write the result
 * 
 * @param               char * from
 *                      Char array that holds the event beginning time
 * 
 * @param               char * to
 *                      Char array that holds the event end time
 * 
 * @param               int * day
 *                      Pointer to the variable that holds the day of the event relative to the current day
 *                      0 For today, 1 for tomorrow, 2 for the next day, -1 by default
 * 
 * @param               time_t * timeStampEnd
 *                      Pointer to struct time_t where to save the result
 * 
 * @param               time_t * timeStampBegin
 *                      Pointer to struct time_t where to save the result 
 * @return              None
*/
void getToFrom(char *dst, char *from, char *to, int *day, time_t *timeStampEnd, time_t *timeStampStart)
{
    // ANSI C time struct
    struct tm ltm = {0}, ltm2 = {0};
    char temp[128], temp2[128];
    strncpy(temp, from, 16); // Copy string temp into string from
    temp[16] = 0;

    // https://github.com/esp8266/Arduino/issues/5141, quickfix
    memmove(temp + 5, temp + 4, 16); // Format time for strptime function
    memmove(temp + 8, temp + 7, 16);
    memmove(temp + 14, temp + 13, 16);
    memmove(temp + 16, temp + 15, 16);
    temp[4] = temp[7] = temp[13] = temp[16] = '-';

    // time.h function
    strptime(temp, "%Y-%m-%dT%H-%M-%SZ", &ltm); // Convert string containing time into time struct

    // create start and end event structs
    struct tm event, event2;
    time_t epoch = mktime(&ltm) + (time_t)timeZone * 3600L; // Get epoch
    gmtime_r(&epoch, &event);
    strncpy(dst, asctime(&event) + 11, 5);

    dst[5] = '-';

    strncpy(temp2, to, 16);
    temp2[16] = 0;

    // Same as above
    // https://github.com/esp8266/Arduino/issues/5141, quickfix
    memmove(temp2 + 5, temp2 + 4, 16); // Format time for strptime function
    memmove(temp2 + 8, temp2 + 7, 16);
    memmove(temp2 + 14, temp2 + 13, 16);
    memmove(temp2 + 16, temp2 + 15, 16);
    temp2[4] = temp2[7] = temp2[13] = temp2[16] = '-';

    strptime(temp2, "%Y-%m-%dT%H-%M-%SZ", &ltm2); // Convert string containing time into time struct

    time_t epoch2 = mktime(&ltm2) + (time_t)timeZone * 3600L; // Get epoch
    gmtime_r(&epoch2, &event2);
    strncpy(dst + 6, asctime(&event2) + 11, 5);

    dst[11] = 0;

    char day0[64], day1[64], day2[64];

    // Find UNIX timestamps for next days to see where to put event
    network.getTime(day0, 0, &timeinfo, timeZone);
    network.getTime(day1, 24 * 3600, &timeinfo, timeZone);
    network.getTime(day2, 48 * 3600, &timeinfo, timeZone);

    *timeStampEnd = epoch2;
    *timeStampStart = epoch;

    // Getting the time from our function in Network.cpp
    network.getTime(temp, 0, &timeinfo, timeZone);
    if (strncmp(day0, asctime(&event), 10) == 0)
        *day = 0;
    else if (strncmp(day1, asctime(&event), 10) == 0)
        *day = 1;
    else if (strncmp(day2, asctime(&event), 10) == 0)
        *day = 2;
    else // event not in next 3 days, don't display
        *day = -1;
}

/**
 * @brief               Comparison function for events
 * 
 * @param               void * a
 *                      First member of comparison
 * 
 * @param               void * b
 *                      Second member of comparison
 *  
 * @return              Positive integer if A ends after B
*/
int cmp(const void *a, const void *b)
{
    entry *entryA = (entry *)a;
    entry *entryB = (entry *)b;

    return (entryA->timeStampEnd - entryB->timeStampEnd); // Compare timestamps of two objects
}

/**
 * @brief               Search for data in result of Google Calendar API GET data
 * 
 * @return              None
*/
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

        if (summary && summary < end && (summary - data) > 0)
        {
            strncpy(entries[entriesNum].name, summary,
                    strchr(summary, '\n') - summary); // Copy summary to struct variable name
            entries[entriesNum].name[strchr(summary, '\n') - summary] = 0;
        }
        if (location && location < end && (location - data) > 0)
        {
            strncpy(entries[entriesNum].location, location,
                    strchr(location, '\n') - location); // Copy location to struct variable location
            entries[entriesNum].location[strchr(location, '\n') - location] = 0;
        }
        if (timeStart && timeStart < end && timeEnd < end) // Copy time to struct variable timeEnd
        {
            getToFrom(entries[entriesNum].time, timeStart, timeEnd, &entries[entriesNum].day,
                      &entries[entriesNum].timeStampEnd, &entries[entriesNum].timeStampStart);
        }
        ++entriesNum;
        if(entriesNum == 128)
        {
            break;
        }
    }

    // Sort entries by time
    qsort(entries, entriesNum, sizeof(entry), cmp); // Call function qsort
}

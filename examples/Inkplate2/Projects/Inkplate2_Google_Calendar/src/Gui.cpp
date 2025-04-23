#include "Gui.h"
#include <ctime>

// font
#include "fonts/FreeSans12pt7b.h"
#include "fonts/FreeSans18pt7b.h"
#include "fonts/FreeSans9pt7b.h"
#include "fonts/FreeSans6pt7b.h"
#include "fonts/FreeSansBold24pt7b.h"

Gui::Gui(Inkplate &inkplate) : inkplate(inkplate) {}

String Gui::getDayName(int dayIndex)
{
    const char *days[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
    return days[dayIndex];
}

String Gui::getMonthName(int monthIndex)
{
    const char *months[] = {"January", "February", "March", "April", "May", "June",
                            "July", "August", "September", "October", "November", "December"};
    return months[monthIndex];
}

String Gui::formatHour(const String &isoDateTime)
{
    if (isoDateTime.length() < 16)
        return "";
    return isoDateTime.substring(11, 16); // "HH:MM" from ISO 8601
}

String Gui::formatDate(const String &isoDateTime)
{
    if (isoDateTime.length() < 10)
        return "";
    return isoDateTime.substring(8, 10); // "YYYY-MM-DD"
}

void Gui::wifiError()
{
    inkplate.clearDisplay();
    inkplate.setTextColor(INKPLATE2_BLACK);
    inkplate.setFont(&FreeSans6pt7b);
    inkplate.setCursor(10, 20);
    inkplate.print("WiFi connection failed.");
    inkplate.setCursor(10, 50);
    inkplate.print("Check credentials or try again.");
    inkplate.display();
}

String Gui::getShortDayName(int dayIndex)
{
    const char *shortDays[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    return shortDays[dayIndex];
}


void Gui::showCalendar(calendarData *calendar)
{
    inkplate.clearDisplay();


    // Get current time
    struct tm timeInfo;
    if (!getLocalTime(&timeInfo))
    {
        showError("Time not available");
        return;
    }

    // === Calendar Events ===
    Event *events = calendar->getEvents();
    int eventCount = calendar->getEventCount();
    int y = 12;
    int x = 40;

    String lastDate = "";

    int counter = 0;

    for (int i = 0; i < eventCount; i++)
    {
        inkplate.setTextColor(INKPLATE2_BLACK); // black text again

        String eventDate = formatDate(events[i].startTime);

        // Draw section header if date changes
        if (eventDate != lastDate)
        {
            if (i != 0){
                y += 20;
                if (y >= 85){
                    break;
                }
            }
        

            // Get day of week from date string (assumes format "YYYY-MM-DD")
            struct tm timeStruct = {};
            timeStruct.tm_year = timeInfo.tm_year;  // use current year as fallback
            timeStruct.tm_mon = timeInfo.tm_mon;    // use current month as fallback
            timeStruct.tm_mday = eventDate.toInt(); // parse day from string
            mktime(&timeStruct);                    // normalize to fill in wday

            // Date (big, bold)
            inkplate.setFont(&FreeSans9pt7b);
            inkplate.setTextColor(INKPLATE2_BLACK);
            inkplate.setCursor(5, y + 5);
            inkplate.println(eventDate);

            // Short Day (under date)
            inkplate.setFont(&FreeSans6pt7b);
            inkplate.setCursor(5, y + 15);
            inkplate.println(getShortDayName(timeStruct.tm_wday));

            lastDate = eventDate;
        }

        inkplate.setFont(&FreeSans6pt7b);
        int yLineStart = y;
        int xTime = 175;

        // Draw event summary and time
        inkplate.setCursor(x, y);
        String summary = events[i].summary;
        if (summary.length() > MAX_SUMMARY_LENGTH)
        {
            summary = summary.substring(0, MAX_SUMMARY_LENGTH) + "...";
        }
        inkplate.println(summary);
        inkplate.setCursor(xTime, y);
        inkplate.setFont(&FreeSans6pt7b);
        inkplate.println(formatHour(events[i].startTime));
        inkplate.setTextColor(2);
        y += 10;
        inkplate.setFont(&FreeSans6pt7b);
        inkplate.setCursor(xTime, y);
        inkplate.println(formatHour(events[i].endTime));
        y += 16;

        counter = i;

        if (y >= 85)  // Stop drawing if out of vertical space
        {
            break;
        }
    }

    // Show end message
    if (counter == eventCount - 1 && y < 90)
    {
        inkplate.setTextColor(1);
        inkplate.setFont(&FreeSans6pt7b);
        inkplate.setCursor(10, y + 5);
        inkplate.println("No more events in the next 2 weeks!");
    }

    inkplate.display();
}

// Shows an error message on the display
void Gui::showError(const String &message)
{
    inkplate.clearDisplay();
    inkplate.setFont(&FreeSans6pt7b);
    inkplate.setTextColor(INKPLATE2_BLACK);
    inkplate.setCursor(10, 10);
    inkplate.println("Error:");
    inkplate.println(message);
    inkplate.display();
}

#include "Gui.h"
#include <ctime>

// font
#include "fonts/FreeSans9pt7b.h"
#include "fonts/FreeSans12pt7b.h"
#include "fonts/FreeSans18pt7b.h"
#include "fonts/FreeSans48pt7b.h"
#include "fonts/FreeSansBold48pt7b.h"
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
    inkplate.setTextColor(0);
    inkplate.setFont(&FreeSans18pt7b);
    inkplate.setCursor(50, 150);
    inkplate.print("WiFi connection failed.");
    inkplate.setCursor(50, 200);
    inkplate.print("Check credentials or try again.");
    inkplate.display();
}

void Gui::drawHeader(const String &title)
{
    inkplate.clearDisplay();
    inkplate.setTextSize(3);
    inkplate.setTextColor(0);
    inkplate.setCursor(10, 10);
    inkplate.println(title);
}

String Gui::getShortDayName(int dayIndex)
{
    const char *shortDays[] = {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
    return shortDays[dayIndex];
}

bool Gui::isCurrentEvent(const String &startTimeStr, const String &endTimeStr)
{
    struct tm timeInfo;
    if (!getLocalTime(&timeInfo))
        return false;
    time_t now = mktime(&timeInfo);

    struct tm startTm = {}, endTm = {};
    startTm.tm_year = startTimeStr.substring(0, 4).toInt() - 1900;
    startTm.tm_mon = startTimeStr.substring(5, 7).toInt() - 1;
    startTm.tm_mday = startTimeStr.substring(8, 10).toInt();
    startTm.tm_hour = startTimeStr.substring(11, 13).toInt();
    startTm.tm_min = startTimeStr.substring(14, 16).toInt();

    endTm.tm_year = endTimeStr.substring(0, 4).toInt() - 1900;
    endTm.tm_mon = endTimeStr.substring(5, 7).toInt() - 1;
    endTm.tm_mday = endTimeStr.substring(8, 10).toInt();
    endTm.tm_hour = endTimeStr.substring(11, 13).toInt();
    endTm.tm_min = endTimeStr.substring(14, 16).toInt();

    time_t start = mktime(&startTm);
    time_t end = mktime(&endTm);

    return (now >= start && now <= end);
}

void Gui::showCalendar(calendarData *calendar)
{
    inkplate.clearDisplay();

    // === Top Section (Black Header Box) ===
    inkplate.fillRect(0, 0, 765, 125, 0); // black box

    // Get current time
    struct tm timeInfo;
    if (!getLocalTime(&timeInfo))
    {
        showError("Time not available");
        return;
    }

    inkplate.setFont(&FreeSansBold48pt7b);
    // === Big Date Number (white) ===
    inkplate.setTextColor(7);
    inkplate.setCursor(10, 85);
    inkplate.println(timeInfo.tm_mday);

    inkplate.setFont(&FreeSansBold24pt7b);

    // === Day of the Week (white) ===
    inkplate.setCursor(125, 50);
    inkplate.println(getDayName(timeInfo.tm_wday));

    // === Month + Year (white) ===
    inkplate.setCursor(125, 100);
    inkplate.println(getMonthName(timeInfo.tm_mon) + " " + String(1900 + timeInfo.tm_year));

    // === Last Updated Section (Top Right) ===
    inkplate.setFont(&FreeSans12pt7b);
    inkplate.setCursor(591, 38);
    inkplate.println("Last Updated:");

    char timeString[6]; // HH:MM
    sprintf(timeString, "%02d:%02d", timeInfo.tm_hour, timeInfo.tm_min);

    inkplate.setCursor(675, 60);
    inkplate.println(timeString);

    // === Calendar Events ===
    Event *events = calendar->getEvents();
    int eventCount = calendar->getEventCount();
    int y = 150;
    int x = 115;

    String lastDate = "";

    int counter = 0;

    for (int i = 0; i < eventCount; i++)
    {

        inkplate.setFont(&FreeSans18pt7b);

        inkplate.setTextColor(0); // black text again

        String eventDate = formatDate(events[i].startTime);

        // Draw section header if date changes
        if (eventDate != lastDate)
        {
            y += 50;

            // Get day of week from date string (assumes format "YYYY-MM-DD")
            struct tm timeStruct = {};
            timeStruct.tm_year = timeInfo.tm_year;  // use current year as fallback
            timeStruct.tm_mon = timeInfo.tm_mon;    // use current month as fallback
            timeStruct.tm_mday = eventDate.toInt(); // parse day from string
            mktime(&timeStruct);                    // normalize to fill in wday

            // Date (big, bold)
            inkplate.setFont(&FreeSans18pt7b);
            inkplate.setTextColor(0);
            inkplate.setCursor(15, y);
            inkplate.println(eventDate);

            // Short Day (under date)
            inkplate.setFont(&FreeSans12pt7b);
            inkplate.setCursor(15, y + 30);
            inkplate.println(getShortDayName(timeStruct.tm_wday));

            lastDate = eventDate;
        }

        inkplate.setFont(&FreeSans18pt7b);
        int yLineStart = y;
        int xTime = 640;

        // Highlight if it's happening now
        bool isNow = isCurrentEvent(events[i].startTime, events[i].endTime);
        if (isNow)
        {
            inkplate.fillRoundRect(x - 10, y - 35, 635, 70, 10, 6); // Draw highlight
            String summaryLength = events[i].summary;
            // inkplate.drawLine(x, y + 15, x + summaryLength.length() * 15, y + 15, 0);
        }

        // Draw event summary and time
        inkplate.setCursor(x, y);
        String summary = events[i].summary;
        if (summary.length() > MAX_SUMMARY_LENGTH)
        {
            summary = summary.substring(0, MAX_SUMMARY_LENGTH) + "...";
        }
        inkplate.println(summary);
        inkplate.setCursor(xTime, y);
        inkplate.println(formatHour(events[i].startTime));
        inkplate.setTextColor(2);
        y += 25;
        inkplate.setFont(&FreeSans12pt7b);
        inkplate.setCursor(xTime + 25, y);
        inkplate.println(formatHour(events[i].endTime));
        y += 50;

        // margin drawing
        inkplate.drawLine(85, yLineStart - 43, 85, y - 43, 0);
        inkplate.drawLine(84, yLineStart - 43, 84, y - 43, 0);
        inkplate.drawLine(83, yLineStart - 43, 83, y - 43, 0);

        counter = i;

        if (y >= 925)  // Stop drawing if out of vertical space
        {
            break;
        }
    }

    // Show end message
    if (counter == eventCount - 1 && y < 975)
    {
        inkplate.setTextColor(2);
        inkplate.setFont(&FreeSans12pt7b);
        inkplate.setCursor(185, y + 15);
        inkplate.println("No more events in the next 2 weeks!");
    }

    inkplate.display();
}

// Shows an error message on the display
void Gui::showError(const String &message)
{
    inkplate.clearDisplay();
    inkplate.setTextSize(2);
    inkplate.setTextColor(0);
    inkplate.setCursor(10, 10);
    inkplate.println("Error:");
    inkplate.println(message);
    inkplate.display();
}

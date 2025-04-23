#pragma once

#include <Inkplate.h>
#include "calendarData.h"

#define MAX_SUMMARY_LENGTH 22

class Gui {
public:
    Gui(Inkplate &inkplate);
    void showCalendar(calendarData *calendar);
    void showError(const String &message);
    void wifiError();
    int highlightColor;
    void setHighlightColor(int colorIndex);

private:
    Inkplate &inkplate;
    void drawHeader(const String &title);
    String getDayName(int dayIndex);
    String getMonthName(int monthIndex);
    String formatHour(const String &isoDateTime);
    String formatDate(const String &isoDateTime);
    String getShortDayName(int dayIndex);
    bool isCurrentEvent(const String& startTimeStr, const String& endTimeStr);
};

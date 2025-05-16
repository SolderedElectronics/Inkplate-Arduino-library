#pragma once

#include <Inkplate.h>
#include "calendarData.h"

#define MAX_SUMMARY_LENGTH 15

class Gui {
public:
    Gui(Inkplate &inkplate);
    void showCalendar(calendarData *calendar);
    void showError(const String &message);
    void wifiError();

private:
    Inkplate &inkplate;
    String getDayName(int dayIndex);
    String getMonthName(int monthIndex);
    String formatHour(const String &isoDateTime);
    String formatDate(const String &isoDateTime);
    String getShortDayName(int dayIndex);
};

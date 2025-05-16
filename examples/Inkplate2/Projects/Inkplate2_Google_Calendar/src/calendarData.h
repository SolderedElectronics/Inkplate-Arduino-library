#ifndef CALENDARDATA_H
#define CALENDARDATA_H

#include <Arduino.h>

const int MAX_EVENTS = 16;

struct Event {
    String summary;
    String startTime;
    String endTime;
};

class calendarData {
public:
    calendarData();

    void clearEvents();
    void addEvent(const String& summary, const String& start, const String& end);
    Event* getEvents();
    int getEventCount();

private:
    Event events[MAX_EVENTS];
    int eventCount;
};

#endif

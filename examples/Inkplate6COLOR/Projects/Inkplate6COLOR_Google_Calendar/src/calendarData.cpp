#include "calendarData.h"

// Constructor: initialize event count to 0
calendarData::calendarData() {
    eventCount = 0;
}

// Clears all stored events by resetting the counter
void calendarData::clearEvents() {
    eventCount = 0;
}

// Adds an event if under the maximum limit
void calendarData::addEvent(const String& summary, const String& start, const String& end) {
    if (eventCount < MAX_EVENTS) {
        events[eventCount].summary = summary;     // Set event title
        events[eventCount].startTime = start;     // Set event start time
        events[eventCount].endTime = end;         // Set event end time
        eventCount++;                             // Increment stored event count
    }
}

// Returns pointer to array of events
Event* calendarData::getEvents() {
    return events;
}

// Returns total number of stored events
int calendarData::getEventCount() {
    return eventCount;
}

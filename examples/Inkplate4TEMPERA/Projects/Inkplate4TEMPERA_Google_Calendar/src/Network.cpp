#include "Network.h"

// Constructor to initialize WiFi and Google Calendar credentials
Network::Network(const String& calendarID, const String& apiKey)
    : calendarID(calendarID), apiKey(apiKey) {}

// Fetches calendar events from Google Calendar and fills in the data structure
bool Network::fetchCalendar(calendarData* data) {
    struct tm timeinfo;

    // Retry loop to wait for time sync (up to 10 seconds)
    int attempts = 0;
    while (!getLocalTime(&timeinfo) && attempts < 10) {
        Serial.println("Waiting for time sync...");
        delay(1000);
        attempts++;
    }

    if (!getLocalTime(&timeinfo)) {
        Serial.println("Cannot fetch calendar - time not available");
        return false;
    }

    // Set start time to today's date at 00:00:00 UTC
    char timeMin[64];
    strftime(timeMin, sizeof(timeMin), "%Y-%m-%dT00:00:00Z", &timeinfo);

    // Set end time to 14 days from now at 23:59:59 UTC
    time_t now = mktime(&timeinfo);
    now += 14 * 24 * 60 * 60;  // Add 14 days
    struct tm timeMaxInfo;
    gmtime_r(&now, &timeMaxInfo);  // Convert to UTC time

    char timeMax[64];
    strftime(timeMax, sizeof(timeMax), "%Y-%m-%dT23:59:59Z", &timeMaxInfo);

    // Build the Google Calendar API URL
    String url = "https://www.googleapis.com/calendar/v3/calendars/" + calendarID +
                 "/events?singleEvents=true&orderBy=startTime&" +
                 "timeMin=" + String(timeMin) +
                 "&timeMax=" + String(timeMax) +
                 "&maxResults=14&key=" + apiKey;

    // Make HTTP GET request to fetch calendar events
    HTTPClient http;
    http.begin(url);
    int httpCode = http.GET();

    if (httpCode != 200) {
        Serial.println("HTTP error: " + String(httpCode));
        return false;
    }

    String payload = http.getString();
    http.end();

    // Parse JSON response
    StaticJsonDocument<8192> doc;
    DeserializationError error = deserializeJson(doc, payload);
    if (error) {
        Serial.println("JSON parse error");
        return false;
    }

    // Clear existing events and add new ones
    data->clearEvents();
    JsonArray items = doc["items"];
    for (JsonObject event : items) {
        String summary = event["summary"] | "No Title";
        JsonObject start = event["start"];
        JsonObject end = event["end"];
        String startTime = String((start["dateTime"] | start["date"]).as<const char*>());
        String endTime = String((end["dateTime"] | end["date"]).as<const char*>());

        data->addEvent(summary, startTime, endTime);
    }

    return true;
}

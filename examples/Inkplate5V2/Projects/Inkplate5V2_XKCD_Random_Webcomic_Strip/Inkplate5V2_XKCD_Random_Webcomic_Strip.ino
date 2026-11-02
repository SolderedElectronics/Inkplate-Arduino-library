/*
   Inkplate5V2_XKCD_Random_Webcomic_Strip - Display random XKCD comics

   This example displays random XKCD comics with on-device PNG scaling.
   Button on GPIO 36 skips to next comic, auto-refresh every 60s.

   REQUIRED LIBRARY:
   Install ArduinoJson library from Arduino IDE Library Manager:
   Sketch → Include Library → Manage Libraries → Search "ArduinoJson" → Install

   Hardware needed: Inkplate 5 V2, USB-C cable, WiFi connection

   June 2025 by Piotr Migdał
*/

#include <ArduinoJson.h>
#include "HTTPClient.h"
#include "Inkplate.h"
#include "WiFi.h"
#include "WiFiClientSecure.h"
#include "credentials.h"
#include "pngle_scaling.h"

const uint8_t DISPLAY_MODE = INKPLATE_3BIT;
const unsigned long DISPLAY_DURATION_MS = 60000;
const unsigned long RETRY_DELAY_MS = 3000;
const uint8_t BUTTON_PIN = GPIO_NUM_36;

Inkplate display(DISPLAY_MODE);
int latestComicNumber = 0;

// Download JSON as String (for XKCD API metadata)
String downloadJson(const char* url) {
    // XKCD requires HTTPS. setInsecure() skips certificate verification (ESP32 limitation).
    Serial.printf("[JSON] Downloading: %s\n", url);

    // Small delay before connection to avoid overwhelming network
    delay(100);

    WiFiClientSecure client;
    client.setInsecure();
    client.setTimeout(15);  // 15 second timeout

    HTTPClient http;
    String result = "";
    if (!http.begin(client, url)) {
        Serial.println("[JSON] ERROR: http.begin() failed - cannot connect");
        return result;
    }

    http.setTimeout(15000);
    Serial.println("[JSON] Sending GET request...");
    int httpCode = http.GET();
    Serial.printf("[JSON] HTTP response code: %d\n", httpCode);

    if (httpCode == HTTP_CODE_OK) {
        result = http.getString();
        Serial.printf("[JSON] Downloaded %d bytes\n", result.length());
    } else if (httpCode < 0) {
        Serial.printf("[JSON] ERROR: Connection failed with code %d (likely timeout or connection refused)\n", httpCode);
    } else {
        Serial.printf("[JSON] ERROR: HTTP error code %d\n", httpCode);
    }

    http.end();
    client.stop();
    delay(50);  // Small delay after connection cleanup

    return result;
}

// Download binary data to buffer (for images). Returns nullptr on failure. Caller must free().
uint8_t* downloadToBuffer(const char* url, int32_t* outSize) {
    Serial.printf("[IMAGE] Downloading: %s\n", url);

    // Small delay before connection to avoid overwhelming network
    delay(100);

    WiFiClientSecure client;
    client.setInsecure();
    client.setTimeout(30);  // 30 second timeout for images

    HTTPClient http;

    if (!http.begin(client, url)) {
        Serial.println("[IMAGE] ERROR: http.begin() failed - cannot connect");
        return nullptr;
    }

    http.setTimeout(30000);
    Serial.println("[IMAGE] Sending HTTP GET request...");
    unsigned long startTime = millis();
    int httpCode = http.GET();
    unsigned long elapsed = millis() - startTime;
    Serial.printf("[IMAGE] HTTP response code: %d (took %lu ms)\n", httpCode, elapsed);

    if (httpCode != HTTP_CODE_OK) {
        if (httpCode == -1) {
            Serial.println("[IMAGE] ERROR: Connection failed (code -1)");
            Serial.println("[IMAGE] Possible causes: timeout, connection refused, SSL handshake failed");
            Serial.printf("[IMAGE] WiFi status: %d, Signal strength: %d dBm\n",
                         WiFi.status(), WiFi.RSSI());
        } else if (httpCode < 0) {
            Serial.printf("[IMAGE] ERROR: HTTP client error %d\n", httpCode);
        } else {
            Serial.printf("[IMAGE] ERROR: HTTP server error %d\n", httpCode);
        }
        http.end();
        client.stop();
        return nullptr;
    }

    WiFiClient* stream = http.getStreamPtr();
    int32_t len = http.getSize();
    Serial.printf("[IMAGE] Content length: %d bytes\n", len);

    if (len <= 0) {
        Serial.printf("[IMAGE] ERROR: Invalid content length: %d\n", len);
        http.end();
        client.stop();
        return nullptr;
    }

    Serial.printf("[IMAGE] Allocating %d bytes in PSRAM...\n", len);
    uint8_t* buffer = (uint8_t*)ps_malloc(len);
    if (!buffer) {
        Serial.printf("[IMAGE] ERROR: Memory allocation failed for %d bytes\n", len);
        Serial.printf("[IMAGE] Free heap: %u, Free PSRAM: %u\n",
                     ESP.getFreeHeap(), ESP.getFreePsram());
        http.end();
        client.stop();
        return nullptr;
    }

    Serial.println("[IMAGE] Reading image data...");
    size_t bytesRead = 0;
    unsigned long downloadStart = millis();

    // Read in chunks until all data is received
    while (bytesRead < len) {
        // Wait for data to be available
        while (!stream->available() && (millis() - downloadStart < 30000)) {
            delay(10);
        }

        if (!stream->available()) {
            Serial.printf("[IMAGE] ERROR: Stream stalled at %d/%d bytes\n", bytesRead, len);
            free(buffer);
            http.end();
            client.stop();
            return nullptr;
        }

        // Read up to 1024 bytes at a time
        size_t toRead = min((size_t)(len - bytesRead), (size_t)1024);
        size_t read = stream->readBytes(buffer + bytesRead, toRead);
        bytesRead += read;

        if (read == 0) {
            Serial.printf("[IMAGE] ERROR: Read returned 0 at %d/%d bytes\n", bytesRead, len);
            free(buffer);
            http.end();
            client.stop();
            return nullptr;
        }

        // Progress reporting every 10KB
        if (bytesRead % 10240 == 0 || bytesRead == len) {
            Serial.printf("[IMAGE] Progress: %d/%d bytes (%.1f%%)\n",
                         bytesRead, len, (bytesRead * 100.0) / len);
        }

        // Overall timeout check
        if (millis() - downloadStart > 30000) {
            Serial.printf("[IMAGE] ERROR: Download timeout at %d/%d bytes\n", bytesRead, len);
            free(buffer);
            http.end();
            client.stop();
            return nullptr;
        }
    }

    Serial.printf("[IMAGE] Download complete: %d bytes in %lu ms\n",
                 bytesRead, millis() - downloadStart);

    http.end();
    client.stop();
    delay(50);  // Small delay after connection cleanup

    *outSize = len;
    return buffer;
}

// Word-wrap text to fit within maxWidth. Breaks on spaces, max 10 lines.
void drawWrappedText(String text, int x, int y, int maxWidth) {
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds("A", 0, 0, &x1, &y1, &w, &h);
    int lineHeight = h + 2;

    String lines[10];
    int lineCount = 0;
    String currentLine = "";
    String word = "";

    for (int i = 0; i <= text.length(); i++) {
        char c = (i < text.length()) ? text.charAt(i) : ' ';

        if (c == ' ' || i == text.length()) {
            if (word.length() > 0) {
                String testLine = currentLine.length() > 0 ? currentLine + " " + word : word;
                display.getTextBounds(testLine, 0, 0, &x1, &y1, &w, &h);

                if (w > maxWidth && currentLine.length() > 0) {
                    if (lineCount < 10) lines[lineCount++] = currentLine;
                    currentLine = word;
                } else {
                    currentLine = testLine;
                }
                word = "";
            }
        } else {
            word += c;
        }
    }
    if (currentLine.length() > 0 && lineCount < 10) {
        lines[lineCount++] = currentLine;
    }

    for (int i = 0; i < lineCount; i++) {
        display.setCursor(x, y + (i * lineHeight));
        display.print(lines[i]);
    }
}

// Display comic title and alt text at bottom of screen
void displayComicText(String title, String altText, int comicNum) {
    String text = "XKCD #" + String(comicNum) + ": " + title + " - " + altText;

    int16_t x1, y1;
    uint16_t w, h;
    display.setTextSize(2);
    display.getTextBounds("A", 0, 0, &x1, &y1, &w, &h);
    int startY = display.height() - (h * 4) - 10;  // Reserve ~4 lines of text at bottom

    display.setTextColor(0, 7);
    drawWrappedText(text, 10, startY, display.width() - 20);
    display.display();
}

void displayStatus(String message) {
    display.clearDisplay();
    display.setCursor(10, 10);
    display.setTextSize(2);
    display.print(message);
    display.display();
    Serial.println(message);
    delay(100);
}

bool waitForButtonOrTimeout(unsigned long timeoutMs) {
    unsigned long startTime = millis();

    while (millis() - startTime < timeoutMs) {
        if (digitalRead(BUTTON_PIN) == LOW) {
            delay(50);
            while (digitalRead(BUTTON_PIN) == LOW) delay(10);
            delay(50);
            Serial.println("Button pressed - loading next comic");
            return true;
        }
        delay(100);
    }
    return false;
}

bool tryDisplayComic(int randomNum) {
    Serial.printf("\n[COMIC] ===== Trying XKCD #%d =====\n", randomNum);
    displayStatus("Loading comic #" + String(randomNum) + "...");

    String comicUrl = "https://xkcd.com/" + String(randomNum) + "/info.0.json";
    String comicJson = downloadJson(comicUrl.c_str());
    if (comicJson.length() == 0) {
        Serial.println("[COMIC] ERROR: Failed to download JSON metadata");
        return false;
    }

    Serial.println("[COMIC] Parsing JSON metadata...");
    JsonDocument comicDoc;
    if (deserializeJson(comicDoc, comicJson)) {
        Serial.println("[COMIC] ERROR: Failed to parse JSON");
        return false;
    }

    String imgUrl = comicDoc["img"].as<String>();
    String title = comicDoc["safe_title"].as<String>();
    String altText = comicDoc["alt"].as<String>();
    Serial.printf("[COMIC] Title: %s\n", title.c_str());
    Serial.printf("[COMIC] Image URL: %s\n", imgUrl.c_str());

    if (imgUrl.length() == 0) {
        Serial.println("[COMIC] ERROR: Empty image URL");
        return false;
    }

    int32_t imageSize = 0;
    uint8_t* imageBuffer = downloadToBuffer(imgUrl.c_str(), &imageSize);
    if (!imageBuffer) {
        Serial.println("[COMIC] ERROR: Failed to download image");
        return false;
    }

    Serial.printf("[COMIC] Image downloaded: %d bytes\n", imageSize);
    uint16_t imageAreaHeight = display.height() - 60;
    bool imageDisplayed = false;

    if (imgUrl.endsWith(".png")) {
        Serial.println("[COMIC] Decoding as PNG...");
        imageDisplayed = drawScaledPngFromBuffer(imageBuffer, imageSize, display.width(), imageAreaHeight, false, false);
    } else if (imgUrl.endsWith(".jpg") || imgUrl.endsWith(".jpeg")) {
        Serial.println("[COMIC] Decoding as JPEG...");
        display.clearDisplay();
        imageDisplayed = display.drawJpegFromBuffer(imageBuffer, imageSize, 0, 0, true, false);
    } else {
        Serial.println("[COMIC] Unknown format, trying PNG...");
        imageDisplayed = drawScaledPngFromBuffer(imageBuffer, imageSize, display.width(), imageAreaHeight, false, false);
    }

    free(imageBuffer);

    if (imageDisplayed) {
        Serial.println("[COMIC] Image decoded successfully");
        displayComicText(title, altText, randomNum);
        Serial.printf("[COMIC] SUCCESS: Displayed XKCD #%d: %s\n", randomNum, title.c_str());

        Serial.printf("[COMIC] Waiting %lu ms for button or timeout...\n", DISPLAY_DURATION_MS);
        if (!waitForButtonOrTimeout(DISPLAY_DURATION_MS)) {
            Serial.println("[COMIC] Timeout - moving to next comic");
        }
        return true;
    }

    Serial.println("[COMIC] ERROR: Image decode/display failed");
    return false;
}

void setup() {
    display.begin();
    Serial.begin(115200);
    while (!Serial);
    pinMode(BUTTON_PIN, INPUT);
    randomSeed(esp_random());

    displayStatus("Connecting to WiFi...");
    WiFi.mode(WIFI_MODE_STA);
    WiFi.begin(ssid, pass);

    display.setCursor(250, 10);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        display.print(".");
        display.partialUpdate();
    }

    displayStatus("WiFi connected!");

    // Fetch latest comic number once at startup
    displayStatus("Getting latest comic...");
    String latestJson = downloadJson("https://xkcd.com/info.0.json");
    if (latestJson.length() == 0) {
        displayStatus("ERROR: Cannot fetch latest comic");
        Serial.println("HALTED: Failed to fetch latest comic number");
        while(1) delay(1000);
    }

    JsonDocument latestDoc;
    if (deserializeJson(latestDoc, latestJson)) {
        displayStatus("ERROR: JSON parse failed");
        Serial.println("HALTED: Failed to parse latest comic JSON");
        while(1) delay(1000);
    }

    latestComicNumber = latestDoc["num"];
    if (latestComicNumber == 0) {
        displayStatus("ERROR: Invalid comic number");
        Serial.println("HALTED: Latest comic number is invalid");
        while(1) delay(1000);
    }

    Serial.printf("Latest XKCD comic: #%d\n", latestComicNumber);
    displayStatus("Ready!");
    delay(1000);
}

void loop() {
    Serial.printf("\n[LOOP] Free heap: %u bytes\n", ESP.getFreeHeap());

    // Try one random comic
    randomSeed(esp_random());
    int randomNum = random(1, latestComicNumber + 1);
    if (randomNum == 404) {
        Serial.println("[LOOP] Skipped #404 (doesn't exist)");
        randomNum = 1;
    }
    Serial.printf("[LOOP] Selected random comic #%d (out of %d)\n", randomNum, latestComicNumber);

    if (!tryDisplayComic(randomNum)) {
        Serial.printf("[LOOP] Comic failed, waiting %lu ms before retry...\n", RETRY_DELAY_MS);
        displayStatus("Failed to load comic");
        delay(RETRY_DELAY_MS);
    }
}

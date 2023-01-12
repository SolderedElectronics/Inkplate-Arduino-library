// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_INKPLATECOLOR) && !defined(ARDUINO_INKPLATECOLORV2)
#error "Wrong board selection for this example, please select Inkplate 6 Color in the boards menu."
#endif

// Do not forget to add WiFi SSID and WiFi Password in test.cpp!

#include "EEPROM.h"
#include "Inkplate.h"
#include "image.h"
#include <Wire.h>
#include "test.h"

Inkplate display;

// EEPROMaddress must be between 0 and 64
int EEPROMaddress = 4;

// Peripheral mode variables and arrays
#define BUFFER_SIZE 1000
char commandBuffer[BUFFER_SIZE + 1];
char strTemp[2001];

void setup()
{
    Serial.begin(115200);
    display.begin();

    EEPROM.begin(512);
    EEPROM.write(EEPROMaddress, 144);
    EEPROM.commit();

    // Test peripherials of the Inkplate.
    if (EEPROM.read(EEPROMaddress) != 170)
    {
        // Test all peripherals of the Inkplate (I/O expander, RTC, uSD card holdler, etc).
        testPeripheral();

        EEPROM.write(EEPROMaddress, 170);
        EEPROM.commit();
    }
    else
    {
        Serial.println("Factory already done");
    }
    memset(commandBuffer, 0, BUFFER_SIZE);

    showSplashScreen();
}

void loop()
{
    if (Serial.available())
    {
        while (Serial.available())
        {
            for (int i = 0; i < (BUFFER_SIZE - 1); i++)
            {
                commandBuffer[i] = commandBuffer[i + 1];
            }
            commandBuffer[BUFFER_SIZE - 1] = Serial.read();
        }
    }
    char *s = NULL;
    char *e = NULL;
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        if (commandBuffer[i] == '#' && s == NULL)
            s = &commandBuffer[i];
        if (commandBuffer[i] == '*' && e == NULL)
            e = &commandBuffer[i];
    }
    if (s != NULL && e != NULL)
    {
        if ((e - s) > 0)
        {
            int x, x1, x2, y, y1, y2, x3, y3, l, c, w, h, r, n;
            char b;
            switch (*(s + 1))
            {
                case '?':
                    Serial.print("OK");
                    break;

                case '0':
                    sscanf(s + 3, "%d,%d,%d", &x, &y, &c);
                    display.drawPixel(x, y, c);
                    break;

                case '1':
                    sscanf(s + 3, "%d,%d,%d,%d,%d", &x1, &y1, &x2, &y2, &c);
                    display.drawLine(x1, y1, x2, y2, c);
                    break;

                case '2':
                    sscanf(s + 3, "%d,%d,%d,%d", &x, &y, &l, &c);
                    display.drawFastVLine(x, y, l, c);
                    break;

                case '3':
                    sscanf(s + 3, "%d,%d,%d,%d", &x, &y, &l, &c);
                    display.drawFastHLine(x, y, l, c);
                    break;

                case '4':
                    sscanf(s + 3, "%d,%d,%d,%d,%d", &x, &y, &w, &h, &c);
                    display.drawRect(x, y, w, h, c);
                    break;

                case '5':
                    sscanf(s + 3, "%d,%d,%d,%d", &x, &y, &r, &c);
                    display.drawCircle(x, y, r, c);
                    break;

                case '6':
                    sscanf(s + 3, "%d,%d,%d,%d,%d,%d,%d", &x1, &y1, &x2, &y2, &x3, &y3, &c);
                    display.drawTriangle(x1, y1, x2, y2, x3, y3, c);
                    break;

                case '7':
                    sscanf(s + 3, "%d,%d,%d,%d,%d,%d", &x, &y, &w, &h, &r, &c);
                    display.drawRoundRect(x, y, w, h, r, c);
                    break;

                case '8':
                    sscanf(s + 3, "%d,%d,%d,%d,%d", &x, &y, &w, &h, &c);
                    display.fillRect(x, y, w, h, c);
                    break;

                case '9':
                    sscanf(s + 3, "%d,%d,%d,%d", &x, &y, &r, &c);
                    display.fillCircle(x, y, r, c);
                    break;

                case 'A':
                    sscanf(s + 3, "%d,%d,%d,%d,%d,%d,%d", &x1, &y1, &x2, &y2, &x3, &y3, &c);
                    display.fillTriangle(x1, y1, x2, y2, x3, y3, c);
                    break;

                case 'B':
                    sscanf(s + 3, "%d,%d,%d,%d,%d,%d", &x, &y, &w, &h, &r, &c);
                    display.fillRoundRect(x, y, w, h, r, c);
                    break;

                case 'C':
                    sscanf(s + 3, "\"%2000[^\"]\"", strTemp);
                    n = strlen(strTemp);
                    for (int i = 0; i < n; i++)
                    {
                        strTemp[i] = toupper(strTemp[i]);
                    }
                    for (int i = 0; i < n; i += 2)
                    {
                        strTemp[i / 2] = (hexToChar(strTemp[i]) << 4) | (hexToChar(strTemp[i + 1]) & 0x0F);
                    }
                    strTemp[n / 2] = 0;
                    display.print(strTemp);
                    break;

                case 'D':
                    sscanf(s + 3, "%d", &c);
                    display.setTextSize(c);
                    break;

                case 'E':
                    sscanf(s + 3, "%d,%d", &x, &y);
                    display.setCursor(x, y);
                    break;

                case 'F':
                    sscanf(s + 3, "%c", &b);
                    if (b == 'T')
                        display.setTextWrap(true);
                    if (b == 'F')
                        display.setTextWrap(false);
                    break;

                case 'G':
                    sscanf(s + 3, "%d", &c);
                    c &= 3;
                    display.setRotation(c);
                    break;

                case 'H':
                    sscanf(s + 3, "%d,%d,\"%149[^\"]\"", &x, &y, strTemp);
                    n = strlen(strTemp);
                    for (int i = 0; i < n; i++)
                    {
                        strTemp[i] = toupper(strTemp[i]);
                    }
                    for (int i = 0; i < n; i += 2)
                    {
                        strTemp[i / 2] = (hexToChar(strTemp[i]) << 4) | (hexToChar(strTemp[i + 1]) & 0x0F);
                    }
                    strTemp[n / 2] = 0;
                    r = display.sdCardInit();
                    if (r)
                    {
                        r = display.drawBitmapFromSd(strTemp, x, y);
                        Serial.print("#H(");
                        Serial.print(r, DEC);
                        Serial.println(")*");
                        Serial.flush();
                    }
                    else
                    {
                        Serial.println("#H(-1)*");
                        Serial.flush();
                    }
                    break;

                case 'K':
                    sscanf(s + 3, "%c", &b);
                    if (b == '1')
                    {
                        display.clearDisplay();
                    }
                    break;

                case 'L':
                    sscanf(s + 3, "%c", &b);
                    if (b == '1')
                    {
                        display.display();
                    }
                    break;

                case 'O':
                    sscanf(s + 3, "%d", &c);
                    if (c >= 0 && c <= 2)
                    {
                        Serial.print("#O(");
                        //Serial.print(display.readTouchpad(c), DEC);
                        Serial.println(")*");
                        Serial.flush();
                    }
                    break;

                case 'P':
                    sscanf(s + 3, "%c", &b);
                    if (b == '?')
                    {
                        Serial.print("#P(");
                        Serial.print(display.readBattery(), 2);
                        Serial.println(")*");
                        Serial.flush();
                    }
                    break;
            }
            *s = 0;
            *e = 0;
        }
    }
}

int hexToChar(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    return -1;
}

void showSplashScreen()
{
    display.clearDisplay();
    display.drawImage(demo_image, 0, 0, demo_image_w, demo_image_h);
    display.display();
}
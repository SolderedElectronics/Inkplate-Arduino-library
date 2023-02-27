#include "test.h"

const char sdCardTestStringLength = 100;
const char *testString = {"This is some test string..."};

const char *WSSID = {"Soldered"};
const char *WPASS = {"dasduino"};

void testPeripheral()
{
    // Send test reports to the UART (this epaper is slow and does not support partial update)
    Serial.println("INKPLATE CHECKLIST");

    // Check I/O expander
    Serial.printf("- I/O Expander: ");

    // Try to communicate with I/O expander
    Wire.beginTransmission(0x20);           // Send address 0x20
    if (Wire.endTransmission() == 0)        // Check if there was an error in communication and print out the results on UART.
    {
        Serial.println("OK");
    }
    else
    {
        // Hang the code if test fails.
        Serial.println("FAIL");
        failHandler();
    }

    // Check the micro SD card slot
    Serial.print("- microSD card slot: ");
    if (checkMicroSDCard())
    {
        Serial.println("OK");
    }
    else
    {
        Serial.println("FAIL");
        failHandler();
    }

    // Check the WiFi
    Serial.print("- WiFi: ");
    if (checkWiFi(WSSID, WPASS, WTIMEOUT))
    {
        Serial.println("OK");
    }
    else
    {
        Serial.println("FAIL");
        failHandler();
    }

    // Check the RTC
    Serial.print("- PCF85063 RTC: ");
    if (rtcCheck())
    {
        Serial.println("OK");
    }
    else
    {
        Serial.println("FAIL");
        failHandler();
    }

    // This test only must be run on older Inkplates (e-radionica.com Inkplates with touchpads)
#ifdef ARDUINO_INKPLATECOLOR
    // Test Touchpads
    if (touchPads(TOUCHPADS_TIMEOUT))
    {
        Serial.println(" OK");
    }
    else
    {
        Serial.println(" FAIL");
        failHandler();
    }
 #endif
}

int checkWiFi(const char *_ssid, const char *_pass, uint8_t _wifiTimeout)
{
    unsigned long _timeout = millis();

    // Try to connect to WiFi network
    WiFi.begin(WSSID, WPASS);

    // Wait until WiFi connection is established or timeout has occured.
    while ((WiFi.status() != WL_CONNECTED) && ((unsigned long)(millis() - _timeout) < (_wifiTimeout * 1000UL)));

     // Check if board is connected to WiFi
    if (WiFi.status() == WL_CONNECTED)
    {
        return 1;
    }
    else
    {
        return 0;
    }

    // Something is wrong if you got there.
    return 0;
}

int checkMicroSDCard()
{
    int sdInitOk = 0;
    sdInitOk = display.sdCardInit();

    if (sdInitOk)
    {
        File file;

        if (file.open("/testFile.txt", O_CREAT | O_RDWR))
        {
            file.print(testString);
            file.close();
        }
        else
        {
            return 0;
        }

        delay(250);

        if (file.open("/testFile.txt", O_RDWR))
        {
            char sdCardString[sdCardTestStringLength];
            file.read(sdCardString, sizeof(sdCardString));
            sdCardString[file.fileSize()] = 0;
            int stringCompare = strcmp(testString, sdCardString);
            file.remove();
            file.close();
            if (stringCompare != 0)
                return 0;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
    return 1;
}

int rtcCheck()
{
    // First "ping" RTC on the I2C protocol and reset the RTC
    Wire.beginTransmission(0x51);
    int _res = Wire.endTransmission();

    // If result is from I2C is anything else than success (_res = 0), return 0 (error).
    if (_res != 0) return 0;

    // Reset and re-init RTC.
    display.rtcReset();

    // Set some time in epoch in RTC.
    uint32_t _epoch = 1640995200ULL;
    display.rtcSetEpoch(_epoch);

    // Wait at least one and a half second
    delay(1500);

    // Read the epoch (if everything is ok, epoch from RTC should not be the same)
    if (display.rtcGetEpoch() != _epoch)
    {
        return 1;
    }
    else
    {
        return 0;
    }

    return 0;
}

int touchPads(uint8_t _timeoutTouchpads)
{
    // This test only must be run on older Inkplates (e-radionica.com Inkplates with touchpads)
#ifdef ARDUINO_INKPLATECOLOR
    // Variable for storing detected touch
    int _flags = 0;

    // Show message on display
    Serial.print("- Touchpads [Press them!]: ");

    // Wait until all touchpads are pressed at least once in test.

    unsigned long _timeout = millis();
    while ((_flags != 0b00000111) && ((unsigned long)(millis() - _timeout) < (_timeoutTouchpads * 1000UL)))
    {
        for (int i = 0; i < 3; i++)
        {
            if (display.readTouchpad(PAD1 + i) && (!(_flags & (1 << i))))
            {
              _flags |= (1 << i);
              Serial.print(i + 1, DEC);
            }
        }
    }

    if ((millis() - _timeout) >= (_timeoutTouchpads * 1000UL))
    {
        return 0;
    }
    else
    {
        return 1;
    }

    return 0;
#endif
    return 1;
}

// Show a message and stop the code from executing.
void failHandler()
{
    Serial.print(" -> Test stopped!");

    // Inf. loop... halt the program!
    while (true);
}

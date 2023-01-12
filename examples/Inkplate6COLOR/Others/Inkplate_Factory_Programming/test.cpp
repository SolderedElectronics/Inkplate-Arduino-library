#include "test.h"

const char sdCardTestStringLength = 100;
const char *testString = {"This is some test string..."};

const char *WSSID = {"Soldered"};
const char *WPASS = {"dasduino"};

void testPeripheral()
{
    // Set display for test report
    // Send test reports to the UART (this epaper is slow and does not support partial update)
    Serial.println("INKPLATE CHECKLIST");

    // Check I/O expander internal
    Serial.println("- I/O Expander Internal:");
    // Try to communicate with I/O expander
    Wire.beginTransmission(IO_INT_ADDR);
    if (Wire.endTransmission() ==
        0) // Check if there was an error in communication and print out the results on display.
    {
        Serial.println("OK");
    }
    else
    {
        Serial.println("FAIL");
        failHandler();
    }

    // Check I/O expander 1
    Serial.println("- I/O Expander External:");

    // Try to communicate with I/O expander
    Wire.beginTransmission(IO_EXT_ADDR);
    if (Wire.endTransmission() ==
        0) // Check if there was an error in communication and print out the results on display.
    {
        Serial.println("OK");
    }
    else
    {
        Serial.println("FAIL");
        failHandler();
    }

    // Check the micro SD card slot
    Serial.println("- microSD card slot: ");
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
    Serial.println("- WiFi: ");
    if (checkWiFi(WSSID, WPASS, WTIMEOUT))
    {
        Serial.println("OK");
    }
    else
    {
        Serial.println("FAIL");
        failHandler();
    }

    // First version of the Inkplate doesn't have RTC.

    // Check the RTC
    Serial.println("- PCF85063 RTC: ");
    if (rtcCheck())
    {
        Serial.println("OK");
    }
    else
    {
        Serial.println("FAIL");
        failHandler();
    }


    // Check I2C (easyc)
    // A slave must be connected via easyC address (0x28)
    Serial.println("- I2C (easyC): ");
    if (checkI2C(0x28))
    {
        Serial.println("OK");
    }
    else
    {
        Serial.println("FAIL");
        failHandler();
    }

    // Check battery
    // A battery must be connected
    Serial.println("- Battery and temperature: ");
    if (checkBatteryAndTemp())
    {
        Serial.println("OK");
    }
    else
    {
        Serial.println("FAIL");
        failHandler();
    }
}

int checkWiFi(const char *_ssid, const char *_pass, uint8_t _wifiTimeout)
{
    unsigned long _timeout = millis();

    // Try to connect to WiFi network
    WiFi.begin(WSSID, WPASS);

    // Wait until WiFi connection is established or timeout has occured.
    while ((WiFi.status() != WL_CONNECTED) && ((unsigned long)(millis() - _timeout) < (_wifiTimeout * 1000UL)))
        ;

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

int checkI2C(int address)
{
    Wire.beginTransmission(address);
    if (Wire.endTransmission() == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int checkBatteryAndTemp()
{
    int temperature;
    float voltage;
    int result = 1;

    voltage = display.readBattery();

    Serial.println(temperature);
    Serial.println(temperature);
    Serial.println(voltage);
    Serial.println(voltage);

    // ToDo print napone na display

    if (temperature < -10 || temperature > 85)
    {
        result = 0;
    }
    if (voltage <= 3 || voltage > 4.3)
    {
        result = 0;
    }
    return result;
}

int rtcCheck()
{
    // First "ping" RTC on the I2C protocol and reset the RTC
    Wire.beginTransmission(0x51);
    int _res = Wire.endTransmission();

    // If result is from I2C is anything else than success (_res = 0), return 0 (error).
    if (_res != 0)
        return 0;

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

// Show a message and stop the code from executing.
void failHandler()
{
    Serial.println(" -> Test stopped!");
    
    // Inf. loop... halt the program!
    while (true)
        ;
}

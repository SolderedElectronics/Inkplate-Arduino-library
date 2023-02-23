#include "test.h"

const char sdCardTestStringLength = 100;
const char *testString = {"This is some test string..."};

const char *WSSID = {"Soldered-testingPurposes"};
const char *WPASS = {"Testing443"};

// Change this to your used slave device
const uint8_t easyCDeviceAddress = 0x30;

void testPeripheral()
{
    // Set display for test report
    // Send test reports to the UART (this epaper is slow and does not support partial update)
    Serial.println("INKPLATE CHECKLIST");

    // Check I/O expander internal
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

    // First version of the Inkplate doesn't have RTC.

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


    // Check I2C (easyc)
    // A slave must be connected via easyC address (0x28)
    Serial.print("- I2C (easyC): ");
    if (checkI2C(easyCDeviceAddress))
    {
        Serial.println("OK");
    }
    else
    {
        Serial.println("FAIL");
        failHandler();
    }

    // Check battery
    float batteryVoltage = 0.0;
    Serial.print("- Battery: ");
    if (checkBattery(&batteryVoltage))
    {
        Serial.print(batteryVoltage);
        Serial.print("V ");
        Serial.println("OK");
    }
    else
    {
        Serial.println("FAIL");
        failHandler();
    }

    // Text wake up button
    long beginWakeUpTest = millis();
    int wakeButtonState = digitalRead(GPIO_NUM_36);

    Serial.println("Press WAKEUP button within 30 seconds to finish testing...");
    while (true)
    {
        long now = millis();
        if (now - beginWakeUpTest > 30000)
        {
            Serial.println("WAKEUP not pressed for 30 seconds!");
            failHandler();
        }

        if (digitalRead(GPIO_NUM_36) != wakeButtonState)
        {
            break;
        }
        delay(1);
    }
    Serial.println("WAKEUP button pressed!");
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
    Wire.setTimeOut(3000);
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

int checkBattery(float *batVoltage)
{
    float voltage;
    voltage = display.readBattery();
    *batVoltage = voltage;

    // Check the battery voltage.
    // If the measured voltage is below 2.8V and above 4.6V, charger is dead.
    if (voltage <= 2.8 || voltage >= 4.6)
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
        delay(1000);
}

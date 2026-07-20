#include "test.h"

// Use a proper size type for buffer lengths
static constexpr size_t sdCardTestStringLength = 100;

// Keep this short so it always fits comfortably
static const char *testString = "This is some test string...";

static const char *WSSID = "Soldered-testingPurposes";
static const char *WPASS = "Testing443";

// Change this to your used slave device
static const uint8_t easyCDeviceAddress = 0x30;

void testPeripheral()
{
    Serial.println("INKPLATE CHECKLIST");

    // Check I/O expander
    Serial.print("- I/O Expander: ");
    Wire.beginTransmission(IO_INT_ADDR);
    if (Wire.endTransmission() == 0)
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
    float batteryVoltage = 0.0f;
    Serial.print("- Battery: ");
    if (checkBattery(&batteryVoltage))
    {
        Serial.print(batteryVoltage, 2);
        Serial.print("V ");
        Serial.println("OK");
    }
    else
    {
        Serial.println("FAIL");
        failHandler();
    }

    // Test wake up button
    unsigned long beginWakeUpTest = millis();
    int wakeButtonState = digitalRead(GPIO_NUM_18);

    Serial.println("Press WAKEUP button within 30 seconds to finish testing...");
    while (true)
    {
        unsigned long now = millis();
        if (now - beginWakeUpTest > 30000UL)
        {
            Serial.println("WAKEUP not pressed for 30 seconds!");
            failHandler();
        }

        if (digitalRead(GPIO_NUM_18) != wakeButtonState)
        {
            break;
        }
        delay(1);
    }
    Serial.println("WAKEUP button pressed!");
}

int checkWiFi(const char *_ssid, const char *_pass, uint8_t _wifiTimeout)
{
    unsigned long start = millis();

    // FIX: use parameters, not globals
    WiFi.begin(_ssid, _pass);

    while ((WiFi.status() != WL_CONNECTED) &&
           ((unsigned long)(millis() - start) < (static_cast<unsigned long>(_wifiTimeout) * 1000UL)))
    {
        delay(10);
    }

    return (WiFi.status() == WL_CONNECTED) ? 1 : 0;
}

int checkMicroSDCard()
{
    if (!inkplate.sdCardInit())
        return 0;

    File file;

    // Create/truncate so there is no leftover data
    if (!file.open("/testFile.txt", O_CREAT | O_TRUNC | O_RDWR))
        return 0;

    file.print(testString);
    file.close();

    delay(50);

    // Read back
    if (!file.open("/testFile.txt", O_RDONLY))
        return 0;

    // +1 for terminator
    char sdCardString[sdCardTestStringLength + 1];
    size_t bytesRead = file.read(sdCardString, sdCardTestStringLength);
    sdCardString[bytesRead] = '\0';
    file.close();

    // Compare
    if (strcmp(testString, sdCardString) != 0)
    {
        // Best-effort cleanup: reopen and remove open file
        if (file.open("/testFile.txt", O_RDWR))
        {
            file.remove();   // SdFat: remove currently open file
            file.close();
        }
        return 0;
    }

    // Cleanup: reopen and remove open file
    if (file.open("/testFile.txt", O_RDWR))
    {
        file.remove();
        file.close();
    }

    return 1;
}


int checkI2C(int address)
{
    Wire.beginTransmission(address);
    return (Wire.endTransmission() == 0) ? 1 : 0;
}

int checkBattery(float *batVoltage)
{
    float voltage = static_cast<float>(inkplate.readBattery());
    *batVoltage = voltage;

    // If the measured voltage is below 2.8V or above 4.6V, something is wrong.
    if (voltage <= 2.8f || voltage >= 4.6f)
        return 0;

    return 1;
}

int rtcCheck()
{
    Wire.beginTransmission(0x51);
    int res = Wire.endTransmission();

    if (res != 0)
        return 0;

    inkplate.rtc.reset();

    // 2022-01-01 00:00:00 UTC
    uint32_t epoch = 1640995200UL;
    inkplate.rtc.setEpoch(epoch);

    delay(1500);

    return (inkplate.rtc.getEpoch() != epoch) ? 1 : 0;
}

void failHandler()
{
    Serial.println(" -> Test stopped!");
    while (true)
        delay(1000);
}

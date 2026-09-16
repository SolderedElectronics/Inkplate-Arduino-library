#include "test.h"

// Use a proper size type for buffer lengths
static constexpr size_t sdCardTestStringLength = 100;

// Keep this short so it always fits comfortably
static const char *testString = "This is some test string...";

static const char *WSSID = "Soldered-testingPurposes";
static const char *WPASS = "Testing443";

// Change this to your used slave device
static const uint8_t qwiicDeviceAddress = 0x30;

// Print the result of a single test on the serial in a machine readable format ("NAME:PASS" / "NAME:FAIL").
void testResult(const char *_testName, bool _passed)
{
    Serial.print(_testName);
    Serial.println(_passed ? ":PASS" : ":FAIL");
}

// Ask the operator to confirm a visual/audible check over the serial.
// Prints the result as "NAME:PASS" or "NAME:FAIL". Returns 1 on 'y', 0 on 'n' or on timeout.
int askOperator(const char *_testName, const char *_question, uint8_t _timeout)
{
    // Drop anything left in the serial buffer from a previous test.
    while (Serial.available())
        Serial.read();

    Serial.print(_testName);
    Serial.print(": ");
    Serial.print(_question);
    Serial.print(" (y/n, ");
    Serial.print(_timeout);
    Serial.println("s)");

    unsigned long _timeoutStart = millis();
    while ((unsigned long)(millis() - _timeoutStart) < (_timeout * 1000UL))
    {
        if (Serial.available())
        {
            char _answer = Serial.read();
            if ((_answer == 'y') || (_answer == 'Y'))
            {
                testResult(_testName, true);
                return 1;
            }
            if ((_answer == 'n') || (_answer == 'N'))
            {
                testResult(_testName, false);
                return 0;
            }
        }
        delay(1);
    }

    // No answer from the operator, treat it as a failed test.
    testResult(_testName, false);
    return 0;
}

void testPeripheral()
{
    Serial.println("INKPLATE CHECKLIST");

    // Check I/O expander
    Serial.println("- I/O Expander:");
    Wire.beginTransmission(IO_INT_ADDR);
    if (Wire.endTransmission() == 0)
    {
        testResult("IO_EXP", true);
    }
    else
    {
        testResult("IO_EXP", false);
        failHandler();
    }

    // Check the micro SD card slot
    Serial.println("- microSD card slot:");
    if (checkMicroSDCard())
    {
        testResult("MICROSD", true);
    }
    else
    {
        testResult("MICROSD", false);
        failHandler();
    }

    // Check the WiFi
    Serial.println("- WiFi:");
    if (checkWiFi(WSSID, WPASS, WTIMEOUT))
    {
        testResult("WIFI", true);
    }
    else
    {
        testResult("WIFI", false);
        failHandler();
    }

    // Check the RTC
    Serial.println("- PCF85063 RTC:");
    if (rtcCheck())
    {
        testResult("RTC", true);
    }
    else
    {
        testResult("RTC", false);
        failHandler();
    }

    // Check I2C (Qwiic)
    Serial.println("- I2C (Qwiic):");
    if (checkI2C(qwiicDeviceAddress))
    {
        testResult("QWIIC", true);
    }
    else
    {
        testResult("QWIIC", false);
        failHandler();
    }

    // Check battery
    float batteryVoltage = 0.0f;
    Serial.println("- Battery:");
    if (checkBattery(&batteryVoltage))
    {
        Serial.print(batteryVoltage, 2);
        Serial.println("V ");
        testResult("BATTERY", true);
    }
    else
    {
        testResult("BATTERY", false);
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
            testResult("WAKEUP", false);
            failHandler();
        }

        if (digitalRead(GPIO_NUM_18) != wakeButtonState)
        {
            break;
        }
        delay(1);
    }
    Serial.println("WAKEUP button pressed!");
    testResult("WAKEUP", true);
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
    // Report the overall result of the factory test.
    Serial.println("INKPLATE:FAIL");

    Serial.println(" -> Test stopped!");
    while (true)
        delay(1000);
}

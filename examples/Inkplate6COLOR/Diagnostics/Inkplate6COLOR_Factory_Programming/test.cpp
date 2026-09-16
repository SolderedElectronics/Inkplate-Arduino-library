#include "test.h"

const char sdCardTestStringLength = 100;
const char *testString = {"This is some test string..."};

const char *WSSID = {"Soldered-testingPurposes"};
const char *WPASS = {"Testing443"};

// Change this to your used slave device
const uint8_t qwiicDeviceAddress = 0x30;

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
    // Set display for test report
    // Send test reports to the UART (this epaper is slow and does not support partial update)
    Serial.println("INKPLATE CHECKLIST");

    // Check I/O expander
    Serial.println("- I/O Expander:");
    // Try to communicate with I/O expander
    Wire.beginTransmission(IO_INT_ADDR);
    if (Wire.endTransmission() == 0) // Check if there was an error in communication and print out the results on display.
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

    // First version of the Inkplate doesn't have RTC.

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
    // A slave must be connected via Qwiic address (0x28)
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
    float batteryVoltage = 0.0;
    Serial.println("- Battery:");
    if (checkBattery(&batteryVoltage))
    {
        Serial.print(batteryVoltage);
        Serial.println("V ");
        testResult("BATTERY", true);
    }
    else
    {
        testResult("BATTERY", false);
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
            testResult("WAKEUP", false);
            failHandler();
        }

        if (digitalRead(GPIO_NUM_36) != wakeButtonState)
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
    Wire.begin();
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

    // reset and re-init RTC.
    display.rtc.reset();

    // Set some time in epoch in RTC.
    uint32_t _epoch = 1640995200ULL;
    display.rtc.setEpoch(_epoch);

    // Wait at least one and a half second
    delay(1500);

    // Read the epoch (if everything is ok, epoch from RTC should not be the same)
    if (display.rtc.getEpoch() != _epoch)
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
    // Report the overall result of the factory test.
    Serial.println("INKPLATE:FAIL");


    Serial.println(" -> Test stopped!");

    // Inf. loop... halt the program!
    while (true)
        delay(1000);
}
#include "test.h"
#include "Wire.h"

// Change this to your WiFi
const char *WSSID = {"Soldered-testingPurposes"};
const char *WPASS = {"Testing443"};

// Change this to your used slave device
const uint8_t qwiicDeviceAddress = 0x30;

// Test all peripherals
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
    Serial.println("INKPLATE TEST CHECKLIST:");

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

    // Check I2C (Qwiic)
    // A slave must be connected via Qwiic address set in this file
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
}

// Test I2C
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

int checkWiFi(const char *_ssid, const char *_pass, uint8_t _wifiTimeout)
{
    unsigned long _timeout = millis();

    // Try to connect to WiFi network
    WiFi.begin(WSSID, WPASS);

    // Wait until WiFi connection is established or timeout has occured
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

    // Something is wrong if you got there
    return 0;
}

// Show a message and stop the code from executing
void failHandler()
{
    // Report the overall result of the factory test.
    Serial.println("INKPLATE:FAIL");

    Serial.println(" -> Test stopped!");

    // Inf. loop... halt the program!
    while (true)
        delay(1000);
}

#include "test.h"
#include "Wire.h"

// Change this to your WiFi
const char *WSSID = {"Soldered-testingPurposes"};
const char *WPASS = {"Testing443"};

// Change this to your used slave device
const uint8_t easyCDeviceAddress = 0x30;

// Test all peripherals
void testPeripheral()
{
    Serial.println("INKPLATE TEST CHECKLIST:");

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

    // Check I2C (easyc)
    // A slave must be connected via easyC address set in this file
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

    Serial.println("Test OK");
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
    Serial.println(" -> Test stopped!");

    // Inf. loop... halt the program!
    while (true)
        delay(1000);
}

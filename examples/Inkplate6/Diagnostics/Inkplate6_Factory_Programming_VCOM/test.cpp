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
    display.setTextSize(3);
    display.setTextColor(BLACK);
    display.setCursor(50, 50);
    display.println("INKPLATE TEST CHECKLIST");

    //  Power up epaper PSU
    display.einkOn();

    // Check if epaper PSU (TPS65186 EPD PMIC) is ok.
    if (!checkI2C(0x48) || (display.readPowerGood() != PWR_GOOD_OK)) // Check if there was an error in communication
    {
        Serial.println("- TPS Fail!");
        failHandler();
    }
    display.println("- TPS65186: OK");
    display.partialUpdate(0, 1);

    // Check if the screen is showing the image properly. There should be a black rectangle around the display area and 4 small rectangles in every corner of the screen. If not, the display is not compatible with this library.
    checkScreenBorder();

    // Check I/O expander 1
    display.printf("- I/O Expander 1: ");
    display.partialUpdate(0, 1);

    // Try to communicate with I/O expander
    if (checkI2C(IO_INT_ADDR)) // Check if there was an error in communication and print out the results on display.
    {    
        display.println("OK");
        display.partialUpdate(0, 1);
    }
    else
    {
        display.println("FAIL");
        failHandler();
    }

    // Check I/O expander 2
    display.printf("- I/O Expander 2: ");
    display.partialUpdate(0, 1);

    // Try to communicate with I/O expander
    if (checkI2C(IO_EXT_ADDR)) // Check if there was an error in communication and print out the results on display.
    {
        display.println("OK");
        display.partialUpdate(0, 1);
    }
    else
    {
        display.println("FAIL");
        failHandler();
    }

    // Check the micro SD card slot
    display.print("- microSD card slot: ");
    display.partialUpdate(0, 1);
    if (checkMicroSDCard())
    {
        display.println("OK");
        display.partialUpdate(0, 1);
    }
    else
    {
        display.println("FAIL");
        failHandler();
    }

    // Check the WiFi
    display.print("- WiFi: ");
    display.partialUpdate(0, 1);
    if (checkWiFi(WSSID, WPASS, WTIMEOUT))
    {
        display.println("OK");
        display.partialUpdate(0, 1);
    }
    else
    {
        display.println("FAIL");
        failHandler();
    }

    // First version of the Inkplate doesn't have RTC.

    // Check the RTC
    display.print("- PCF85063 RTC: ");
    if (rtcCheck())
    {
        display.println("OK");
        display.partialUpdate(0, 1);
    }
    else
    {
        display.println("FAIL");
        failHandler();
    }


    // Check I2C (easyc)
    // A slave must be connected via easyC address set in this file
    display.print("- I2C (easyC): ");
    display.partialUpdate(0, 1);
    if (checkI2C(easyCDeviceAddress))
    {
        display.println("OK");
        display.partialUpdate(0, 1);
    }
    else
    {
        display.println("FAIL");
        failHandler();
    }

    float batteryVoltage = 0;
    float temperature = 0;
    // Check battery
    display.print("- Battery and temperature: ");
    display.partialUpdate(0, 1);
    if (checkBatteryAndTemp(&temperature, &batteryVoltage))
    {
        display.print(batteryVoltage);
        display.print("V, ");
        display.print(temperature);
        display.println("C OK");
        display.partialUpdate(0, 1);
    }
    else
    {
        display.println("FAIL");
        failHandler();
    }

    // Text wake up button
    long beginWakeUpTest = millis();
    int wakeButtonState = digitalRead(GPIO_NUM_36);
    
    display.print("Press WAKEUP button within 30 seconds to finish testing... ");
    display.partialUpdate(0, 1);

    while (true)
    {
        long now = millis();
        if (now - beginWakeUpTest > 30000)
        {
            display.println("WAKEUP not pressed within 30 seconds!");
            display.partialUpdate(0, 1);
            failHandler();
        }

        if (digitalRead(GPIO_NUM_36) != wakeButtonState)
        {
            break;
        }
        delay(1);
    }

    display.println("OK");
    display.partialUpdate(0, 1);


    // This test only must be run on older Inkplates (e-radionica.com Inkplates with touchpads)
#ifdef ARDUINO_ESP32_DEV
    // Test Touchpads
    if (touchPads(TOUCHPADS_TIMEOUT))
    {
        display.println(" OK");
        display.partialUpdate(0, 1);
    }
    else
    {
        display.println(" FAIL");
        failHandler();
    }
#endif
}

// Check if the screen is showing the image correctly. The image should not have any shift to the left or right.
// In the corners of the screen, there should be a small rectangle.
void checkScreenBorder()
{
    // Size of small rectangles in the corners of the screen (minimum is 3, maximum is 7)
    int _smallRectSize = 6;

    // Display an info message on the screen
    display.println("Check corners and border (visual check)");

    // Draw a black rectangle from edge to edge of the screen
    display.drawRect(0, 0, display.width(), display.height(), BLACK);

    // Now draw 4 small rectangles in all four corners of the screen
    display.drawRect(0, 0, _smallRectSize, _smallRectSize, BLACK);
    display.drawRect(display.width() - _smallRectSize, 0, _smallRectSize, _smallRectSize, BLACK);
    display.drawRect(0, display.height() - _smallRectSize, _smallRectSize, _smallRectSize, BLACK);
    display.drawRect(display.width() - _smallRectSize, display.height() - _smallRectSize, _smallRectSize, _smallRectSize, BLACK);

    // Send image to the screen
    display.partialUpdate(false, true);

    // Wait a little bit
    delay(5000);
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

int checkBatteryAndTemp(float *temp, float *batVoltage)
{
    int temperature;
    float voltage;
    int result = 1;

    temperature = display.readTemperature();
    voltage = display.readBattery();
    *temp = temperature;
    *batVoltage = voltage;

    // Check the temperature sensor of the TPS65186.
    // If the result is -10 or +85, something is wrong.
    if (temperature <= -10 || temperature >= 85)
    {
        result = 0;
    }

    // Check the battery voltage.
    // If the measured voltage is below 2.8V and above 4.6V, charger is dead.
    if (voltage <= 2.8 || voltage >= 4.6)
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

int touchPads(uint8_t _timeoutTouchpads)
{
    // This test only must be run on older Inkplates (e-radionica.com Inkplates with touchpads)
#ifdef ARDUINO_ESP32_DEV
    // Variable for storing detected touch
    int _flags = 0;

    // Show message on display
    display.print("- Touchpads [Press them!]: ");
    display.partialUpdate(0, 1);

    // Wait until all touchpads are pressed at least once in test.

    unsigned long _timeout = millis();
    while ((_flags != 0b00000111) && ((unsigned long)(millis() - _timeout) < (_timeoutTouchpads * 1000UL)))
    {
        for (int i = 0; i < 3; i++)
        {
            if (display.readTouchpad(PAD1 + i) && (!(_flags & (1 << i))))
            {
                _flags |= (1 << i);
                display.print(i + 1, DEC);
                display.partialUpdate(0, 1);
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
void failHandler(bool printErrorOnSerial)
{
    if (printErrorOnSerial)
    {
        Serial.println(" -> Test stopped!");
    }
    else
    {
        display.print(" -> Test stopped!");
        display.partialUpdate(0, 1);
    }


    // Inf. loop... halt the program!
    while (true)
        delay(1000);
}


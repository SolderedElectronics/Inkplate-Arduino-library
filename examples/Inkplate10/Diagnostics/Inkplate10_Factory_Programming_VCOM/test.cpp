#include "test.h"

const char sdCardTestStringLength = 100;
const char *testString = {"This is some test string..."};

<<<<<<< HEAD:examples/Inkplate10/Diagnostics/Inkplate10_Factory_Programming_VCOM/test.cpp
// Change this to your WiFi
const char *WSSID = {"Soldered-testingPurposes"};
const char *WPASS = {"Testing443"};

// Change this to your used slave device
const uint8_t easyCDeviceAddress = 0x30;

// Test all peripherals
void testPeripheral()
{
    // Set display for test report
    display.setTextSize(3);
    display.setTextColor(BLACK);
    display.setCursor(50, 50);
    display.println("INKPLATE TEST CHECKLIST");
=======
const char *WSSID = {""};
const char *WPASS = {""};

void testPeripheral(uint8_t _skipSecondIO)
{
    // Set display for test report
    display.setTextSize(4);
    display.setTextColor(BLACK);
    display.setCursor(0, 0);
    display.println("INKPLATE CHECKLIST");
>>>>>>> 93efc99f1d39c8fc1fa726ac84022b59e384def1:examples/Inkplate10/Others/Inkplate_Factory_Programming_VCOM/test.cpp

    //  Power up epaper PSU
    display.einkOn();

    // Check if epaper PSU (TPS65186 EPD PMIC) is ok.
<<<<<<< HEAD:examples/Inkplate10/Diagnostics/Inkplate10_Factory_Programming_VCOM/test.cpp
    if (!checkI2C(0x48) || (display.readPowerGood() != PWR_GOOD_OK)) // Check if there was an error in communication
=======
    Wire.beginTransmission(0x48); // Send address 0x48 on I2C
    if (!(Wire.endTransmission() == 0) || !(display.readPowerGood())) // Check if there was an error in communication
>>>>>>> 93efc99f1d39c8fc1fa726ac84022b59e384def1:examples/Inkplate10/Others/Inkplate_Factory_Programming_VCOM/test.cpp
    {
        Serial.println("- TPS Fail!");
        failHandler();
    }
    display.println("- TPS65186: OK");
    display.partialUpdate(0, 1);

<<<<<<< HEAD:examples/Inkplate10/Diagnostics/Inkplate10_Factory_Programming_VCOM/test.cpp
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

=======
    // IF needed, check of the second I/O expander can be skipped.
    int _n = _skipSecondIO? 1 : 2;
    for (int i = 0; i < _n; i++)
    {
        // Check I/O expander
        display.printf("- I/O Expander %d: ", i + 1);
        display.partialUpdate(0, 1);
    
        // Try to communicate with I/O expander
        Wire.beginTransmission(0x20 + (i * 2)); // Send address 0x20 or 0x22
        if (Wire.endTransmission() == 0)        // Check if there was an error in communication and print out the results on display.
        {
            display.println("OK");
            display.partialUpdate(0, 1);
        }
        else
        {
            // Hang the code only if first MCP is not found.
            display.println("FAIL");
            failHandler();
        }
    }
>>>>>>> 93efc99f1d39c8fc1fa726ac84022b59e384def1:examples/Inkplate10/Others/Inkplate_Factory_Programming_VCOM/test.cpp

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

<<<<<<< HEAD:examples/Inkplate10/Diagnostics/Inkplate10_Factory_Programming_VCOM/test.cpp
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

    Serial.print("Press WAKEUP button within 30 seconds to finish testing... ");
    display.print("Press WAKEUP button within 30 seconds to finish testing... ");
    display.partialUpdate(0, 1);

    while (true)
    {
        long now = millis();
        if (now - beginWakeUpTest > 30000)
        {
            display.println("WAKEUP not pressed within 30 seconds!");
            Serial.println("WAKEUP not pressed within 30 seconds!");
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
    Serial.println("OK");
    display.partialUpdate(0, 1);


    // This test only must be run on older Inkplates (e-radionica.com Inkplates with touchpads)
#ifdef ARDUINO_ESP32_DEV
=======
    // This test only must be run on older Inkplates (e-radionica.com Inkplates with touchpads)
#ifdef ARDUINO_INKPLATE10 
>>>>>>> 93efc99f1d39c8fc1fa726ac84022b59e384def1:examples/Inkplate10/Others/Inkplate_Factory_Programming_VCOM/test.cpp
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

<<<<<<< HEAD:examples/Inkplate10/Diagnostics/Inkplate10_Factory_Programming_VCOM/test.cpp
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
=======
double getVCOMFromSerial(double *_vcom)
{
    double vcom = 1;
    char serialBuffer[50];
    unsigned long serialTimeout;

    // Display a message on Inkplate
    display.print("\r\n- Write VCOM on UART: ");
    display.partialUpdate(0, 1);

    while (true)
    {
        Serial.println("Write VCOM voltage from epaper panel.\r\nDon't forget negative (-) sign!\r\nUse dot as the decimal point. For example -1.23\n");
        while (!Serial.available());

        serialTimeout = millis();
        int i = 0;
        while ((Serial.available()) && ((unsigned long)(millis() - serialTimeout) < 500))
        {
            if ((Serial.available()) && (i < 49))
            {
                serialBuffer[i++] = Serial.read();
                serialTimeout = millis();
            }
        }
        serialBuffer[i] = 0;
        if (sscanf(serialBuffer, "%lf", &vcom) == 1)
        {
            *_vcom = vcom;
            return 1;
        }
    }

    return 0;
>>>>>>> 93efc99f1d39c8fc1fa726ac84022b59e384def1:examples/Inkplate10/Others/Inkplate_Factory_Programming_VCOM/test.cpp
}

int checkWiFi(const char *_ssid, const char *_pass, uint8_t _wifiTimeout)
{
    unsigned long _timeout = millis();

    // Try to connect to WiFi network
    WiFi.begin(WSSID, WPASS);

    // Wait until WiFi connection is established or timeout has occured.
<<<<<<< HEAD:examples/Inkplate10/Diagnostics/Inkplate10_Factory_Programming_VCOM/test.cpp
    while ((WiFi.status() != WL_CONNECTED) && ((unsigned long)(millis() - _timeout) < (_wifiTimeout * 1000UL)))
        ;

    // Check if board is connected to WiFi
=======
    while ((WiFi.status() != WL_CONNECTED) && ((unsigned long)(millis() - _timeout) < (_wifiTimeout * 1000UL)));

     // Check if board is connected to WiFi
>>>>>>> 93efc99f1d39c8fc1fa726ac84022b59e384def1:examples/Inkplate10/Others/Inkplate_Factory_Programming_VCOM/test.cpp
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

<<<<<<< HEAD:examples/Inkplate10/Diagnostics/Inkplate10_Factory_Programming_VCOM/test.cpp
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

=======
>>>>>>> 93efc99f1d39c8fc1fa726ac84022b59e384def1:examples/Inkplate10/Others/Inkplate_Factory_Programming_VCOM/test.cpp
int rtcCheck()
{
    // First "ping" RTC on the I2C protocol and reset the RTC
    Wire.beginTransmission(0x51);
    int _res = Wire.endTransmission();

    // If result is from I2C is anything else than success (_res = 0), return 0 (error).
<<<<<<< HEAD:examples/Inkplate10/Diagnostics/Inkplate10_Factory_Programming_VCOM/test.cpp
    if (_res != 0)
        return 0;
=======
    if (_res != 0) return 0;
>>>>>>> 93efc99f1d39c8fc1fa726ac84022b59e384def1:examples/Inkplate10/Others/Inkplate_Factory_Programming_VCOM/test.cpp

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
<<<<<<< HEAD:examples/Inkplate10/Diagnostics/Inkplate10_Factory_Programming_VCOM/test.cpp
#ifdef ARDUINO_ESP32_DEV
=======
#ifdef ARDUINO_INKPLATE10 
>>>>>>> 93efc99f1d39c8fc1fa726ac84022b59e384def1:examples/Inkplate10/Others/Inkplate_Factory_Programming_VCOM/test.cpp
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
<<<<<<< HEAD:examples/Inkplate10/Diagnostics/Inkplate10_Factory_Programming_VCOM/test.cpp
                _flags |= (1 << i);
                display.print(i + 1, DEC);
                display.partialUpdate(0, 1);
=======
              _flags |= (1 << i);
              display.print(i + 1, DEC);
              display.partialUpdate(0, 1);
>>>>>>> 93efc99f1d39c8fc1fa726ac84022b59e384def1:examples/Inkplate10/Others/Inkplate_Factory_Programming_VCOM/test.cpp
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
<<<<<<< HEAD:examples/Inkplate10/Diagnostics/Inkplate10_Factory_Programming_VCOM/test.cpp
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
=======
void failHandler()
{
    display.print(" -> Test stopped!");
    display.partialUpdate(0, 1);

    // Inf. loop... halt the program!
    while (true);
>>>>>>> 93efc99f1d39c8fc1fa726ac84022b59e384def1:examples/Inkplate10/Others/Inkplate_Factory_Programming_VCOM/test.cpp
}

#include "test.h"

const char sdCardTestStringLength = 100;
const char *testString = {"This is some test string..."};

const char *WSSID = {"Soldered-testingPurposes"};
const char *WPASS = {"Testing443"};

// Change this to your used slave device
const uint8_t qwiicDeviceAddress = 0x30;

const int TOUCHSCREEN_TIMEOUT = 30;
const int GESTURE_TIMEOUT = 30;

// If you're testing with a battery:
// Change this to the battery you're using to test the device
int batteryCapacity = 600;

// Short way to add a margin to move the print from the left edge
#define ADD_PRINT_MARGIN display.print("      ");

// The flag which is set when the APDS interrupt is tested
volatile bool apdsIntFlag = false;

// The ISR which tests the interrupt from the gesture sensor
void IRAM_ATTR ioExpanderISR()
{
    apdsIntFlag = true;
}

// Helper function to clear the gesture buffer for testing the APDS gesture sensor
bool checkGesture(uint8_t *_gesturePtr)
{
    // Check if there is any gesture detected at all.
    if (display.apds9960.isGestureAvailable())
    {
        // If it is, read it.
        // NOTE: If the gesture is detected, but it's not read, sensor won't fire any interrupt
        // until the gesture is read from the register.
        uint8_t _gesture = display.apds9960.readGesture();

        // If the pointer for detected gesture is not NULL, copy the result.
        if (_gesturePtr != NULL)
            (*_gesturePtr) = _gesture;

        // Return true for successfully detected gesture.
        return true;
    }

    // Otherwise return false.
    return false;
}

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
    display.setTextSize(2);
    display.setTextColor(BLACK);
    display.setCursor(30, 30);
    display.println("INKPLATE CHECKLIST");

    //  Power up epaper PSU
    display.einkOn();

    // Check if epaper PSU (TPS65186 EPD PMIC) is ok.
    Wire.beginTransmission(0x48); // Send address 0x48 on I2C
    if (!(Wire.endTransmission() == 0) || !display.isPowerGood()) // Check if there was an error in communication
    {
        // Notify over Serial because the TPS doesn't work
        Serial.println("- TPS Fail!");
        testResult("TPS65186", false);
        failHandler();
    }
    // For adding a margin to the text, blank spaces are printed in the same line before any text
    ADD_PRINT_MARGIN
    display.println("- TPS65186: PASS");
    testResult("TPS65186", true);
    display.partialUpdate(0, 1);

    // Check I/O expander internal
    ADD_PRINT_MARGIN
    display.printf("- I/O Expander Internal:");
    display.partialUpdate(0, 1);

    // Try to communicate with I/O expander
    Wire.beginTransmission(IO_INT_ADDR);
    if (Wire.endTransmission() ==
        0) // Check if there was an error in communication and print out the results on display.
    {
        display.println("PASS");
        testResult("IO_EXP_INT", true);
        display.partialUpdate(0, 1);
    }
    else
    {
        display.println("FAIL");
        testResult("IO_EXP_INT", false);
        failHandler();
    }

    // Try to communicate with external I/O expander
    ADD_PRINT_MARGIN
    display.printf("- I/O Expander External:");
    display.partialUpdate(0, 1);

    // Try to communicate with I/O expander
    Wire.beginTransmission(IO_EXT_ADDR);
    // Check if there was an error in communication and print out the results on display.
    if (Wire.endTransmission() == 0)
    {
        display.println("PASS");
        testResult("IO_EXP_EXT", true);
        display.partialUpdate(0, 1);
    }
    else
    {
        display.println("FAIL");
        testResult("IO_EXP_EXT", false);
        failHandler();
    }

    // Check touch screen and frontlight
    // Check frontlight (just a visual check). Set frontlight to max.
    display.frontlight.setState(true);  // Enable frontlight circuit
    display.frontlight.setBrightness(63); // Set frontlight intensity to the max.
    ADD_PRINT_MARGIN
    display.println("- Frontlight test (visual check)");
    display.partialUpdate(0, 1);
    delay(1000);
    if (!askOperator("FRONTLIGHT", "Is the frontlight turned on?"))
    {
        failHandler();
    }

    // Check the touchscreen (init and touch)
    ADD_PRINT_MARGIN
    display.print("- Touchscreen init: ");
    display.partialUpdate(0, 1);
    if (checkTouch(TOUCHSCREEN_TIMEOUT))
    {
        display.println("PASS");
        testResult("TOUCHSCREEN", true);
        display.partialUpdate(0, 1);
    }
    else
    {
        display.println("FAIL");
        testResult("TOUCHSCREEN", false);
        failHandler();
    }

    // Check the micro SD card
    ADD_PRINT_MARGIN
    display.print("- microSD card slot: ");
    display.partialUpdate(0, 1);
    if (checkMicroSDCard())
    {
        display.println("PASS");
        testResult("MICROSD", true);
        display.partialUpdate(0, 1);
    }
    else
    {
        display.println("FAIL");
        testResult("MICROSD", false);
        failHandler();
    }
    // Great, put it back to sleep to save power
    display.sdCardSleep();

    // Check the WiFi
    ADD_PRINT_MARGIN
    display.print("- WiFi: ");
    display.partialUpdate(0, 1);
    if (checkWiFi(WSSID, WPASS, WTIMEOUT))
    {
        display.println("PASS");
        testResult("WIFI", true);
        display.partialUpdate(0, 1);
    }
    else
    {
        display.println("FAIL");
        testResult("WIFI", false);
        failHandler();
    }

    // Check the RTC
    ADD_PRINT_MARGIN
    display.print("- PCF85063 RTC: ");
    if (rtcCheck())
    {
        display.println("PASS");
        testResult("RTC", true);
        display.partialUpdate(0, 1);
    }
    else
    {
        display.println("FAIL");
        testResult("RTC", false);
        failHandler();
    }

    // Check I2C (Qwiic)
    // A slave must be connected via Qwiic address (0x30)
    ADD_PRINT_MARGIN
    display.print("- I2C (Qwiic): ");
    display.partialUpdate(0, 1);
    if (checkI2C(qwiicDeviceAddress))
    {
        display.println("PASS");
        testResult("QWIIC", true);
        display.partialUpdate(0, 1);
    }
    else
    {
        display.println("FAIL");
        testResult("QWIIC", false);
        failHandler();
    }

    // Check Temperature via TPS
    float temperature = 0;
    ADD_PRINT_MARGIN
    display.print("- Temperature: ");
    display.partialUpdate(0, 1);
    if (checkTemp(&temperature))
    {
        display.println("PASS");
        testResult("TEMPERATURE", true);
        ADD_PRINT_MARGIN
        ADD_PRINT_MARGIN
        display.print("- Temperature: ");
        display.print(temperature);
        display.println("c");
        display.partialUpdate(0, 1);
    }
    else
    {
        ADD_PRINT_MARGIN
        display.println("FAIL");
        testResult("TEMPERATURE", false);
        failHandler();
    }

    // Check BME sensor
    float bmeTemp, bmeHumidity, bmePres;
    ADD_PRINT_MARGIN
    display.print("- Check BME680 Sensor: ");
    display.partialUpdate(0, 1);
    if (checkBME(&bmeTemp, &bmeHumidity, &bmePres))
    {
        display.println("PASS");
        testResult("BME688", true);
        ADD_PRINT_MARGIN
        ADD_PRINT_MARGIN
        display.print("- Temperature: ");
        display.print(bmeTemp);
        display.println(" C");
        ADD_PRINT_MARGIN
        ADD_PRINT_MARGIN
        display.print("- Humidity: ");
        display.print(bmeHumidity);
        display.println(" %");
        ADD_PRINT_MARGIN
        ADD_PRINT_MARGIN
        display.print("- Pressure: ");
        display.print(bmePres);
        display.println(" hPa");
    }
    else
    {
        ADD_PRINT_MARGIN
        display.println("FAIL");
        testResult("BME688", false);
        failHandler();
    }
    // Put it back to sleep to save power
    display.sleepPeripheral(INKPLATE_BME688);

    // Check the fuel gauge
    int fuelGaugeSOC, fuelGaugeVolts;
    ADD_PRINT_MARGIN
    display.print("- Check Fuel Gauge: ");
    display.partialUpdate(0, 1);
    if (checkFuelGauge(&fuelGaugeSOC, &fuelGaugeVolts))
    {
        display.println("PASS");
        testResult("FUEL_GAUGE", true);
        ADD_PRINT_MARGIN
        ADD_PRINT_MARGIN
        display.print("- SOC: ");
        display.println(fuelGaugeSOC);
        ADD_PRINT_MARGIN
        ADD_PRINT_MARGIN
        display.print("- Voltage: ");
        display.print(fuelGaugeVolts);
        display.println(" mV");
    }
    else
    {
        ADD_PRINT_MARGIN
        display.println("FAIL");
        testResult("FUEL_GAUGE", false);
        failHandler();
    }
    // Put it back to sleep
    display.sleepPeripheral(INKPLATE_FUEL_GAUGE);


    // Test the gyroscope
    float gyroAccX, gyroAccY, gyroAccZ;
    ADD_PRINT_MARGIN
    display.print("- Check Gyroscope: ");
    display.partialUpdate(0, 1);
    if (checkGyroscope(&gyroAccX, &gyroAccY, &gyroAccZ))
    {
        display.println("PASS");
        testResult("GYROSCOPE", true);
        ADD_PRINT_MARGIN
        ADD_PRINT_MARGIN
        display.print("- AccX: ");
        display.println(gyroAccX);
        ADD_PRINT_MARGIN
        ADD_PRINT_MARGIN
        display.print("- AccY: ");
        display.println(gyroAccY);
        ADD_PRINT_MARGIN
        ADD_PRINT_MARGIN
        display.print("- AccZ: ");
        display.println(gyroAccZ);
    }

    else
    {
        ADD_PRINT_MARGIN
        display.println("FAIL");
        testResult("GYROSCOPE", false);
        failHandler();
    }
    // Put it back to sleep
    display.sleepPeripheral(INKPLATE_ACCELEROMETER);

    // Check the gesture sensor
    ADD_PRINT_MARGIN
    display.print("- Check gesture sensor (swipe, 30s): ");
    display.partialUpdate(0, 1);
    String gesture = " ";
    if (checkGestureSensor(GESTURE_TIMEOUT, &gesture))
    {
        display.println("PASS");
        testResult("GESTURE", true);
        ADD_PRINT_MARGIN
        ADD_PRINT_MARGIN
        display.print("- Gesture: ");
        display.println(gesture);
    }
    else
    {
        ADD_PRINT_MARGIN
        display.println("FAIL");
        testResult("GESTURE", false);
        failHandler();
    }
    // Put it back to sleep
    display.sleepPeripheral(INKPLATE_APDS9960);

    // Check the buzzer
    ADD_PRINT_MARGIN
    display.println("- Check buzzer (audible check): ");
    display.partialUpdate(0, 1);
    checkBuzzer();
    if (!askOperator("BUZZER", "Did the buzzer beep?"))
    {
        failHandler();
    }

    // Text wake up button
    long beginWakeUpTest = millis();
    int wakeButtonState = digitalRead(GPIO_NUM_36);
    ADD_PRINT_MARGIN
    display.println("Press WAKEUP to finish test (30s)!");
    display.partialUpdate(0, 1);

    // Wait for WAKEUP button press
    while (true)
    {
        long now = millis();
        if (now - beginWakeUpTest > 30000)
        {
            ADD_PRINT_MARGIN
            display.println("WAKEUP not pressed for 30 seconds!");
            display.partialUpdate(0, 1);
            testResult("WAKEUP", false);
            failHandler();
        }

        if (digitalRead(GPIO_NUM_36) != wakeButtonState)
        {
            break;
        }
        delay(1);
    }

    ADD_PRINT_MARGIN
    display.println("WAKEUP button pressed!");
    testResult("WAKEUP", true);
    display.partialUpdate(0, 1);
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

int checkTemp(float *temp)
{
    int temperature;
    float voltage;
    int result = 1;

    temperature = display.readTemperature();
    *temp = temperature;

    // Check the temperature sensor of the TPS65186.
    // If the result is -10 or +85, something is wrong.
    if (temperature <= -10 || temperature >= 85)
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

int checkTouch(uint8_t _tsTimeout)
{
    unsigned long _timeout;

    // First try to init touchscreen controller.
    if (!display.touchscreen.init(1))
    {
        return 0;
    }

    // Now wait for the touch
    display.println("OK");
    ADD_PRINT_MARGIN
    display.print("Touch the corner (30s): ");
    display.drawRect(400, 0, 200, 200, BLACK);
    display.partialUpdate(0, 1);
    _timeout = millis();

    // Wait 30 seconds to detect touch in specified area, otherwise return 0 (error).
    while (((unsigned long)(millis() - _timeout)) < (_tsTimeout * 1000UL))
    {
        if (display.touchscreen.available())
        {
            uint8_t n;
            uint16_t x[2], y[2];
            // See how many fingers are detected (max 2) and copy x and y position of each finger on touchscreen
            n = display.touchscreen.getData(x, y);

            if ((x[0] > 400) && (x[0] < 600) && (y[0] > 0) && (y[0] < 200))
                return 1;
        }
    }
    return 0;
}

int checkBME(float *temp, float *hum, float *pres)
{
    display.wakePeripheral(INKPLATE_BME688);
    // Init BME
    int beginResult = display.bme688.begin();

    // Get the sensor data
    float temperature = display.bme688.readTemperature();
    float humidity = display.bme688.readHumidity();
    float pressure = display.bme688.readPressure();

    // Save it via the pointers as well
    *temp = temperature;
    *hum = humidity;
    *pres = pressure;

    // Check if it's within the expected range
    if (temperature < -40 || temperature > 85)
        return 0;
    if (humidity < 0 || humidity > 100)
        return 0;
    if (pressure < 300 || pressure > 1500)
        return 0;

    return beginResult;
}

int checkFuelGauge(int *_soc, int *_volts)
{
    // Wake battery
    display.wakePeripheral(INKPLATE_FUEL_GAUGE);

    // Init fuel gauge
    int beginResult = display.battery.begin();

    // Set the battery's capacity
    display.battery.setCapacity(batteryCapacity);

    // Read values
    int soc = display.battery.soc();       // Read state-of-charge (%)
    int volts = display.battery.voltage(); // Read battery voltage (mV)
    // Current will be 0 if no battery is connected

    // Also save via pointers
    *_soc = soc;
    *_volts = volts;

    // Check if it's within the expected range
    if (soc < 0 || soc > 100)
        return 0;
    if (volts < 1000 || volts > 5500)
        return 0;

    return true;
}

void checkBuzzer()
{
    // Init buzzer
    display.buzzer.init();

    // Do two short low beeps then two short high beeps
    display.buzzer.beep(100, 750);
    delay(100);
    display.buzzer.beep(100, 750);
    delay(100);
    display.buzzer.beep(100, 2400);
    delay(100);
    display.buzzer.beep(100, 2400);
    delay(100);
}

int checkGestureSensor(int _gestTimeout, String *gesture)
{
    // Wake peripheral
    display.wakePeripheral(INKPLATE_APDS9960);

    // Init APDS and enable the gesture sensor
    if (!display.apds9960.init())
    {
        return false;
    }

    if (!display.apds9960.enableGestureSensor(true))
    {
        return false;
    }
    delay(250);

    display.apds9960.setGestureGain(0); // Also set gain to lower so it's less sensitive
    display.apds9960.setGestureIntEnable(true);
    // Finally, enable the light sensor

    // Set the interrupt chain from the APDS, to the GPIO expander to the ESP32
    display.expander1.pinMode(9, INPUT);
    display.expander1.setIntPin(9);
    attachInterrupt(digitalPinToInterrupt(34), ioExpanderISR, CHANGE);

    // Clear out any previous detected gestures.
    checkGesture(NULL);

    unsigned long _timeout;
    _timeout = millis();

    // Wait 30 seconds to detect gesture
    while (((unsigned long)(millis() - _timeout)) < (_gestTimeout * 1000UL))
    {
        // If the APDS interrupt was read
        if (apdsIntFlag)
        {
            if (!display.expander1.digitalRead(9, IO_INT_ADDR))
            {
                // Get the gesture
                if (display.apds9960.isGestureAvailable())
                {
                    uint8_t detectedGesture;
                    checkGesture(&detectedGesture);
                    switch (detectedGesture)
                    {
                        // Return the gesture value depending which gesture was made
                    case DIR_UP:
                        *gesture = "UP";
                        break;
                    case DIR_DOWN:
                        *gesture = "DOWN";
                        break;
                    case DIR_LEFT:
                        *gesture = "LEFT";
                        break;
                    case DIR_RIGHT:
                        *gesture = "RIGHT";
                        break;
                    case DIR_NEAR:
                        *gesture = "NEAR";
                        break;
                    case DIR_FAR:
                        *gesture = "FAR";
                        break;
                    default:
                        *gesture = "NONE";
                    }
                    return 1;
                }
            }
            apdsIntFlag = false;
        }
    }
    return 0;
}

int checkGyroscope(float *acX, float *acY, float *acZ)
{
    // Let's wake the peripheral
    display.wakePeripheral(INKPLATE_ACCELEROMETER);

    // This actually returns 0 on success
    int beginResult = display.lsm6ds3.begin();

    // Multiply by 9.81 to get m/s^2
    float accelX = display.lsm6ds3.readFloatAccelX() * 9.81;
    float accelY = display.lsm6ds3.readFloatAccelY() * 9.81;
    float accelZ = display.lsm6ds3.readFloatAccelZ() * 9.81;

    float gyroX = display.lsm6ds3.readFloatGyroX();
    float gyroY = display.lsm6ds3.readFloatGyroY();
    float gyroZ = display.lsm6ds3.readFloatGyroZ();

    // Let's calculate the total magnitude of the readings
    float magnitude = sqrt(accelX * accelX + accelY * accelY + accelZ * accelZ);

    //  This should be approx 9.81, allow for some margin of error
    if (magnitude <= 8.5 || magnitude > 10.8)
        return 0;

    // Also save accel values via pointer
    *acX = accelX;
    *acY = accelY;
    *acZ = accelZ;

    // On success of begin, 0 is returned
    // In our case, 1 is a success, so flip it
    if (beginResult == 0)
        return 1;
    else
        return 0;
}

// Show a message and stop the code from executing.
void failHandler(bool printErrorOnSerial)
{
    // Report the overall result of the factory test.
    Serial.println("INKPLATE:FAIL");

    if (printErrorOnSerial)
    {
        Serial.println(" -> Test stopped!");
    }
    else
    {
        ADD_PRINT_MARGIN
        display.print(" -> Test stopped!");
        display.partialUpdate(0, 1);
    }

    // Inf. loop... halt the program!
    while (true)
        delay(1000);
}
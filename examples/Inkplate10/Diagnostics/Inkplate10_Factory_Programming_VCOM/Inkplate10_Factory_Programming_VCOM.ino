/**
 **************************************************
 * @file        Inkplate10_Factory_Programming_VCOM.ino
 *
 * @brief       File for programming the Inkplate's VCOM
 *
 * @note        !WARNING! VCOM can only be set 100 times, so keep usage to a minimum.
 *              !WARNING! Use at your own risk.
 *
 *              Inkplate 10 does not support auto VCOM, it has to be set manually.
 *              The user will be prompted to enter VCOM via serial (baud 115200).
 *              VCOM ranges from 0V to -5.0.
 *
 *              Tests will also be done, to pass all tests:
 *              - Edit the WiFi information in test.cpp.
 *              - Connect a slave device via EasyC on address 0x30 (you may change this in test.cpp also).
 *                In the InkplateEasyCTester folder, you can find the code for uploading to Dasduino Core 
 *                or Dasduino ConnectPlus to convert Dasduino to an I2C slave device for testing an easyC connector
 *                if you don't have a device with address 0x30.
 *              - Insert a formatted microSD card (doesn't have to be empty)
 *              - Press wake button to finish testing
 *
 *License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the
 *LICENSE file included with this example. If you have any questions about
 *licensing, please visit https://soldered.com/contact/ Distributed as-is; no
 *warranty is given.
 *
 * @authors     Soldered
 ***************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_INKPLATE10) && !defined(ARDUINO_INKPLATE10V2)
#error                                                                                                                 \
    "Wrong board selection for this example, please select e-radionica Inkplate10 or Soldered Inkplate10 in the boards menu."
#endif

// Include needed libraries in the sketch
#include "EEPROM.h"
#include "Inkplate.h"
#include "Wire.h"

// Include our functions and image
#include "Peripheral.h"
#include "test.h"
#include "image.h"

// Create object on Inkplate library and set library to work in monochorme mode
Inkplate display(INKPLATE_1BIT);

// If you want to write new VCOM voltage and perform all tests change this number
const int EEPROMoffset = 16;
int EEPROMaddress = sizeof(waveformData) + EEPROMoffset;

// Peripheral mode variables and arrays
#define BUFFER_SIZE 1000
char commandBuffer[BUFFER_SIZE + 1];

double vcomVoltage;

// All waveforms for Inkplate 10 boards
uint8_t waveform1[8][9] = {{0, 0, 0, 0, 0, 0, 0, 1, 0}, {0, 0, 0, 2, 2, 2, 1, 1, 0}, {0, 0, 2, 1, 1, 2, 2, 1, 0},
                           {0, 1, 2, 2, 1, 2, 2, 1, 0}, {0, 0, 2, 1, 2, 2, 2, 1, 0}, {0, 2, 2, 2, 2, 2, 2, 1, 0},
                           {0, 0, 0, 0, 0, 2, 1, 2, 0}, {0, 0, 0, 2, 2, 2, 2, 2, 0}};
uint8_t waveform2[8][9] = {{0, 0, 0, 0, 0, 0, 0, 0, 0}, {0, 0, 0, 2, 1, 2, 1, 1, 0}, {0, 0, 0, 2, 2, 1, 2, 1, 0},
                           {0, 0, 2, 2, 1, 2, 2, 1, 0}, {0, 0, 0, 2, 1, 1, 1, 2, 0}, {0, 0, 2, 2, 2, 1, 1, 2, 0},
                           {0, 0, 0, 0, 0, 1, 2, 2, 0}, {0, 0, 0, 0, 2, 2, 2, 2, 0}};
uint8_t waveform3[8][9] = {{0, 3, 3, 3, 3, 3, 3, 3, 0}, {0, 1, 2, 1, 1, 2, 2, 1, 0}, {0, 2, 2, 2, 1, 2, 2, 1, 0},
                           {0, 0, 2, 2, 2, 2, 2, 1, 0}, {0, 3, 3, 2, 1, 1, 1, 2, 0}, {0, 3, 3, 2, 2, 1, 1, 2, 0},
                           {0, 2, 1, 2, 1, 2, 1, 2, 0}, {0, 3, 3, 3, 2, 2, 2, 2, 0}};
uint8_t waveform4[8][9] = {{0, 0, 0, 0, 0, 0, 0, 1, 0}, {0, 0, 0, 2, 2, 2, 1, 1, 0}, {0, 0, 2, 1, 1, 2, 2, 1, 0},
                           {1, 1, 2, 2, 1, 2, 2, 1, 0}, {0, 0, 2, 1, 2, 2, 2, 1, 0}, {0, 1, 2, 2, 2, 2, 2, 1, 0},
                           {0, 0, 0, 2, 2, 2, 1, 2, 0}, {0, 0, 0, 2, 2, 2, 2, 2, 0}};
uint8_t waveform5[8][9] = {{0, 0, 0, 0, 0, 0, 0, 1, 0}, {0, 0, 0, 2, 2, 2, 1, 1, 0}, {2, 2, 2, 1, 0, 2, 1, 0, 0},
                           {2, 1, 1, 2, 1, 1, 1, 2, 0}, {2, 2, 2, 1, 1, 1, 0, 2, 0}, {2, 2, 2, 1, 1, 2, 1, 2, 0},
                           {0, 0, 0, 0, 2, 1, 2, 2, 0}, {0, 0, 0, 0, 2, 2, 2, 2, 0}};
uint8_t *waveformList[] = {&waveform1[0][0], &waveform2[0][0], &waveform3[0][0], &waveform4[0][0], &waveform5[0][0]};

// Calculate number of possible waveforms
uint8_t waveformListSize = (sizeof(waveformList) / sizeof(uint8_t *));

// Struct for reading waveform from EEPROM memory of ESP32
struct waveformData waveformEEPROM;

// Waveform 1 is index 0
// Waveform 2 is index 1
// Waveform 3 is index 2
// Waveform 4 is index 3
// Waveform 5 is index 4
int selectedWaveform = 0;

void setup()
{
    Serial.begin(115200);
    pinMode(GPIO_NUM_36, INPUT); // Wakeup button
    EEPROM.begin(512);
    Wire.begin();

    // Check for the first run of this code. If it is first run, check the I2C bus.
    bool isFirstStartup = (EEPROM.read(EEPROMaddress) != 170);
    if (isFirstStartup)
    {
        // First, test I2C as all the peripherals are connected with it
        // A slave must be connected on the address set in test.cpp (0x30 by default) for the tests to pass
        // Will print results to serial

        // Try to ping first expander.
        Wire.setTimeOut(1000);
        Wire.beginTransmission(IO_INT_ADDR);
        int result = Wire.endTransmission();

        if (result == 5)
        {
            Serial.println("I2C Bus Error!");
            failHandler(true);
        }
    }

    // Init the Inkplate library (after the check of the I2C bus).
    display.begin();

    if (isFirstStartup)
    {
        // Test all the peripherals
        testPeripheral();

        do
        {
            // Get VCOM voltage from serial from user
            uint8_t flag = getVCOMFromSerial(&vcomVoltage);

            // Show the user the entered VCOM voltage
            Serial.print("Entered VCOM: ");
            Serial.println(vcomVoltage);
            display.print(vcomVoltage);
            display.partialUpdate();

            if (vcomVoltage < -5.0 || vcomVoltage > 0.0)
            {
                Serial.println("VCOM out of range!");
                display.print(" VCOM out of range!");
                display.partialUpdate();
            }

        } while (vcomVoltage < -5.0 || vcomVoltage > 0.0);

        // Write VCOM to EEPROM
        display.pinModeInternal(IO_INT_ADDR, display.ioRegsInt, 6, INPUT_PULLUP);
        writeVCOMToEEPROM(vcomVoltage);
        EEPROM.write(EEPROMaddress, 170);
        EEPROM.commit();

        display.selectDisplayMode(INKPLATE_3BIT);

        // Select the waveform via serial
        // Write "OK" to confirm input
        int result = 1;
        do
        {
            result = getWaveformFromSerial(&selectedWaveform);
        } while (result != 0);

        // Write waveform to EEPROM
        waveformEEPROM.waveformId = INKPLATE10_WAVEFORM1 + selectedWaveform;
        memcpy(&waveformEEPROM.waveform, waveformList[selectedWaveform], sizeof(waveformEEPROM.waveform));
        waveformEEPROM.checksum = display.calculateChecksum(waveformEEPROM);
        display.burnWaveformToEEPROM(waveformEEPROM);
    }
    else
    {
        display.einkOn();
        vcomVoltage = (double)(readReg(0x03) | ((uint16_t)(readReg(0x04 & 1) << 8))) / -100;
        display.getWaveformFromEEPROM(&waveformEEPROM) ? waveformEEPROM.waveformId : -1;
    }

    // Clean command buffer and show splash screen
    memset(commandBuffer, 0, BUFFER_SIZE);
    showSplashScreen(waveformEEPROM);
}

// Peripheral mode
void loop()
{
    // Peripheral mode 
    // More about peripheral mode: https://inkplate.readthedocs.io/en/latest/peripheral-mode.html
    
    if (Serial.available())
    {
        while (Serial.available())
        {
            for (int i = 0; i < (BUFFER_SIZE - 1); i++)
            {
                commandBuffer[i] = commandBuffer[i + 1];
            }
            commandBuffer[BUFFER_SIZE - 1] = Serial.read();
        }
    }

    // Function in Peripheral.h
    run(commandBuffer, BUFFER_SIZE, &display);
}

// Low level functions for reading and writing to registers
void writeReg(uint8_t _reg, uint8_t _data)
{
    Wire.beginTransmission(0x48);
    Wire.write(_reg);
    Wire.write(_data);
    Wire.endTransmission();
}

uint8_t readReg(uint8_t _reg)
{
    Wire.beginTransmission(0x48);
    Wire.write(_reg);
    Wire.endTransmission(false);
    Wire.requestFrom(0x48, 1);
    return Wire.read();
}

// Show splash screen with set VCOM and selected waveform
void showSplashScreen(struct waveformData _w)
{
    display.clearDisplay();
    display.display();
    display.selectDisplayMode(INKPLATE_3BIT);
    display.drawBitmap3Bit(0, 0, demo_image, demo_image_w, demo_image_h);
    display.setTextColor(0, 7);
    display.setTextSize(1);
    display.setCursor(19, 796);
    display.print(vcomVoltage, 2);
    display.print("V");
    display.setCursor(19, 808);
    display.print("Waveform");
    display.print(_w.waveformId - 20 + 1, DEC);
    display.display();
}

// Show Inkplate 10 Null waveform
void writeToScreen()
{
    display.clean(1, 8);
    display.clean(0, 2);
    display.clean(2, 10);
    // delay(10);
}

// Write VCOM
void writeVCOMToEEPROM(double v)
{
    int vcom = int(abs(v) * 100);
    int vcomH = (vcom >> 8) & 1;
    int vcomL = vcom & 0xFF;
    // First, we have to power up TPS65186
    // Pull TPS65186 WAKEUP pin to High
    display.digitalWriteInternal(IO_INT_ADDR, display.ioRegsInt, 3, HIGH);

    // Pull TPS65186 PWR pin to High
    display.digitalWriteInternal(IO_INT_ADDR, display.ioRegsInt, 4, HIGH);
    delay(10);

    // Send to TPS65186 first 8 bits of VCOM
    writeReg(0x03, vcomL);

    // Send new value of register to TPS
    writeReg(0x04, vcomH);
    delay(1);

    // Program VCOM value to EEPROM
    writeReg(0x04, vcomH | (1 << 6));

    // Wait until EEPROM has been programmed
    delay(1);
    do
    {
        delay(1);
    } while (display.digitalReadInternal(IO_INT_ADDR, display.ioRegsInt, 6));

    // Clear Interrupt flag by reading INT1 register
    readReg(0x07);

    // Now, power off whole TPS
    // Pull TPS65186 WAKEUP pin to Low
    display.digitalWriteInternal(IO_INT_ADDR, display.ioRegsInt, 3, LOW);

    // Pull TPS65186 PWR pin to Low
    display.digitalWriteInternal(IO_INT_ADDR, display.ioRegsInt, 4, LOW);

    // Wait a little bit...
    delay(1000);

    // Power up TPS again
    display.digitalWriteInternal(IO_INT_ADDR, display.ioRegsInt, 3, HIGH);

    delay(10);

    // Read VCOM valuse from registers
    vcomL = readReg(0x03);
    vcomH = readReg(0x04);
    Serial.print("Vcom: ");
    Serial.println(vcom);
    Serial.print("Vcom register: ");
    Serial.println(vcomL | (vcomH << 8));

    // Trun off the TPS65186 and wait a little bit
    display.einkOff();
    delay(100);

    if (vcom != (vcomL | (vcomH << 8)))
    {
        Serial.println("\nVCOM EEPROM PROGRAMMING FAILED!\n");
    }
    else
    {
        Serial.println("\nVCOM EEPROM PROGRAMMING OK\n");
    }
}

// Show gradient of waveform - for testing which one is best
void showGradient(int _selected)
{
    int w = display.width() / 8;
    int h = display.height() - 100;

    display.changeWaveform(waveformList[_selected]);

    display.fillRect(0, 725, 1200, 100, 7);

    display.setTextSize(3);
    display.setTextColor(0);
    display.setCursor(50, 740);
    display.print("Send '1', '2', '3', '4' or '5' via serial to select waveform.");
    display.setCursor(50, 780);
    display.print("Currently selected: ");
    display.print(_selected + 1); // Increment by 1 for printing
    display.print(", send 'OK' to confirm.");

    for (int i = 0; i < 8; i++)
    {
        display.fillRect(i * w, 0, w, h, i);
    }
    display.display();
}

// Prompt the user to enter '1', '2', '3' od '4' via serial to select the waveform
// Returns 0 when done
int getWaveformFromSerial(int *selected)
{
    char serialBuffer[50];
    unsigned long serialTimeout;

    showGradient(*selected);

    while (true)
    {
        Serial.println("Write '1', '2', '3', '4' or '5' to select waveform.\nWrite 'OK' to confirm.");
        while (!Serial.available())
            ;

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
        if (strstr(serialBuffer, "1") != NULL)
        {
            *selected = 0;
            return 1;
        }
        else if (strstr(serialBuffer, "2") != NULL)
        {
            *selected = 1;
            return 1;
        }
        if (strstr(serialBuffer, "3") != NULL)
        {
            *selected = 2;
            return 1;
        }
        if (strstr(serialBuffer, "4") != NULL)
        {
            *selected = 3;
            return 1;
        }
        if (strstr(serialBuffer, "5") != NULL)
        {
            *selected = 4;
            return 1;
        }
        else if (strstr(serialBuffer, "OK") != NULL || strstr(serialBuffer, "ok") != NULL ||
                 strstr(serialBuffer, "Ok") != NULL || strstr(serialBuffer, "oK") != NULL)
        {
            return 0;
        }
    }

    Serial.println("Input error!");
    return 0;
}

// Prompt user to enter VCOM
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
        Serial.println(
            "Write VCOM voltage from epaper panel.\r\nDon't forget negative (-) sign!\r\nUse dot as the decimal point. "
            "For example -1.23\n");
        while (!Serial.available())
            ;

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
}

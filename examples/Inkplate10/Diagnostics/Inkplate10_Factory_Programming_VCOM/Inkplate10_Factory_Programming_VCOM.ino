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
 *              After all tests have passed the device will showcase the Inkplate 10 onboarding image sequence.
 *              This consists of a sidebar and slides, some of which are compressed using RLE compression to save space.
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

// Include our test functions
#include "test.h"

// Include different images and fonts for the onboarding sequence
#include "fonts/Open_Sans_SemiBold_24.h"
#include "images/sidebar.h"
#include "images/slide_00.h"
#include "images/slide_01.h"
#include "images/slide_02.h"
#include "images/slide_03.h"
#include "images/slide_04.h"
#include "images/slide_05.h"
#include "images/slide_06.h"
#include "images/welcome.h"

// Create object on Inkplate library and set library to work in monochorme mode
// It starts off in monochrome but then changes between grayscale and monochrome mode during the onboarding sequence
Inkplate display(INKPLATE_1BIT);

// If you want to write new VCOM voltage and perform all tests change this number
const int EEPROMoffset = 0;
int EEPROMaddress = sizeof(waveformData) + EEPROMoffset;

// Peripheral mode variables and arrays
#define BUFFER_SIZE 1000
char commandBuffer[BUFFER_SIZE + 1];

// The entered voltage to use for VCOM
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

// Declare the functions that are used in this file to keep the compiler happy

// Writing and reading from e-Paper power supply chip
void writeReg(uint8_t _reg, uint8_t _data);
uint8_t readReg(uint8_t _reg);

// Show the welcome screen
void showSplashScreen(struct waveformData _w);

// Write the set VCOM as a pernament configuration value
void writeVCOMToEEPROM(double v);

// Show the selected waveform's gradient so that it can be determined which is best
void showGradient(int _selected);

// Read from the Serial input so that the waveform can be selected
int getWaveformFromSerial(int *selected);

// Read the VCOM from the Serial input
double getVCOMFromSerial(double *_vcom);

// For the onboarding sequence - draw the sidebar
void drawSidebar(int selectedItem, bool bw);

// Does the partial update animation for slide 3 in the onboarding sequence
void partialUpdateAnimation();

// Decompress the slide for the onboarding sequence
void decompressRLE(const uint8_t *input, size_t inputSize);

// The array to hold the frame data for frames which have been compressed
// They get uncompressed into this array
uint8_t *uncompressedFrame;


// Simple function to wait for button press
void waitForWakeButtonPress()
{
    while (digitalRead(GPIO_NUM_36) == HIGH)
    {
        delay(10);
    }
}

// Setup function, runs only once
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

        // Now let's get the VCOM
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

        // Show the splash screen at first startup:
        showSplashScreen(waveformEEPROM);
        // Now go to infinite loop. The onboarding sequence will start upon turning on next time!
        while (true)
            ;
    }
    else
    {
        display.einkOn();
        vcomVoltage = (double)(readReg(0x03) | ((uint16_t)(readReg(0x04 & 1) << 8))) / -100;
        display.getWaveformFromEEPROM(&waveformEEPROM) ? waveformEEPROM.waveformId : -1;
    }

    // It's time to start the onboarding sequence
    // Let's memory allocate for the slides
    uncompressedFrame = (uint8_t *)ps_malloc(393938);
    if (uncompressedFrame == nullptr)
    {
        // Something really went wrong!
        // Let's show it on the display as it has inited at this point
        display.clearDisplay();
        display.setCursor(50, 50);
        display.setTextSize(2);
        display.print("ERROR: Couldn't allocate memory for the onboarding sequence!");
        display.display();
        // Go to infinite loop...
        while (true)
            ;
    }
}

// Loop code runs repeteadly, in this case it's the onboarding sequence
void loop()
{
    // Let's show the slides!

    // First, certain slides have to be decompressed, so this is not done in a loop automatically
    // This slide is the 'welcome' slide
    display.setDisplayMode(INKPLATE_3BIT);
    display.clearDisplay();
    decompressRLE(slide_00_compressed, sizeof(slide_00_compressed));
    display.drawBitmap3Bit(245, 0, uncompressedFrame, 955, 825); // Draw the slide
    drawSidebar(0, false);                                       // Draw sidebar for first menu item, in grayscale mode
    display.display();                                           // Show it!
    waitForWakeButtonPress(); // Wait until the wake button is pressed to go to the next slide

    // Slide 01 - Black and white mode
    display.setDisplayMode(INKPLATE_1BIT); // Go to BW mode
    display.clearDisplay();
    display.drawBitmap(245, 0, slide_01_cropped, 955, 825, WHITE, BLACK); // Draw the BW bitmap
    drawSidebar(1, true); // Draw sidebar for element 1 in black and white
    display.display();
    waitForWakeButtonPress(); // Wait until the wake button is pressed to go to the next slide

    // Slide 02 - Grayscale mode
    // Go back to grayscale mode
    display.setDisplayMode(INKPLATE_3BIT);
    display.clearDisplay();
    // Same principle as slide 00
    decompressRLE(slide_02_compressed, sizeof(slide_02_compressed));
    display.drawBitmap3Bit(245, 0, uncompressedFrame, 955, 825);
    drawSidebar(2, false);
    display.display();
    waitForWakeButtonPress(); // Wait until the wake button is pressed to go to the next slide

    // Slide 03 - Partial update
    // Go back to BW mode
    display.setDisplayMode(INKPLATE_1BIT);
    display.clearDisplay();
    drawSidebar(3, true);
    display.drawRect(248, 2, 949, 820, BLACK);
    display.drawBitmap(282, 21, partial_update_text, partial_update_text_w, partial_update_text_h, BLACK);
    partialUpdateAnimation(); // This function will also wait for button press

    // Slide 04 - Connectivity
    // Go back to grayscale mode
    display.setDisplayMode(INKPLATE_3BIT);
    display.clearDisplay();
    // Same principle as slide 00
    decompressRLE(slide_04_compressed, sizeof(slide_04_compressed));
    display.drawBitmap3Bit(245, 0, uncompressedFrame, 955, 825);
    drawSidebar(4, false);
    display.display();
    waitForWakeButtonPress(); // Wait until the wake button is pressed to go to the next slide

    // Slide 05 - Peripherals
    display.clearDisplay();
    // Same principle as slide 00
    decompressRLE(slide_05_compressed, sizeof(slide_05_compressed));
    display.drawBitmap3Bit(245, 0, uncompressedFrame, 955, 825);
    drawSidebar(5, false);
    display.display();
    waitForWakeButtonPress(); // Wait until the wake button is pressed to go to the next slide

    // Slide 06 - Uploading code
    display.clearDisplay();
    // Same principle as slide 00
    decompressRLE(slide_06_compressed, sizeof(slide_06_compressed));
    display.drawBitmap3Bit(245, 0, uncompressedFrame, 955, 825);
    drawSidebar(6, false);
    display.display();
    waitForWakeButtonPress(); // Wait until the wake button is pressed to go to the next slide
}

// The rest are helper functions used throughout this code

/**
 * @brief Write to a register of the TPS e-Paper power supply chip
 *
 * @param _reg The selected register
 * @param _data The data to write
 */
void writeReg(uint8_t _reg, uint8_t _data)
{
    Wire.beginTransmission(0x48);
    Wire.write(_reg);
    Wire.write(_data);
    Wire.endTransmission();
}

/**
 * @brief Read a register of the TPS e-Paper power supply chip
 *
 * @param _reg The selected register to read
 * @return uint8_t The data stored in the register
 */
uint8_t readReg(uint8_t _reg)
{
    Wire.beginTransmission(0x48);
    Wire.write(_reg);
    Wire.endTransmission(false);
    Wire.requestFrom(0x48, 1);
    return Wire.read();
}

/**
 * @brief Show the welcome screen with written VCOM and selected waveform
 *
 * @param _w the waveform data
 */
void showSplashScreen(struct waveformData _w)
{
    display.clearDisplay();
    display.display();
    display.selectDisplayMode(INKPLATE_3BIT);
    display.drawBitmap3Bit(0, 0, welcome, E_INK_WIDTH, E_INK_HEIGHT);
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

/**
 * @brief Write VCOM in EEPROM memory
 *
 * @note Keep usage of this to a minimum due to limited ESP32 EEPROM writes!
 *
 * @param v VCOM value to write
 */
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

/**
 * @brief Show the selected waveform's gradient - used while selecting the waveform
 *
 * @param _selected the index of the selected waveform
 */
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

/**
 * @brief Get the Waveform index from Serial communication
 *
 * @param selected the index of the selected waveform
 * @return int 1 if it was successful, 0 if it failed
 */
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

/**
 * @brief Read the VCOM input from Serial at 115200 baud
 *
 * @param _vcom pointer to where to save VCOM
 * @return double
 */
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

/**
 * @brief For the onboarding sequence - draw the sidebar
 *
 * @note The screen is split like this to save memory - the sidebar is mostly the same
 *
 * @param selectedItem The index of the selected menu item in the sidebar
 * @param bw To draw in 1-bit mode or not, depends on the slide
 */
void drawSidebar(int selectedItem, bool bw)
{
    // Make sure value is not out of bounds
    if (selectedItem < 0)
        selectedItem = 0;
    if (selectedItem >= 6)
        selectedItem = 6;

    // If not black and white, draw it in grayscale
    if (!bw)
    {
        // Draw the part of the image which is always the same
        display.drawBitmap3Bit(0, 0, sidebar, sidebar_w, sidebar_h);
    }
    else
    // If it's BW, draw it in bw
    {
        display.drawBitmap(0, 0, bw_sidebar, bw_sidebar_w, bw_sidebar_h, WHITE, BLACK);
    }

    // Set the font for this part of the process
    display.setFont(&Open_Sans_SemiBold_24);
    // Set the text size which is constant
    display.setTextSize(1);

    // Go through the menu items...
    for (int i = 0; i <= 6; i++)
    {
        int yOffset = 53 * i;
        // We're drawing the item currently selected with this code
        if (i == selectedItem)
        {
            if (bw)
            {
                // For BW mode, draw like this:
                display.fillRoundRect(3, 61 + yOffset, 239, 50, 5, WHITE);
                display.fillTriangle(0, 67 + yOffset, 11, 84 + yOffset, 0, 100 + yOffset, BLACK);
                display.setCursor(sidebarXOffsets[i], 96 + yOffset);
                display.setTextColor(BLACK);
                display.print(sidebarMenuTitles[i]);
            }
            else
            {
                // For grayscale mode, draw like this:
                display.fillRoundRect(3, 61 + yOffset, 239, 50, 5, 6);
                display.drawRoundRect(3, 61 + yOffset, 239, 50, 5, 3);
                display.fillTriangle(0, 67 + yOffset, 11, 84 + yOffset, 0, 100 + yOffset, 1);
                display.setCursor(sidebarXOffsets[i], 96 + yOffset);
                display.setTextColor(2);
                display.print(sidebarMenuTitles[i]);
            }
        }
        // This is code for the rest of the items
        else
        {
            if (bw)
            {
                // For BW mode, draw like this:
                display.fillRoundRect(3, 61 + yOffset, 239, 50, 5, BLACK);
                display.setCursor(sidebarXOffsets[i], 96 + yOffset);
                display.setTextColor(WHITE);
                display.print(sidebarMenuTitles[i]);
            }

            else
            {
                // For grayscale mode, draw like this:
                display.fillRoundRect(3, 61 + yOffset, 239, 50, 5, 1);
                display.drawRoundRect(3, 61 + yOffset, 239, 50, 5, 4);
                display.setCursor(sidebarXOffsets[i], 96 + yOffset);
                display.setTextColor(6);
                display.print(sidebarMenuTitles[i]);
            }
        }
    }
}

/**
 * @brief This function runs repeteadly during the partial update slide. Similar to a DVD logo animation.
 *
 */
void partialUpdateAnimation()
{
    // To count the number of updates
    int numUpdates = 0;
    int maxPartialUpdates = 20;

    // Some basic coordinations and dimensions of the animation
    int x = 272;
    int y = 190;
    int dx = 40; // Change in x direction
    int dy = 40; // Change in y direction
    int width = 840;
    int height = 560;

    // The logo which gets bounced around is also in slide_03.h
    int logoWidth = soldered_s_w;
    int logoHeight = soldered_s_h;

    // Manually draw the rectangle around the animation
    display.drawRect(271, 189, 880, 600, BLACK);
    display.fillRect(272, 190, 880 - 2, 600 - 2, WHITE);

    // Show everything
    display.display();

    while (true)
    {
        // Clear the previous position
        display.fillRect(272, 190, 880 - 2, 600 - 2, WHITE);

        // Update the position of the logo
        x += dx;
        y += dy;

        // Check for button press to exit the function
        // This is done multiple times during this while loop to increase responsiveness
        if (digitalRead(GPIO_NUM_36) == LOW)
            return;

        // Check for collisions with the edges and reverse direction if necessary
        if (x <= 272 || x + logoWidth >= 272 + width)
        {
            dx = -dx;
        }
        if (y <= 190 || y + logoHeight >= 190 + height)
        {
            dy = -dy;
        }

        if (digitalRead(GPIO_NUM_36) == LOW)
            return;

        // Draw the logo at the new position
        display.drawBitmap(x, y, soldered_s, logoWidth, logoHeight, BLACK);
        display.partialUpdate(false, true);
        numUpdates++;

        if (digitalRead(GPIO_NUM_36) == LOW)
            return;

        if (numUpdates >= maxPartialUpdates)
        {
            numUpdates = 0;
            display.display();
        }

        if (digitalRead(GPIO_NUM_36) == LOW)
            return;
    }
}

/**
 * @brief Decompress the compressed grayscale slide into the memory allocated array
 *
 * @note The data is compressed in this way, it goes by two bytes:
 *
 * Pixel value, how many pixels
 *
 * So, if there are 800 white pixels in a row, the compression algorithm (which is in compress.py)
 * will write it as:
 *
 * White, 800
 *
 * The decompression process does this in reverse to get the full image
 *
 * @param input
 * @param inputSize
 */
void decompressRLE(const uint8_t *input, size_t inputSize)
{
    size_t inIndex = 0;
    size_t outIndex = 0;

    while (inIndex < inputSize)
    {
        uint8_t value = input[inIndex++];
        uint8_t count = input[inIndex++];

        for (size_t i = 0; i < count; i++)
        {
            uncompressedFrame[outIndex++] = value;
        }
    }
}
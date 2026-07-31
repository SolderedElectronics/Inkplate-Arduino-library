/**
 **************************************************
 * @file        Inkplate10_Factory_Programming_VCOM.ino
 * @brief       File for programming the Inkplate's VCOM.
 *
 * @details     Inkplate 10 does not support auto VCOM, it has to be set
 *              manually. The user will be prompted to enter VCOM via serial
 *              (baud 115200). VCOM ranges from 0 to -5.0.
 *
 *              Tests will also be done. To pass all tests:
 *              - Edit the WiFi information in test.cpp.
 *              - Connect a slave device via EasyC on address 0x30 (you may
 *                change this in test.cpp also). In the InkplateEasyCTester
 *                folder, you can find the code for uploading to Dasduino Core
 *                or Dasduino ConnectPlus to convert Dasduino to an I2C slave
 *                device for testing an easyC connector if you don't have a
 *                device with address 0x30.
 *              - Insert a formatted microSD card (doesn't have to be empty).
 *              - Press wake button to finish testing.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 10
 * - Hardware:   Inkplate 10, USB cable, formatted microSD card,
 *               EasyC slave device on address 0x30
 * - Serial:     115200 baud
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate10"
 *    from Tools -> Board.
 * 2) Edit the WiFi information in test.cpp and connect the EasyC slave device.
 * 3) Insert a formatted microSD card and upload the sketch to Inkplate 10.
 * 4) Open Serial Monitor at 115200 baud and enter the VCOM value when prompted.
 * 5) Press the wake button to finish testing.
 *
 * @warning     VCOM can only be set 100 times, so keep usage to a minimum.
 * @warning     This example uses einkOn() and einkOff() methods that turn on
 *              power supply for the e-paper display. They should only be used
 *              in these examples, otherwise you risk damaging your e-paper
 *              display permanently!
 * @warning     Use at your own risk!
 * @warning     If it's a USB Power only Inkplate 10, set the #define in test.cpp.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/10/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 * @note        License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html
 *              Please review the LICENSE file included with this example. If you
 *              have any questions about licensing, please visit
 *              https://soldered.com/contact/ Distributed as-is; no warranty is
 *              given.
 *
 * @authors     Soldered
 ***************************************************/

#if !defined(ARDUINO_INKPLATE10) && !defined(ARDUINO_INKPLATE10V2)
#error                                                                                                                 \
    "Wrong board selection for this example, please select e-radionica Inkplate10 or Soldered Inkplate10 in the boards menu."
#endif

#include "Inkplate.h"
#include "Wire.h"
#include "EEPROM.h"

// Include our test functions
#include "test.h"

// Include different images and fonts for the onboarding sequence
#include "Fonts/Open_Sans_SemiBold_24.h"
#include "images/sidebar.h"
#include "images/slide_00.h"
#include "images/slide_01.h"
#include "images/slide_02.h"
#include "images/slide_03.h"
#include "images/slide_04.h"
#include "images/slide_05.h"
#include "images/slide_06.h"
#include "images/welcome.h"

// Create object on Inkplate library and set library to work in monochrome mode
Inkplate display(INKPLATE_1BIT);

// Provision marker and waveform number for factory flow.
// Addresses 0-75 are occupied by the waveformData struct written by setWaveform();
// the marker and waveform number must live beyond that range to avoid collisions.
int EEPROM_PROVISION_MARKER_ADDR = 76; // 170 => provisioned
int EEPROM_WAVEFORM_NUM_ADDR     = 77; // 1..5

// Peripheral mode variables and arrays (kept from original)
#define BUFFER_SIZE 1000
char commandBuffer[BUFFER_SIZE + 1];

// Entered voltage to use for VCOM
double vcomVoltage = 0.0;

// Waveform selection:
// - selectedWaveformIndex:  0..4 (for UI selection logic)
// - selectedWaveformNumber: 1..5 (for display.setWaveform())
int selectedWaveformIndex = 0;
uint8_t selectedWaveformNumber = 1;

// The array to hold the frame data for frames which have been compressed
uint8_t *uncompressedFrame = nullptr;

void showSplashScreen();
void showGradient(int _selectedIndex);
int  getWaveformFromSerial(int *selectedIndex);
double getVCOMFromSerial(double *vcomOut);

void drawSidebar(int selectedItem, bool bw);
void partialUpdateAnimation();
void decompressRLE(const uint8_t *input, size_t inputSize);

void waitForWakeButtonPress()
{
    while (digitalRead(GPIO_NUM_36) == HIGH)
    {
        delay(10);
    }
}

void setup()
{
    Serial.begin(115200);
    pinMode(GPIO_NUM_36, INPUT); // Wakeup button
    EEPROM.begin(512);
    Wire.begin();

    bool isFirstStartup = (EEPROM.read(EEPROM_PROVISION_MARKER_ADDR) != 170);

    if (isFirstStartup)
    {
        Wire.setTimeOut(1000);
        Wire.beginTransmission(IO_INT_ADDR);
        int result = Wire.endTransmission();

        if (result == 5)
        {
            Serial.println("I2C Bus Error!");
            failHandler(true);
        }
    }

    // Init Inkplate
    display.begin();

    if (isFirstStartup)
    {
        // Run peripheral tests
        testPeripheral();

        while (true)
        {
            getVCOMFromSerial(&vcomVoltage);

            Serial.print("Entered VCOM: ");
            Serial.println(vcomVoltage, 3);

            // Quick operator feedback on screen
            display.print(vcomVoltage);
            display.partialUpdate();

            // NEW API: setVCOM -> setVCOM
            if (display.setVCOM(vcomVoltage))
            {
                Serial.println("\nVCOM PROGRAMMING OK\n");
                break;
            }
            else
            {
                Serial.println("VCOM PROGRAMMING ERROR");
            }
        }

        display.selectDisplayMode(INKPLATE_3BIT);

        int result = 1;
        do
        {
            result = getWaveformFromSerial(&selectedWaveformIndex);
        } while (result != 0);

        // Convert 0..4 -> 1..5 for driver API
        selectedWaveformNumber = (uint8_t)(selectedWaveformIndex + 1);

        // NEW API: setWaveform(waveformNumber, burnToEEPROM)
        if (display.setWaveform(selectedWaveformNumber, true))
        {
            Serial.print("Waveform ");
            Serial.print(selectedWaveformNumber);
            Serial.println(" programmed OK (burned to EEPROM).");
        }
        else
        {
            Serial.println("Waveform programming FAILED.");
            // If this is a factory flow, you may want to stop here:
            // failHandler(true);
        }

        // Mark provisioned and store waveform number for UI purposes
        EEPROM.write(EEPROM_PROVISION_MARKER_ADDR, 170);
        EEPROM.write(EEPROM_WAVEFORM_NUM_ADDR, selectedWaveformNumber);
        EEPROM.commit();

        // Splash screen at first startup
        showSplashScreen();

        // Stop here; onboarding starts after power-cycle / reset (same behavior as original)
        while (true)
            ;
    }
    else
    {
        // Normal boot flow
        display.einkOn();

        vcomVoltage = display.getStoredVCOM();

        // Read waveform number chosen during provisioning (fallback to 1 if missing/invalid)
        uint8_t wf = (uint8_t)EEPROM.read(EEPROM_WAVEFORM_NUM_ADDR);
        if (wf < 1 || wf > 5)
            wf = 1;

        selectedWaveformNumber = wf;
        selectedWaveformIndex  = (int)wf - 1;

        // Make sure driver is using the chosen waveform without reburning EEPROM
        display.setWaveform(selectedWaveformNumber, false);
    }

    // Allocate PSRAM for slide decompression
    uncompressedFrame = (uint8_t *)ps_malloc(393938);
    if (uncompressedFrame == nullptr)
    {
        display.clearDisplay();
        display.setCursor(50, 50);
        display.setTextSize(2);
        display.print("ERROR: Couldn't allocate memory for onboarding!");
        display.display();
        while (true)
            ;
    }
}

// -------------------- Loop (onboarding sequence) --------------------
void loop()
{
    // Slide 00: splash screen
    showSplashScreen();
    waitForWakeButtonPress();

    // Slide 00 - welcome (grayscale, RLE)
    display.selectDisplayMode(INKPLATE_3BIT);
    display.clearDisplay();
    decompressRLE(slide_00_compressed, sizeof(slide_00_compressed));
    display.image.drawBitmap3Bit(245, 0, uncompressedFrame, 955, 825);
    drawSidebar(0, false);
    display.display();
    waitForWakeButtonPress();

    // Slide 01 - BW
    display.selectDisplayMode(INKPLATE_1BIT);
    display.clearDisplay();
    display.drawBitmap(245, 0, slide_01_cropped, 955, 825, BLACK);
    drawSidebar(1, true);
    display.display();
    waitForWakeButtonPress();

    // Slide 02 - Grayscale (RLE)
    display.selectDisplayMode(INKPLATE_3BIT);
    display.clearDisplay();
    decompressRLE(slide_02_compressed, sizeof(slide_02_compressed));
    display.image.drawBitmap3Bit(245, 0, uncompressedFrame, 955, 825);
    drawSidebar(2, false);
    display.display();
    waitForWakeButtonPress();

    // Slide 03 - Partial update demo (BW)
    display.selectDisplayMode(INKPLATE_1BIT);
    display.clearDisplay();
    drawSidebar(3, true);
    display.drawRect(248, 2, 949, 820, BLACK);
    display.image.drawBitmap3Bit(282, 21, partial_update_text, partial_update_text_w, partial_update_text_h);
    partialUpdateAnimation(); // waits for button press internally to exit

    // Slide 04 - Connectivity (RLE)
    display.selectDisplayMode(INKPLATE_3BIT);
    display.clearDisplay();
    decompressRLE(slide_04_compressed, sizeof(slide_04_compressed));
    display.image.drawBitmap3Bit(245, 0, uncompressedFrame, 955, 825);
    drawSidebar(4, false);
    display.display();
    waitForWakeButtonPress();

    // Slide 05 - Peripherals (RLE)
    display.clearDisplay();
    decompressRLE(slide_05_compressed, sizeof(slide_05_compressed));
    display.image.drawBitmap3Bit(245, 0, uncompressedFrame, 955, 825);
    drawSidebar(5, false);
    display.display();
    waitForWakeButtonPress();

    // Slide 06 - Uploading code (RLE)
    display.clearDisplay();
    decompressRLE(slide_06_compressed, sizeof(slide_06_compressed));
    display.image.drawBitmap3Bit(245, 0, uncompressedFrame, 955, 825);
    drawSidebar(6, false);
    display.display();
    waitForWakeButtonPress();
}

// -------------------- Helper functions --------------------

void showSplashScreen()
{
    display.clearDisplay();
    display.display();

    display.selectDisplayMode(INKPLATE_3BIT);
    display.image.drawBitmap3Bit(0, 0, welcome, E_INK_WIDTH, E_INK_HEIGHT);

    display.setTextColor(0, 7);
    display.setTextSize(1);

    display.setCursor(19, 796);
    display.print(vcomVoltage, 2);
    display.print("V");

    display.setCursor(19, 808);
    display.print("Waveform ");
    display.print(selectedWaveformNumber, DEC);

    display.display();
}

void showGradient(int _selectedIndex)
{
    int w = display.width() / 8;
    int h = display.height() - 100;

    display.fillRect(0, 725, 1200, 100, 7);

    display.setTextSize(3);
    display.setTextColor(0);
    display.setCursor(50, 740);
    display.print("Send '1', '2', '3', '4' or '5' via serial to select waveform.");
    display.setCursor(50, 780);
    display.print("Currently selected: ");
    display.print(_selectedIndex + 1);
    display.print(", send 'OK' to confirm.");

    for (int i = 0; i < 8; i++)
    {
        display.fillRect(i * w, 0, w, h, i);
    }
    display.display();
}

int getWaveformFromSerial(int *selectedIndex)
{
    char serialBuffer[50];
    unsigned long serialTimeout;

    // Ensure we're in grayscale while previewing gradients/UI text
    display.selectDisplayMode(INKPLATE_3BIT);
    showGradient(*selectedIndex);

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
                serialBuffer[i++] = (char)Serial.read();
                serialTimeout = millis();
            }
        }
        serialBuffer[i] = 0;

        if (strstr(serialBuffer, "1") != NULL)
        {
            *selectedIndex = 0;
            return 1;
        }
        else if (strstr(serialBuffer, "2") != NULL)
        {
            *selectedIndex = 1;
            return 1;
        }
        else if (strstr(serialBuffer, "3") != NULL)
        {
            *selectedIndex = 2;
            return 1;
        }
        else if (strstr(serialBuffer, "4") != NULL)
        {
            *selectedIndex = 3;
            return 1;
        }
        else if (strstr(serialBuffer, "5") != NULL)
        {
            *selectedIndex = 4;
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

double getVCOMFromSerial(double *vcomOut)
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
                serialBuffer[i++] = (char)Serial.read();
                serialTimeout = millis();
            }
        }
        serialBuffer[i] = 0;

        if (sscanf(serialBuffer, "%lf", &vcom) == 1)
        {
            *vcomOut = vcom;
            return 1;
        }
    }

    return 0;
}

void drawSidebar(int selectedItem, bool bw)
{
    if (selectedItem < 0)
        selectedItem = 0;
    if (selectedItem >= 6)
        selectedItem = 6;

    if (!bw)
    {
        display.image.drawBitmap3Bit(0, 0, sidebar, sidebar_w, sidebar_h);
    }
    else
    {
        display.drawBitmap(0, 0, bw_sidebar, bw_sidebar_w, bw_sidebar_h, BLACK);
    }

    display.setFont(&Open_Sans_SemiBold_24);
    display.setTextSize(1);

    for (int i = 0; i <= 6; i++)
    {
        int yOffset = 53 * i;

        if (i == selectedItem)
        {
            if (bw)
            {
                display.fillRoundRect(3, 61 + yOffset, 239, 50, 5, WHITE);
                display.fillTriangle(0, 67 + yOffset, 11, 84 + yOffset, 0, 100 + yOffset, BLACK);
                display.setCursor(sidebarXOffsets[i], 96 + yOffset);
                display.setTextColor(BLACK);
                display.print(sidebarMenuTitles[i]);
            }
            else
            {
                display.fillRoundRect(3, 61 + yOffset, 239, 50, 5, 6);
                display.drawRoundRect(3, 61 + yOffset, 239, 50, 5, 3);
                display.fillTriangle(0, 67 + yOffset, 11, 84 + yOffset, 0, 100 + yOffset, 1);
                display.setCursor(sidebarXOffsets[i], 96 + yOffset);
                display.setTextColor(2);
                display.print(sidebarMenuTitles[i]);
            }
        }
        else
        {
            if (bw)
            {
                display.fillRoundRect(3, 61 + yOffset, 239, 50, 5, BLACK);
                display.setCursor(sidebarXOffsets[i], 96 + yOffset);
                display.setTextColor(WHITE);
                display.print(sidebarMenuTitles[i]);
            }
            else
            {
                display.fillRoundRect(3, 61 + yOffset, 239, 50, 5, 1);
                display.drawRoundRect(3, 61 + yOffset, 239, 50, 5, 4);
                display.setCursor(sidebarXOffsets[i], 96 + yOffset);
                display.setTextColor(6);
                display.print(sidebarMenuTitles[i]);
            }
        }
    }
}

void partialUpdateAnimation()
{
    int numUpdates = 0;
    int maxPartialUpdates = 20;

    int x = 272;
    int y = 190;
    int dx = 40;
    int dy = 40;
    int width = 840;
    int height = 560;

    int logoWidth = soldered_s_w;
    int logoHeight = soldered_s_h;

    display.drawRect(271, 189, 880, 600, BLACK);
    display.fillRect(272, 190, 880 - 2, 600 - 2, WHITE);
    display.display();

    while (true)
    {
        display.fillRect(272, 190, 880 - 2, 600 - 2, WHITE);

        x += dx;
        y += dy;

        if (digitalRead(GPIO_NUM_36) == LOW)
            return;

        if (x <= 272 || x + logoWidth >= 272 + width)
            dx = -dx;

        if (y <= 190 || y + logoHeight >= 190 + height)
            dy = -dy;

        if (digitalRead(GPIO_NUM_36) == LOW)
            return;

        display.image.draw(soldered_s, x, y, logoWidth, logoHeight, BLACK);
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
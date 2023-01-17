/**
 **************************************************
 * @file        Inkplate_Factory_Programming_VCOM.ino
 *
 * @brief       File for programming the Inkplate's VCOM
 *
 * @note        !WARNING! VCOM can only be set 100 times, so keep usage to a minimum.
 *
 *              Inkplate 10 does not support auto VCOM, it has to be set manually.
 *              The user will be prompted to enter VCOM via serial (baud 115200).
 *              VCOM ranges from -1.0 to -2.0.
 *
 *              Tests will also be done, to pass all tests:
 *              -edit the WiFi information in test.cpp.
 *              -connect a slave device via EasyC on address 0x28 (you may change this in test.cpp also).
 *              -insert a formatted microSD card (doesn't have to be empty)
 *              -press wake button to finish testing
 *
 *License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the
 *LICENSE file included with this example. If you have any questions about
 *licensing, please contact techsupport@e-radionica.com Distributed as-is; no
 *warranty is given.
 *
 * @authors     Soldered
 ***************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_INKPLATE10) && !defined(ARDUINO_INKPLATE10V2)
#error                                                                                                                 \
    "Wrong board selection for this example, please select e-radionica Inkplate10 or Soldered Inkplate10 in the boards menu."
#endif

#include "EEPROM.h"
#include "Inkplate.h"
#include "image.h"
#include "test.h"
#include <Wire.h>

Inkplate display(INKPLATE_1BIT);

double vcomVoltage;
const int EEPROMOffset = 0;
int EEPROMaddress = sizeof(waveformData) + EEPROMOffset;

// Peripheral mode variables and arrays
#define BUFFER_SIZE 1000
char commandBuffer[BUFFER_SIZE + 1];
char strTemp[2001];

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
uint8_t *waveformList[] = {&waveform1[0][0], &waveform2[0][0], &waveform3[0][0]};

// Calculate number of possible waveforms
uint8_t waveformListSize = (sizeof(waveformList) / sizeof(uint8_t *));

// Struct for reading waveform from EEPROM memory of ESP32
struct waveformData waveformEEPROM;

// Waveform 1 is index 0
// Waveform 2 is index 1
// Waveform 3 is index 2
int selectedWaveform = 0;

void setup()
{
    Serial.begin(115200);
    display.begin();
    EEPROM.begin(512);

    // Wakeup button
    pinMode(GPIO_NUM_36, INPUT);

    // Uncomment if you want to write new VCOM voltage every time you run this sketch
    // WARNING: It can only be overwritten 100 times! Keep usage to a minimum

    //Serial.println("Resetting vcom voltage..");
    //delay(1000);
    //EEPROM.write(EEPROMaddress, 0);
    //EEPROM.commit();
    //Serial.println("It's been reset!");
    //delay(1000);

    // Setting default value for safety
    vcomVoltage = -1.3;

    if (EEPROM.read(EEPROMaddress) != 170)
    {
        Serial.println("VCOM not set, do tests");

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

            if (vcomVoltage < -2.0 || vcomVoltage > -1.0)
            {
                Serial.println("VCOM out of range!");
                display.print(" VCOM out of range!");
                display.partialUpdate();
            }

        } while (vcomVoltage < -2.0 || vcomVoltage > -1.0);

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
        Serial.println("VCOM and waveform already set!");
        display.einkOn();
        vcomVoltage = (double)(readReg(0x03) | ((uint16_t)(readReg(0x04 & 1) << 8))) / -100;
        display.getWaveformFromEEPROM(&waveformEEPROM) ? waveformEEPROM.waveformId : -1;
    }

    // Clean command buffer and show splash screen
    memset(commandBuffer, 0, BUFFER_SIZE);
    showSplashScreen(waveformEEPROM);
}

void loop()
{
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
    char *s = NULL;
    char *e = NULL;
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        if (commandBuffer[i] == '#' && s == NULL)
            s = &commandBuffer[i];
        if (commandBuffer[i] == '*' && e == NULL)
            e = &commandBuffer[i];
    }
    if (s != NULL && e != NULL)
    {
        if ((e - s) > 0)
        {
            int x, x1, x2, y, y1, y2, x3, y3, l, c, w, h, r, n;
            char b;
            // char temp[150];
            switch (*(s + 1))
            {
            case '?':
                Serial.print("OK");
                break;

            case '0':
                sscanf(s + 3, "%d,%d,%d", &x, &y, &c);
                // sprintf(temp, "display.drawPixel(%d, %d, %d)\n\r", x, y, c);
                // Serial.print(temp);
                display.drawPixel(x, y, c);
                break;

            case '1':
                sscanf(s + 3, "%d,%d,%d,%d,%d", &x1, &y1, &x2, &y2, &c);
                // sprintf(temp, "display.drawLine(%d, %d, %d, %d, %d)\n\r", x1, y1, x2, y2, c);
                // Serial.print(temp);
                display.drawLine(x1, y1, x2, y2, c);
                break;

            case '2':
                sscanf(s + 3, "%d,%d,%d,%d", &x, &y, &l, &c);
                // sprintf(temp, "display.drawFastVLine(%d, %d, %d, %d)\n\r", x, y, l, c);
                // Serial.print(temp);
                display.drawFastVLine(x, y, l, c);
                break;

            case '3':
                sscanf(s + 3, "%d,%d,%d,%d", &x, &y, &l, &c);
                // sprintf(temp, "display.drawFastHLine(%d, %d, %d, %d)\n\r", x, y, l, c);
                // Serial.print(temp);
                display.drawFastHLine(x, y, l, c);
                break;

            case '4':
                sscanf(s + 3, "%d,%d,%d,%d,%d", &x, &y, &w, &h, &c);
                // sprintf(temp, "display.drawRect(%d, %d, %d, %d, %d)\n\r", x, y, w, h, c);
                // Serial.print(temp);
                display.drawRect(x, y, w, h, c);
                break;

            case '5':
                sscanf(s + 3, "%d,%d,%d,%d", &x, &y, &r, &c);
                // sprintf(temp, "display.drawCircle(%d, %d, %d, %d)\n\r", x, y, r, c);
                // Serial.print(temp);
                display.drawCircle(x, y, r, c);
                break;

            case '6':
                sscanf(s + 3, "%d,%d,%d,%d,%d,%d,%d", &x1, &y1, &x2, &y2, &x3, &y3, &c);
                // sprintf(temp, "display.drawTriangle(%d, %d, %d, %d, %d, %d, %d)\n\r", x1, y1, x2, y2, x3, y3, c);
                // Serial.print(temp);
                display.drawTriangle(x1, y1, x2, y2, x3, y3, c);
                break;

            case '7':
                sscanf(s + 3, "%d,%d,%d,%d,%d,%d", &x, &y, &w, &h, &r, &c);
                // sprintf(temp, "display.drawRoundRect(%d, %d, %d, %d, %d, %d)\n\r", x, y, w, h, r, c);
                // Serial.print(temp);
                display.drawRoundRect(x, y, w, h, r, c);
                break;

            case '8':
                sscanf(s + 3, "%d,%d,%d,%d,%d", &x, &y, &w, &h, &c);
                // sprintf(temp, "display.fillRect(%d, %d, %d, %d, %d)\n\r", x, y, w, h, c);
                // Serial.print(temp);
                display.fillRect(x, y, w, h, c);
                break;

            case '9':
                sscanf(s + 3, "%d,%d,%d,%d", &x, &y, &r, &c);
                // sprintf(temp, "display.fillCircle(%d, %d, %d, %d)\n\r", x, y, r, c);
                // Serial.print(temp);
                display.fillCircle(x, y, r, c);
                break;

            case 'A':
                sscanf(s + 3, "%d,%d,%d,%d,%d,%d,%d", &x1, &y1, &x2, &y2, &x3, &y3, &c);
                // sprintf(temp, "display.fillTriangle(%d, %d, %d, %d, %d, %d, %d)\n\r", x1, y1, x2, y2, x3, y3, c);
                // Serial.print(temp);
                display.fillTriangle(x1, y1, x2, y2, x3, y3, c);
                break;

            case 'B':
                sscanf(s + 3, "%d,%d,%d,%d,%d,%d", &x, &y, &w, &h, &r, &c);
                // sprintf(temp, "display.fillRoundRect(%d, %d, %d, %d, %d, %d)\n\r", x, y, w, h, r, c);
                // Serial.print(temp);
                display.fillRoundRect(x, y, w, h, r, c);
                break;

            case 'C':
                sscanf(s + 3, "\"%2000[^\"]\"", strTemp);
                n = strlen(strTemp);
                for (int i = 0; i < n; i++)
                {
                    strTemp[i] = toupper(strTemp[i]);
                }
                for (int i = 0; i < n; i += 2)
                {
                    strTemp[i / 2] = (hexToChar(strTemp[i]) << 4) | (hexToChar(strTemp[i + 1]) & 0x0F);
                }
                strTemp[n / 2] = 0;
                // Serial.print("display.print(\"");
                // Serial.print(strTemp);
                // Serial.println("\");");
                display.print(strTemp);
                break;

            case 'D':
                sscanf(s + 3, "%d", &c);
                // sprintf(temp, "display.setTextSize(%d)\n", c);
                // Serial.print(temp);
                display.setTextSize(c);
                break;

            case 'E':
                sscanf(s + 3, "%d,%d", &x, &y);
                // sprintf(temp, "display.setCursor(%d, %d)\n", x, y);
                // Serial.print(temp);
                display.setCursor(x, y);
                break;

            case 'F':
                sscanf(s + 3, "%c", &b);
                // sprintf(temp, "display.setTextWrap(%s)\n", b == 'T' ? "True" : "False");
                // Serial.print(temp);
                if (b == 'T')
                    display.setTextWrap(true);
                if (b == 'F')
                    display.setTextWrap(false);
                break;

            case 'G':
                sscanf(s + 3, "%d", &c);
                c &= 3;
                // sprintf(temp, "display.setRotation(%d)\n", c);
                // Serial.print(temp);
                display.setRotation(c);
                break;

            case 'H':
                sscanf(s + 3, "%d,%d,\"%149[^\"]\"", &x, &y, strTemp);
                n = strlen(strTemp);
                for (int i = 0; i < n; i++)
                {
                    strTemp[i] = toupper(strTemp[i]);
                }
                for (int i = 0; i < n; i += 2)
                {
                    strTemp[i / 2] = (hexToChar(strTemp[i]) << 4) | (hexToChar(strTemp[i + 1]) & 0x0F);
                }
                strTemp[n / 2] = 0;
                r = display.sdCardInit();
                if (r)
                {
                    r = display.drawBitmapFromSd(strTemp, x, y);
                    Serial.print("#H(");
                    Serial.print(r, DEC);
                    Serial.println(")*");
                    Serial.flush();
                    // sprintf(temp, "display.drawBitmap(%d, %d, %s)\n", x, y, strTemp);
                    // Serial.print(temp);
                }
                else
                {
                    Serial.println("#H(-1)*");
                    Serial.flush();
                }
                break;

            case 'I':
                sscanf(s + 3, "%d", &c);
                // sprintf(temp, "display.setDisplayMode(%s)\n", c == 0 ? "INKPLATE_1BIT" : "INKPLATE_3BIT");
                // Serial.print(temp);
                if (c == 1)
                    display.selectDisplayMode(INKPLATE_1BIT);
                if (c == 3)
                    display.selectDisplayMode(INKPLATE_3BIT);
                break;

            case 'J':
                sscanf(s + 3, "%c", &b);
                if (b == '?')
                {
                    // if (0 == 0) {
                    //  Serial.println("#J(0)*");
                    //} else {
                    //  Serial.println("#J(1)*");
                    //}
                    if (display.getDisplayMode() == INKPLATE_1BIT)
                    {
                        Serial.println("#J(0)*");
                        Serial.flush();
                    }
                    if (display.getDisplayMode() == INKPLATE_3BIT)
                    {
                        Serial.println("#J(1)*");
                        Serial.flush();
                    }
                }
                break;

            case 'K':
                sscanf(s + 3, "%c", &b);
                if (b == '1')
                {
                    // Serial.print("display.clearDisplay();\n");
                    display.clearDisplay();
                }
                break;

            case 'L':
                sscanf(s + 3, "%c", &b);
                if (b == '1')
                {
                    // Serial.print("display.display();\n");
                    display.display();
                }
                break;

            case 'M':
                sscanf(s + 3, "%d,%d,%d", &y1, &x2, &y2);
                // sprintf(temp, "display.partialUpdate(%d, %d, %d);\n", y1, x2, y2);
                // Serial.print(temp);
                display.partialUpdate();
                break;

            case 'N':
                sscanf(s + 3, "%c", &b);
                if (b == '?')
                {
                    Serial.print("#N(");
                    Serial.print(display.readTemperature(), DEC);
                    // Serial.print(23, DEC);
                    Serial.println(")*");
                    Serial.flush();
                }
                break;

            case 'O':
                sscanf(s + 3, "%d", &c);
                if (c >= 0 && c <= 2)
                {
                    Serial.print("#O(");
                    Serial.print(display.readTouchpad(c), DEC);
                    // Serial.print(0, DEC);
                    Serial.println(")*");
                    Serial.flush();
                }
                break;

            case 'P':
                sscanf(s + 3, "%c", &b);
                if (b == '?')
                {
                    Serial.print("#P(");
                    Serial.print(display.readBattery(), 2);
                    // Serial.print(3.54, 2);
                    Serial.println(")*");
                    Serial.flush();
                }
                break;

            case 'Q':
                sscanf(s + 3, "%d", &c);
                c &= 1;
                // if (c == 0) Serial.print("display.einkOff();\n");
                // if (c == 1) Serial.print("display.einkOn();\n");
                if (c == 0)
                    display.einkOff();
                if (c == 1)
                    display.einkOn();
                break;

            case 'R':
                sscanf(s + 3, "%c", &b);
                if (b == '?')
                {
                    Serial.print("#R(");
                    Serial.print(display.getPanelState(), DEC);
                    // Serial.print(1, DEC);
                    Serial.println(")*");
                    Serial.flush();
                }
                break;
            }
            *s = 0;
            *e = 0;
        }
    }
}

int hexToChar(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    return -1;
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
    display.print("Send '1', '2', or '3' via serial to select waveform.");
    display.setCursor(50, 780);
    display.print("Currently selected: ");
    display.print(_selected + 1); // Increment by 1 for printing

    for (int i = 0; i < 8; i++)
    {
        display.fillRect(i * w, 0, w, h, i);
    }
    display.display();
}

// Prompt the user to enter '1', '2' or '3' via serial to select the waveform
// Returns 0 when done
int getWaveformFromSerial(int *selected)
{
    char serialBuffer[50];
    unsigned long serialTimeout;

    showGradient(*selected);

    while (true)
    {
        Serial.println("Write '1', '2' or '3' to select waveform.\nWrite 'OK' to confirm.");
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
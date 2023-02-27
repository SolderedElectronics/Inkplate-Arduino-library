<<<<<<< HEAD:examples/Inkplate6/Diagnostics/Inkplate6_Factory_Programming_VCOM/Inkplate6_Factory_Programming_VCOM.ino
/**
 **************************************************
 * @file        Inkplate_Factory_Programming_VCOM.ino
 *
 * @brief       File for programming the Inkplate's VCOM
 *
 * @note        !WARNING! VCOM can only be set 100 times, so keep usage to a minimum.
 *              !WARNING! Use at your own risk.
 *
 *              Inkplate 6 does not support auto VCOM, it has to be set manually.
 *              The user will be prompted to enter VCOM via serial (baud 115200).
 *              VCOM ranges from 0.0 to -5.0.
 *
 *              Tests will also be done, to pass all tests:
 *              -edit the WiFi information in test.cpp.
 *              -connect a slave device via EasyC on address 0x30 (you may change this in test.cpp also).
 *              -insert a formatted microSD card (doesn't have to be empty)
 *              -press wake button to finish testing
 *
 *License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the
 *LICENSE file included with this example. If you have any questions about
 *licensing, please visit https://soldered.com/contact/ Distributed as-is; no
 *warranty is given.
 *
 * @authors     Soldered
 ***************************************************/
=======
// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#if !defined(ARDUINO_ESP32_DEV) && !defined(ARDUINO_INKPLATE6V2)
#error "Wrong board selection for this example, please select Inkplate 10 or Inkplate 10 V2 in the boards menu."
#endif

// Do not forget to add WiFi SSID and WiFi Password in test.cpp!

// Uncomment if you want to skip test of the second I/O expander
#define SKIP_SECOND_EXPANDER
>>>>>>> 93efc99f1d39c8fc1fa726ac84022b59e384def1:examples/Inkplate6/Others/Inkplate_Factory_Programming_VCOM/Inkplate_Factory_Programming_VCOM.ino

#include "EEPROM.h"
#include "Inkplate.h"
#include "image.h"
#include "test.h"
#include <Wire.h>
#include "test.h"

Inkplate display(INKPLATE_1BIT);

<<<<<<< HEAD:examples/Inkplate6/Diagnostics/Inkplate6_Factory_Programming_VCOM/Inkplate6_Factory_Programming_VCOM.ino
// If you want to write new VCOM voltage and perform all tests change this number
const int EEPROMaddress = 0;
=======
// VCOM voltage is inserted with the UART after the periph. test.
double vcomVoltage;

// EEPROMOffset must be between 0 and 64
const int EEPROMOffset = 0;
>>>>>>> 93efc99f1d39c8fc1fa726ac84022b59e384def1:examples/Inkplate6/Others/Inkplate_Factory_Programming_VCOM/Inkplate_Factory_Programming_VCOM.ino

// Peripheral mode variables and arrays
#define BUFFER_SIZE 1000
char commandBuffer[BUFFER_SIZE + 1];
char strTemp[2001];

void setup()
{
    Serial.begin(115200);
<<<<<<< HEAD:examples/Inkplate6/Diagnostics/Inkplate6_Factory_Programming_VCOM/Inkplate6_Factory_Programming_VCOM.ino
    display.setTextSize(3);
    EEPROM.begin(512);
    Wire.begin();

    // Wakeup button
    pinMode(GPIO_NUM_36, INPUT);

    // Setting default value for safety
    double vcomVoltage = -1.3;

    bool isFirstStartup = (EEPROM.read(EEPROMaddress) != 170);

    if (isFirstStartup)
    {
        // Try to ping first IO expander to test I2C
        Wire.setTimeOut(3000);
        Wire.beginTransmission(IO_INT_ADDR);
        int result = Wire.endTransmission();

        if (result == 5)
        {
            Serial.println("I2C Bus Error!");
            failHandler(true);
        }
    }

    // If I2C is OK, initialise the display
    display.begin();

    if (isFirstStartup)
    {
        // Test all the peripherals
        testPeripheral();

        do
        {
            // Get the VCOM voltage from serial
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
=======
    display.begin();
    EEPROM.begin(512);

    // Check if VCOM programming is not already done
    if (EEPROM.read(EEPROMOffset) != 170)
    {
        // Test all peripherals of the Inkplate (I/O expander, RTC, uSD card holder, etc).
        // For testing old Inkplate versions with no RTC and second I/O expander use testPeripheral(1);
        testPeripheral();
        
        // Wait until valid VCOM has been recieved
        uint8_t flag = getVCOMFromSerial(&vcomVoltage);

        // If the flag is 1, use manual inserted VCOM voltage from UART
        if (flag == 1)
        {
            display.printf("MANUAL VCOM: %.2lf", vcomVoltage);
            display.partialUpdate();
            if (!writeVCOMToEEPROM(vcomVoltage))
            {
                display.println("VCOM PROG. FAIL");
                failHandler();
            }
        }
        else if (flag == 2)
        {
            // If the flag is set to 2, use automatic VCOM voltage detection.
            vcomVoltage = readVCOM();
            display.einkOff();
            display.println("AUTO VCOM");
            display.partialUpdate();
            if (!writeVCOMToEEPROM(vcomVoltage))
            {
                display.println("VCOM PROG. FAIL");
                failHandler();
            }
        }

        EEPROM.write(EEPROMOffset, 170);
        EEPROM.commit();
>>>>>>> 93efc99f1d39c8fc1fa726ac84022b59e384def1:examples/Inkplate6/Others/Inkplate_Factory_Programming_VCOM/Inkplate_Factory_Programming_VCOM.ino
    }
    else
    {
        Serial.println("VCOM already set!");
        display.einkOn();
        vcomVoltage = (double)(readReg(0x03) | ((uint16_t)((readReg(0x04) & 1) << 8))) / (-100);
    }

    memset(commandBuffer, 0, BUFFER_SIZE);

    showSplashScreen(vcomVoltage);
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
            switch (*(s + 1))
            {
                case '?':
                    Serial.print("OK");
                    break;

                case '0':
                    sscanf(s + 3, "%d,%d,%d", &x, &y, &c);
                    display.drawPixel(x, y, c);
                    break;

                case '1':
                    sscanf(s + 3, "%d,%d,%d,%d,%d", &x1, &y1, &x2, &y2, &c);
                    display.drawLine(x1, y1, x2, y2, c);
                    break;

                case '2':
                    sscanf(s + 3, "%d,%d,%d,%d", &x, &y, &l, &c);
                    display.drawFastVLine(x, y, l, c);
                    break;

                case '3':
                    sscanf(s + 3, "%d,%d,%d,%d", &x, &y, &l, &c);
                    display.drawFastHLine(x, y, l, c);
                    break;

                case '4':
                    sscanf(s + 3, "%d,%d,%d,%d,%d", &x, &y, &w, &h, &c);
                    display.drawRect(x, y, w, h, c);
                    break;

                case '5':
                    sscanf(s + 3, "%d,%d,%d,%d", &x, &y, &r, &c);
                    display.drawCircle(x, y, r, c);
                    break;

                case '6':
                    sscanf(s + 3, "%d,%d,%d,%d,%d,%d,%d", &x1, &y1, &x2, &y2, &x3, &y3, &c);
                    display.drawTriangle(x1, y1, x2, y2, x3, y3, c);
                    break;

                case '7':
                    sscanf(s + 3, "%d,%d,%d,%d,%d,%d", &x, &y, &w, &h, &r, &c);
                    display.drawRoundRect(x, y, w, h, r, c);
                    break;

                case '8':
                    sscanf(s + 3, "%d,%d,%d,%d,%d", &x, &y, &w, &h, &c);
                    display.fillRect(x, y, w, h, c);
                    break;

                case '9':
                    sscanf(s + 3, "%d,%d,%d,%d", &x, &y, &r, &c);
                    display.fillCircle(x, y, r, c);
                    break;

                case 'A':
                    sscanf(s + 3, "%d,%d,%d,%d,%d,%d,%d", &x1, &y1, &x2, &y2, &x3, &y3, &c);
                    display.fillTriangle(x1, y1, x2, y2, x3, y3, c);
                    break;

                case 'B':
                    sscanf(s + 3, "%d,%d,%d,%d,%d,%d", &x, &y, &w, &h, &r, &c);
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
                    display.print(strTemp);
                    break;

                case 'D':
                    sscanf(s + 3, "%d", &c);
                    display.setTextSize(c);
                    break;

                case 'E':
                    sscanf(s + 3, "%d,%d", &x, &y);
                    display.setCursor(x, y);
                    break;

                case 'F':
                    sscanf(s + 3, "%c", &b);
                    if (b == 'T')
                        display.setTextWrap(true);
                    if (b == 'F')
                        display.setTextWrap(false);
                    break;

                case 'G':
                    sscanf(s + 3, "%d", &c);
                    c &= 3;
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
                    }
                    else
                    {
                        Serial.println("#H(-1)*");
                        Serial.flush();
                    }
                    break;

                case 'I':
                    sscanf(s + 3, "%d", &c);
                    if (c == 1)
                        display.selectDisplayMode(INKPLATE_1BIT);
                    if (c == 3)
                        display.selectDisplayMode(INKPLATE_3BIT);
                    break;

                case 'J':
                    sscanf(s + 3, "%c", &b);
                    if (b == '?')
                    {
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
                        display.clearDisplay();
                    }
                    break;

                case 'L':
                    sscanf(s + 3, "%c", &b);
                    if (b == '1')
                    {
                        display.display();
                    }
                    break;

                case 'M':
                    sscanf(s + 3, "%d,%d,%d", &y1, &x2, &y2);
                    display.partialUpdate();
                    break;

                case 'N':
                    sscanf(s + 3, "%c", &b);
                    if (b == '?')
                    {
                        Serial.print("#N(");
                        Serial.print(display.readTemperature(), DEC);
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
                        Serial.println(")*");
                        Serial.flush();
                    }
                    break;

                case 'Q':
                    sscanf(s + 3, "%d", &c);
                    c &= 1;
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

void showSplashScreen(float vComVoltage)
{
    display.clearDisplay();
    display.display();
    display.selectDisplayMode(INKPLATE_3BIT);
<<<<<<< HEAD:examples/Inkplate6/Diagnostics/Inkplate6_Factory_Programming_VCOM/Inkplate6_Factory_Programming_VCOM.ino
    display.drawBitmap3Bit(0, 0, demo_image, demo_image_w, demo_image_h);
    display.setTextColor(0, 7);
    display.setTextSize(1);
    display.setCursor(5, 583);
    display.print(vComVoltage, 2);
=======
    display.drawImage(demo_image, 0, 0, demo_image_w, demo_image_h);
    display.setTextColor(0, 7);
    display.setTextSize(1);
    display.setCursor(5, 5);
    display.print(vcomVoltage, 2);
>>>>>>> 93efc99f1d39c8fc1fa726ac84022b59e384def1:examples/Inkplate6/Others/Inkplate_Factory_Programming_VCOM/Inkplate_Factory_Programming_VCOM.ino
    display.print("V");
    display.display();
}

<<<<<<< HEAD:examples/Inkplate6/Diagnostics/Inkplate6_Factory_Programming_VCOM/Inkplate6_Factory_Programming_VCOM.ino
// This function is corrected
=======
// Inkplate 6 Null waveform
void writeToScreen()
{
    // Prepare the panel for the VCOM measurement
    display.clearDisplay();
    display.display(1);

    // Waveform for VCOM measurement.
    display.clean(1, 8);
    display.clean(0, 2);
    display.clean(2, 10);
    delay(10);
}

// Do not use until until null waveform is set correctly
double readVCOM()
{
    double vcomVolts;
    writeReg(0x01, B00111111); // Enable all rails
    writeReg(0x04, (readReg(0x04) | B00100000));
    writeToScreen();
    writeReg(0x04, (readReg(0x04) | B10000000));
    delay(10);
    while (display.digitalReadMCP(6))
    {
        delay(1);
    };
    readReg(0x07);
    uint16_t vcom = ((readReg(0x04) & 1) << 8) | readReg(0x03);
    vcomVolts = vcom * 10 / 1000.0;
    display.einkOff();
    return -vcomVolts;
}

>>>>>>> 93efc99f1d39c8fc1fa726ac84022b59e384def1:examples/Inkplate6/Others/Inkplate_Factory_Programming_VCOM/Inkplate_Factory_Programming_VCOM.ino
uint8_t writeVCOMToEEPROM(double v)
{
    int vcom = int(abs(v) * 100);
    int vcomH = (vcom >> 8) & 1;
    int vcomL = vcom & 0xFF;
<<<<<<< HEAD:examples/Inkplate6/Diagnostics/Inkplate6_Factory_Programming_VCOM/Inkplate6_Factory_Programming_VCOM.ino

    // Set PCAL pin where TPS65186 INT pin is connectet to input pull up
    display.pinModeInternal(IO_INT_ADDR, display.ioRegsInt, 6, INPUT_PULLUP);

    // First power up TPS65186 so we can communicate with it
    display.einkOn();

    // Wait a little bit
    delay(250);
=======
    
    // Set MCP23017 pin where TPS65186 INT pin is connectet to input pull up
    display.pinModeInternal(MCP23017_INT_ADDR, display.mcpRegsInt, 6, INPUT_PULLUP);

    // First power up TPS65186 so we can communicate with it
    display.einkOn();
>>>>>>> 93efc99f1d39c8fc1fa726ac84022b59e384def1:examples/Inkplate6/Others/Inkplate_Factory_Programming_VCOM/Inkplate_Factory_Programming_VCOM.ino

    // Wait a little bit
    delay(250);
    
    // Send to TPS65186 first 8 bits of VCOM
    writeReg(0x03, vcomL);

    // Send new value of register to TPS
    writeReg(0x04, vcomH);
    delay(1);

    // Program VCOM value to EEPROM
    writeReg(0x04, vcomH | (1 << 6));

    // Wait until EEPROM has been programmed
    delay(100);
    do
    {
        delay(1);
<<<<<<< HEAD:examples/Inkplate6/Diagnostics/Inkplate6_Factory_Programming_VCOM/Inkplate6_Factory_Programming_VCOM.ino
    } while (display.digitalReadInternal(IO_INT_ADDR, display.ioRegsInt, 6));
=======
    } while (display.digitalReadInternal(MCP23017_INT_ADDR, display.mcpRegsInt, 6));
>>>>>>> 93efc99f1d39c8fc1fa726ac84022b59e384def1:examples/Inkplate6/Others/Inkplate_Factory_Programming_VCOM/Inkplate_Factory_Programming_VCOM.ino

    // Clear Interrupt flag by reading INT1 register
    readReg(0x07);

    // Now, power off whole TPS
    display.einkOff();

    // Wait a little bit...
    delay(1000);

    // Power up TPS again
    display.einkOn();

    delay(10);

    // Read VCOM valuse from registers
    vcomL = readReg(0x03);
    vcomH = readReg(0x04);
<<<<<<< HEAD:examples/Inkplate6/Diagnostics/Inkplate6_Factory_Programming_VCOM/Inkplate6_Factory_Programming_VCOM.ino
=======
    Serial.print("Vcom: ");
    Serial.println(vcom);
    Serial.print("Vcom register: ");
    Serial.println(vcomL | (vcomH << 8));
>>>>>>> 93efc99f1d39c8fc1fa726ac84022b59e384def1:examples/Inkplate6/Others/Inkplate_Factory_Programming_VCOM/Inkplate_Factory_Programming_VCOM.ino

    // Trun off the TPS65186 and wait a little bit
    display.einkOff();
    delay(100);

    if (vcom != (vcomL | (vcomH << 8)))
    {
        Serial.println("\nVCOM EEPROM PROGRAMMING FAILED!\n");
        return 0;
    }
    else
    {
        Serial.println("\nVCOM EEPROM PROGRAMMING OK\n");
        return 1;
    }
    return 0;
<<<<<<< HEAD:examples/Inkplate6/Diagnostics/Inkplate6_Factory_Programming_VCOM/Inkplate6_Factory_Programming_VCOM.ino
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
=======
>>>>>>> 93efc99f1d39c8fc1fa726ac84022b59e384def1:examples/Inkplate6/Others/Inkplate_Factory_Programming_VCOM/Inkplate_Factory_Programming_VCOM.ino
}

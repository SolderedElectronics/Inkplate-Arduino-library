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

uint8_t ioRegsInt[22];

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

int selectedWaveform = 1;

void setup()
{
    Serial.begin(115200);
    display.begin();
    EEPROM.begin(512);

    //Serial.println("resetting vcom voltage..");
    //delay(2000);
    //EEPROM.write(EEPROMaddress, 0);
    //EEPROM.commit();
    //Serial.println("It's been reset!");
    //delay(2000);

    // setting default value for safety
    vcomVoltage = -1.3;

    if (EEPROM.read(EEPROMaddress) != 170)
    {
        Serial.println("Read not 170, go to tests!");

        testPeripheral();

        display.pinModeInternal(IO_INT_ADDR, ioRegsInt, 6, INPUT_PULLUP);
        writeVCOMToEEPROM(vcomVoltage);
        EEPROM.write(EEPROMaddress, 170);
        EEPROM.commit();
        display.selectDisplayMode(INKPLATE_3BIT);

        int result = 1;

        do
        {
            result = getWaveformFromSerial(&selectedWaveform);
            Serial.println("Result:");
            Serial.println(result);
        } while (result != 0);

        while (1)
        {
            Serial.println("finished");
            delay(100);
        }
    }

    while (1)
    {
        Serial.println("finished read isnt 170");
        delay(100);
    }
}

void loop()
{
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

void showSplashScreen(struct waveformData _w)
{
    display.clearDisplay();
    display.display();
    display.selectDisplayMode(INKPLATE_3BIT);
    display.drawBitmap3Bit(0, 0, demo_image, demo_image_w, demo_image_h);
    display.setTextColor(0, 7);
    display.setTextSize(1);
    display.setCursor(10, 10);
    display.print(vcomVoltage, 2);
    display.print("V");
    display.setCursor(10, 20);
    display.print("Waveform");
    display.print(_w.waveformId - 20 + 1, DEC);
    display.display();
}

// Need to be changed to inkplate 10 Null waveform
void writeToScreen()
{
    display.clean(1, 8);
    display.clean(0, 2);
    display.clean(2, 10);
    // delay(10);
}

// Do not use until until null waveform is set correctly
double readVCOM()
{
    double vcomVolts;
    writeReg(0x01, B00111111); // enable all rails
    writeReg(0x04, (readReg(0x04) | B00100000));
    writeToScreen();
    writeReg(0x04, (readReg(0x04) | B10000000));
    delay(10);
    while (display.digitalReadIO(6))
    {
        delay(1);
    };
    readReg(0x07);
    uint16_t vcom = ((readReg(0x04) & 1) << 8) | readReg(0x03);
    vcomVolts = vcom * 10 / 1000.0;
    display.einkOff();
    return -vcomVolts;
}

void writeVCOMToEEPROM(double v)
{
    int vcom = int(abs(v) * 100);
    int vcomH = (vcom >> 8) & 1;
    int vcomL = vcom & 0xFF;
    // First, we have to power up TPS65186
    // Pull TPS65186 WAKEUP pin to High
    display.digitalWriteInternal(IO_INT_ADDR, ioRegsInt, 3, HIGH);

    // Pull TPS65186 PWR pin to High
    display.digitalWriteInternal(IO_INT_ADDR, ioRegsInt, 4, HIGH);
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
    } while (display.digitalReadInternal(IO_INT_ADDR, ioRegsInt, 6));

    // Clear Interrupt flag by reading INT1 register
    readReg(0x07);

    // Now, power off whole TPS
    // Pull TPS65186 WAKEUP pin to Low
    display.digitalWriteInternal(IO_INT_ADDR, ioRegsInt, 3, LOW);

    // Pull TPS65186 PWR pin to Low
    display.digitalWriteInternal(IO_INT_ADDR, ioRegsInt, 4, LOW);

    // Wait a little bit...
    delay(1000);

    // Power up TPS again
    display.digitalWriteInternal(IO_INT_ADDR, ioRegsInt, 3, HIGH);

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
    display.print(_selected + 1);
    // display.drawRect((_selected * 6 * 4 * 2) + 800 - 3, 740, (6 * 4) + 2, (8 * 4) + 2, 0);

    for (int i = 0; i < 8; i++)
    {
        display.fillRect(i * w, 0, w, h, i);
    }
    display.display();
}

// Returns 0 when done
int getWaveformFromSerial(int *selected)
{
    char serialBuffer[50];
    unsigned long serialTimeout;

    showGradient(*selected - 1);

    while (true)
    {
        Serial.println("Write '1', '2' or '3' to select waveform.\nWrite 'ACCEPT' to confirm.");
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
            *selected = 1;
            return 1;
        }
        else if (strstr(serialBuffer, "2") != NULL)
        {
            *selected = 2;
            return 2;
        }
        if (strstr(serialBuffer, "3") != NULL)
        {
            *selected = 3;
            return 3;
        }
        else if (strstr(serialBuffer, "ACCEPT") != NULL)
        {
            return 0;
        }
    }

    Serial.println("Input error!");
    return 0;
}
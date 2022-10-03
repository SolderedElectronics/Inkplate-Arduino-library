#include "EEPROM.h"
#include "Inkplate.h"
#include "image.h"
#include <Wire.h>

Inkplate display(INKPLATE_1BIT);

double vcomVoltage = -2.95;

// EEPROMAddress must be smaller than 64.
int EEPROMaddress = 10;

// Peripheral mode variables and arrays
#define BUFFER_SIZE 1000
char commandBuffer[BUFFER_SIZE + 1];
char strTemp[2001];

const char sdCardTestStringLength = 100;
const char *testString = {"This is some test string..."};

void setup()
{
    display.begin();
    Serial.begin(115200);
    EEPROM.begin(64);

    // Init the touchscreen - you need to touch the bottom right edge to activate it
    if (display.tsInit(true))
    {
        Serial.println("Touchscreen init ok");
    }
    else
    {
        Serial.println("Touchscreen init fail");
        display.setTextSize(4);
        display.setTextColor(0, 7);
        display.setCursor(300, 300);
        display.print("Touch error");
        display.display();
        while (true);
    }

    // Turn on the frontlight
    display.frontlight(true);

    if (EEPROM.read(EEPROMaddress) != 170)
    {
        microSDCardTest();
        writeVCOMToEEPROM(vcomVoltage);
        EEPROM.write(EEPROMaddress, 170);
        EEPROM.commit();
    }
    else
    {
        vcomVoltage = getVCOM();
    }
    memset(commandBuffer, 0, BUFFER_SIZE);

    showSplashScreen();
}

void loop()
{
  if (display.touchInArea(900, 0, 124, 124)) {
    display.setCursor(970, 10);
    display.print("Touch OK!");
    display.display();
  }

  // Peripheral mode
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
          if (c == INKPLATE_1BIT)
            display.selectDisplayMode(INKPLATE_1BIT);
          if (c == INKPLATE_3BIT)
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

void showSplashScreen()
{
  display.display();
  display.selectDisplayMode(INKPLATE_3BIT);
  display.drawBitmap3Bit(0, 0, picture1, picture1_w, picture1_h);
  display.setTextColor(0, 7);
  display.setTextSize(1);
  display.setCursor(970, 57);
  display.print(vcomVoltage, 2);
  display.print("V");
  display.display();
}

void writeToScreen()
{
  // delay(10);
}

double readVCOM()
{
    double vcomVolts;
    display.einkOn();
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

double getVCOM()
{
  display.einkOn();
  uint16_t vcom = ((readReg(0x04) & 1) << 8) | readReg(0x03);
  double vcomVolts = vcom * 10.0 / 1000.0;
  display.einkOff();
  return vcomVolts;
}

void writeVCOMToEEPROM(double v)
{
  int vcom = int(abs(v) * 100);
  int vcomH = (vcom >> 8) & 1;
  int vcomL = vcom & 0xFF;

  // Set I/O Expander pin where TPS65186 INT pin is connectet to input pull up
  display.pinModeInternal(IO_INT_ADDR, display.ioRegsInt, 6, INPUT_PULLUP);

  // First power up TPS65186 so we can communicate with it
  display.einkOn();

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
  display.einkOff();

  // Wait a little bit...
  delay(1000);

  // Power up TPS again
  display.einkOn();

  delay(10);

  // Read VCOM valuse from registers
  vcomL = readReg(0x03);
  vcomH = readReg(0x04);

  if (vcom != (vcomL | (vcomH << 8)))
  {
    Serial.println("\nVCOM EEPROM PROGRAMMING FAILED!\n");
  }
  else
  {
    Serial.println("\nVCOM EEPROM PROGRAMMING OK\n");
  }
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

void microSDCardTest()
{
    display.setTextSize(5);
    display.setCursor(100, 100);
    display.print("microSD card slot test ");

    if (!checkMicroSDCard())
    {
        display.print("FAIL!");
        display.display();
        while(1);
    }
    display.clearDisplay();
}
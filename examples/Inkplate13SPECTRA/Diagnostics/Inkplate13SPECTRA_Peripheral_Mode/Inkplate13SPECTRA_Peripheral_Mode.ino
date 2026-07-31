/**
 **************************************************
 * @file        Inkplate13SPECTRA_Peripheral_Mode.ino
 * @brief       UART-controlled peripheral mode for Soldered Inkplate 13SPECTRA.
 *
 * @details     Using this sketch, you don't have to program and control the
 *              e-paper using Arduino code. Instead, you can send UART commands.
 *              This gives you the flexibility to use this Inkplate 13SPECTRA on
 *              any platform. Every Inkplate comes with this peripheral mode
 *              right from the factory.
 *
 *              Because it uses UART it is a little bit slower, and it is not
 *              recommended to send a bunch of drawPixel commands to draw an
 *              image. Instead, load bitmaps and pictures on the SD card and load
 *              the image from SD. If we missed some function, you can modify this
 *              sketch and make your own.
 *
 *              UART settings are: 115200 baud, standard parity, ending with
 *              "\n\r" (both). You can send commands via the USB port or by
 *              directly connecting to the ESP32 TX and RX pins. Don't forget you
 *              need to send #L(1)* after each command to show it on the display
 *              (equal to inkplate.display()).
 *
 * Requirements:
 * - Board:      Soldered Inkplate 13SPECTRA
 * - Hardware:   Inkplate 13SPECTRA, USB cable
 * - Extra:      Optional microSD card for image files
 * - Serial:     115200 baud, standard parity, line ending "\n\r" (both)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 13SPECTRA"
 *    from Tools -> Board.
 * 2) Upload the sketch to Inkplate 13SPECTRA.
 * 3) Open a serial terminal at 115200 baud (or connect to the ESP32 TX/RX pins).
 * 4) Send peripheral mode commands, following each with #L(1)* to refresh the
 *    display.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/13spectra/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered Electronics
 * @date        2026-01-21
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE13SPECTRA
#error "Wrong board selection for this example, please select Soldered Inkplate 13SPECTRA in the boards menu."
#endif

#include <Inkplate.h>
Inkplate inkplate;

#define BUFFER_SIZE 1000
char commandBuffer[BUFFER_SIZE + 1];
char strTemp[2001];

void setup()
{
    inkplate.begin();
    Serial.begin(115200);
    memset(commandBuffer, 0, BUFFER_SIZE);
}

void loop()
{
    // put your main code here, to run repeatedly:
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
            int x, x1, x2, y, y1, y2, x3, y3, l, c, w, h, r, n, rx, ry, xc, yc;
            char b;
            char temp[150];
            switch (*(s + 1))
            {
            case '?':
                Serial.print("OK");
                break;

            case '0':
                sscanf(s + 3, "%d,%d,%d", &x, &y, &c);
                inkplate.drawPixel(x, y, c);
                break;

            case '1':
                sscanf(s + 3, "%d,%d,%d,%d,%d", &x1, &y1, &x2, &y2, &c);
                inkplate.drawLine(x1, y1, x2, y2, c);
                break;

            case '2':
                sscanf(s + 3, "%d,%d,%d,%d", &x, &y, &l, &c);
                inkplate.drawFastVLine(x, y, l, c);
                break;

            case '3':
                sscanf(s + 3, "%d,%d,%d,%d", &x, &y, &l, &c);
                inkplate.drawFastHLine(x, y, l, c);
                break;

            case '4':
                sscanf(s + 3, "%d,%d,%d,%d,%d", &x, &y, &w, &h, &c);
                inkplate.drawRect(x, y, w, h, c);
                break;

            case '5':
                sscanf(s + 3, "%d,%d,%d,%d", &x, &y, &r, &c);
                inkplate.drawCircle(x, y, r, c);
                break;

            case '6':
                sscanf(s + 3, "%d,%d,%d,%d,%d,%d,%d", &x1, &y1, &x2, &y2, &x3, &y3, &c);
                inkplate.drawTriangle(x1, y1, x2, y2, x3, y3, c);
                break;

            case '7':
                sscanf(s + 3, "%d,%d,%d,%d,%d,%d", &x, &y, &w, &h, &r, &c);
                inkplate.drawRoundRect(x, y, w, h, r, c);
                break;

            case '8':
                sscanf(s + 3, "%d,%d,%d,%d,%d", &x, &y, &w, &h, &c);
                inkplate.fillRect(x, y, w, h, c);
                break;

            case '9':
                sscanf(s + 3, "%d,%d,%d,%d", &x, &y, &r, &c);
                inkplate.fillCircle(x, y, r, c);
                break;

            case 'A':
                sscanf(s + 3, "%d,%d,%d,%d,%d,%d,%d", &x1, &y1, &x2, &y2, &x3, &y3, &c);
                inkplate.fillTriangle(x1, y1, x2, y2, x3, y3, c);
                break;

            case 'B':
                sscanf(s + 3, "%d,%d,%d,%d,%d,%d", &x, &y, &w, &h, &r, &c);
                inkplate.fillRoundRect(x, y, w, h, r, c);
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
                inkplate.print(strTemp);
                break;

            case 'D':
                sscanf(s + 3, "%d", &c);
                inkplate.setTextSize(c);
                break;

            case 'E':
                sscanf(s + 3, "%d,%d", &x, &y);
                inkplate.setCursor(x, y);
                break;

            case 'F':
                sscanf(s + 3, "%c", &b);
                if (b == 'T')
                    inkplate.setTextWrap(true);
                if (b == 'F')
                    inkplate.setTextWrap(false);
                break;

            case 'G':
                sscanf(s + 3, "%d", &c);
                c &= 3;
                inkplate.setRotation(c);
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
                r = inkplate.sdCardInit();
                if (r)
                {
                    r = inkplate.image.drawBitmapFromSd(strTemp, x, y);
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

            case 'K':
                sscanf(s + 3, "%c", &b);
                if (b == '1')
                {
                    inkplate.clearDisplay();
                }
                break;

            case 'L':
                sscanf(s + 3, "%c", &b);
                if (b == '1')
                {
                    inkplate.display();
                }
                break;

            case 'M':
                sscanf(s + 3, "%d,%d,%d", &y1, &x2, &y2);
                inkplate.display();
                break;

            case 'P':
                sscanf(s + 3, "%c", &b);
                if (b == '?')
                {
                    Serial.print("#P(");
                    Serial.print(inkplate.readBattery(), 2);
                    Serial.println(")*");
                    Serial.flush();
                }
                break;

            case 'S':
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
                r = inkplate.sdCardInit();
                if (r)
                {
                    r = inkplate.image.draw(strTemp, x, y);
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
            case 'T':
                int t;
                sscanf(s + 3, "%d,%d,%d,%d,%d,%d", &x1, &y1, &x2, &y2, &c, &t);
                inkplate.drawThickLine(x1, y1, x2, y2, c, t);
                break;
            case 'U':
                sscanf(s + 3, "%d,%d,%d,%d,%d", &rx, &ry, &xc, &yc, &c);
                inkplate.drawElipse(rx, ry, xc, yc, c);
                break;
            case 'V':
                sscanf(s + 3, "%d,%d,%d,%d,%d", &rx, &ry, &xc, &yc, &c);
                inkplate.fillElipse(rx, ry, xc, yc, c);
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
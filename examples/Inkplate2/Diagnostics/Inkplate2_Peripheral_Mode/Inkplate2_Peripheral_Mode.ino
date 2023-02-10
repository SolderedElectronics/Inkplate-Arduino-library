/*
   Inkplate2_Peripheral_Mode example for Soldered Inkplate 2
   Select "Soldered Inkplate2" from Tools -> Board menu.
   Don't have "Soldered Inkplate2" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   Using this sketch, you don't have to program and control e-paper using Arduino code.
   Instead, you can send UART command (explained in documentation that can be found inside folder of this sketch).
   This gives you flexibility that you can use this Inkplate 2 on any platform!

   Because it uses UART, it's little bit slower and it's not recommended to send bunch of
   drawPixel command to draw some image. Instead, load bitmaps and pictures on SD card and load image from SD.
   If we missed some function, you can modify this and make your own.
   Also, every Inkplate comes with this peripheral mode right from the factory.
   
   The size of Inkplate 2 is limited, just as its functionality. It doesn't have display temperature nor battery 
   voltage monitoring.I/O expander is also missing on this board, just as RTC and SD card support. So using this 
   example is similar to other Inkplates, just there are less functions for this Inkplate. In documentation found
   here https://inkplate.readthedocs.io/en/latest/peripheral-mode.html is explained what functions Inkplate 2 supports.

   Learn more about Peripheral Mode in this update:
   https://www.crowdsupply.com/e-radionica/inkplate-6/updates/successfully-funded-also-third-party-master-controllers-and-partial-updates

   UART settings are: 115200 baud, standard parity, ending with "\n\r" (both) (Choose "BOTH NL and CR" in Serial monitor settings)
   You can send commands via USB port or by directly connecting to ESP32 TX and RX pins.
   Don't forget you need to send #L(1)* after each command to show it on the display
   (equal to display.display()).

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   14 April 2022 by soldered.com
*/

#include <Inkplate.h> // Include Inkplate library
Inkplate display;     // Init Inkplate object

#define BUFFER_SIZE 1000
char commandBuffer[BUFFER_SIZE + 1];
char strTemp[2001];
void setup()
{
    display.begin();                       // Init Inkplate library
    Serial.begin(115200);                  // Init serial communication
    memset(commandBuffer, 0, BUFFER_SIZE); // Clear the command buffer
}

void loop()
{
    if (Serial.available())     // Check for the incoming data
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
                // Draw pixel
                sscanf(s + 3, "%d,%d,%d", &x, &y, &c);
                display.drawPixel(x, y, c);
                break;

            case '1':
                // Draw line
                sscanf(s + 3, "%d,%d,%d,%d,%d", &x1, &y1, &x2, &y2, &c);
                display.drawLine(x1, y1, x2, y2, c);
                break;

            case '2':
                // Draw fast vertical line
                sscanf(s + 3, "%d,%d,%d,%d", &x, &y, &l, &c);
                display.drawFastVLine(x, y, l, c);
                break;

            case '3':
                // Draw fast horizontal line
                sscanf(s + 3, "%d,%d,%d,%d", &x, &y, &l, &c);
                display.drawFastHLine(x, y, l, c);
                break;

            case '4':
                // Draw rect.
                sscanf(s + 3, "%d,%d,%d,%d,%d", &x, &y, &w, &h, &c);
                display.drawRect(x, y, w, h, c);
                break;

            case '5':
                // Draw circle
                sscanf(s + 3, "%d,%d,%d,%d", &x, &y, &r, &c);
                display.drawCircle(x, y, r, c);
                break;

            case '6':
                // Draw triangle
                sscanf(s + 3, "%d,%d,%d,%d,%d,%d,%d", &x1, &y1, &x2, &y2, &x3, &y3, &c);
                display.drawTriangle(x1, y1, x2, y2, x3, y3, c);
                break;

            case '7':
                // Draw round rect.
                sscanf(s + 3, "%d,%d,%d,%d,%d,%d", &x, &y, &w, &h, &r, &c);
                display.drawRoundRect(x, y, w, h, r, c);
                break;

            case '8':
                // Draw filled rect.
                sscanf(s + 3, "%d,%d,%d,%d,%d", &x, &y, &w, &h, &c);
                display.fillRect(x, y, w, h, c);
                break;

            case '9':
                // Draw filled circle
                sscanf(s + 3, "%d,%d,%d,%d", &x, &y, &r, &c);
                display.fillCircle(x, y, r, c);
                break;

            case 'A':
                // Draw filled triangle
                sscanf(s + 3, "%d,%d,%d,%d,%d,%d,%d", &x1, &y1, &x2, &y2, &x3, &y3, &c);
                display.fillTriangle(x1, y1, x2, y2, x3, y3, c);
                break;

            case 'B':
                // Draw filled round triangle
                sscanf(s + 3, "%d,%d,%d,%d,%d,%d", &x, &y, &w, &h, &r, &c);
                display.fillRoundRect(x, y, w, h, r, c);
                break;

            case 'C':
                // Draw string
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
                // Set text size
                sscanf(s + 3, "%d", &c);
                display.setTextSize(c);
                break;

            case 'E':
                // Set print cursor position
                sscanf(s + 3, "%d,%d", &x, &y);
                display.setCursor(x, y);
                break;

            case 'F':
                // Enable or disable text wrapping.
                sscanf(s + 3, "%c", &b);
                if (b == 'T')
                    display.setTextWrap(true);
                if (b == 'F')
                    display.setTextWrap(false);
                break;

            case 'G':
                // Set screen rotation
                sscanf(s + 3, "%d", &c);
                c &= 3;
                display.setRotation(c);
                break;


            case 'K':
                // Clear the display (frame buffer only)
                sscanf(s + 3, "%c", &b);
                if (b == '1')
                {
                    display.clearDisplay();
                }
                break;

            case 'L':
                // Display image from the frame buffer
                sscanf(s + 3, "%c", &b);
                if (b == '1')
                {
                    display.display();
                }
                break;

            case 'T':
                // Draw thick line.
                int t;
                sscanf(s + 3, "%d,%d,%d,%d,%d,%d", &x1, &y1, &x2, &y2, &c, &t);
                display.drawThickLine(x1, y1, x2, y2, c, t);
                break;
            case 'U':
                // Draw elipse.
                sscanf(s + 3, "%d,%d,%d,%d,%d", &rx, &ry, &xc, &yc, &c);
                display.drawElipse(rx, ry, xc, yc, c);
                break;
            case 'V':
                // Draw filled elipse
                sscanf(s + 3, "%d,%d,%d,%d,%d", &rx, &ry, &xc, &yc, &c);
                display.fillElipse(rx, ry, xc, yc, c);
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

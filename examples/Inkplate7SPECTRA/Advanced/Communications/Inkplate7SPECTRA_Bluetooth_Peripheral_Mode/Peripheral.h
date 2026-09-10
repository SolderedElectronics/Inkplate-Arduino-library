/*
   Inkplate7SPECTRA_Bluetooth_Peripheral_Mode example for Soldered Inkplate 7SPECTRA
   For this example you will need USB cable and an Inkplate 7SPECTRA
   Select "Soldered Inkplate 7SPECTRA" from Tools -> Board menu.
   Don't have "Soldered Inkplate 7SPECTRA" option? Follow our tutorial and add it:
   
   ///////////////////////////// INSERT BOARD DEF LINK HERE

   This example shows how to use Inkplate as a peripheral device over Bluetooth.

   Upload this example to the Inkplate and connect your phone to it via Bluetooth.
   First, you have to pair the Inkplate with your phone in Bluetooth settings in your phone, then go to the
   Serial Bluetooth Terminal app and you can find the Inkplate in the device list. You can use another similar app.
   If Bluetooth starts successfully, you can send commands from your phone. Don't forget you need to send #L(1)* after
   each command to show it on the display (equal to display->display()).

  Want to learn more about Inkplate? Visit https://docs.soldered.com/inkplate/
  21 January 2026 by Soldered Electronics
*/

char strTemp[2001];

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

void run(char commandBuffer[], size_t n, Inkplate *display, BLECharacteristic *pTxCharacteristic)
{
    char *s = NULL;
    char *e = NULL;
    for (int i = 0; i < n; i++)
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
            int x, x1, x2, y, y1, y2, x3, y3, l, c, w, h, r, n, rx, ry, xc, yc, yr;
            uint8_t hr, min, sec, wday, day, mon, k, as, am, ah, ad, aw, amc, v;
            bool ie, ip;
            RTC::rtcCountdownSrcClock sc;
            uint32_t ep, ae;
            char b;
            char temp[150];
            switch (*(s + 1))
            {
            case '?':
            {
                pTxCharacteristic->setValue("OK");
                pTxCharacteristic->notify();
                break;
            }

            case '0':
                sscanf(s + 3, "%d,%d,%d", &x, &y, &c);
                display->drawPixel(x, y, c);
                break;

            case '1':
                sscanf(s + 3, "%d,%d,%d,%d,%d", &x1, &y1, &x2, &y2, &c);
                display->drawLine(x1, y1, x2, y2, c);
                break;

            case '2':
                sscanf(s + 3, "%d,%d,%d,%d", &x, &y, &l, &c);
                display->drawFastVLine(x, y, l, c);
                break;

            case '3':
                sscanf(s + 3, "%d,%d,%d,%d", &x, &y, &l, &c);
                display->drawFastHLine(x, y, l, c);
                break;

            case '4':
                sscanf(s + 3, "%d,%d,%d,%d,%d", &x, &y, &w, &h, &c);
                display->drawRect(x, y, w, h, c);
                break;

            case '5':
                sscanf(s + 3, "%d,%d,%d,%d", &x, &y, &r, &c);
                display->drawCircle(x, y, r, c);
                break;

            case '6':
                sscanf(s + 3, "%d,%d,%d,%d,%d,%d,%d", &x1, &y1, &x2, &y2, &x3, &y3, &c);
                display->drawTriangle(x1, y1, x2, y2, x3, y3, c);
                break;

            case '7':
                sscanf(s + 3, "%d,%d,%d,%d,%d,%d", &x, &y, &w, &h, &r, &c);
                display->drawRoundRect(x, y, w, h, r, c);
                break;

            case '8':
                sscanf(s + 3, "%d,%d,%d,%d,%d", &x, &y, &w, &h, &c);
                display->fillRect(x, y, w, h, c);
                break;

            case '9':
                sscanf(s + 3, "%d,%d,%d,%d", &x, &y, &r, &c);
                display->fillCircle(x, y, r, c);
                break;

            case 'A':
                sscanf(s + 3, "%d,%d,%d,%d,%d,%d,%d", &x1, &y1, &x2, &y2, &x3, &y3, &c);
                display->fillTriangle(x1, y1, x2, y2, x3, y3, c);
                break;

            case 'B':
                sscanf(s + 3, "%d,%d,%d,%d,%d,%d", &x, &y, &w, &h, &r, &c);
                display->fillRoundRect(x, y, w, h, r, c);
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
                display->print(strTemp);
                break;

            case 'D':
                sscanf(s + 3, "%d", &c);
                display->setTextSize(c);
                break;

            case 'E':
                sscanf(s + 3, "%d,%d", &x, &y);
                display->setCursor(x, y);
                break;

            case 'F':
                sscanf(s + 3, "%c", &b);
                if (b == 'T')
                    display->setTextWrap(true);
                if (b == 'F')
                    display->setTextWrap(false);
                break;

            case 'G':
                sscanf(s + 3, "%d", &c);
                c &= 3;
                display->setRotation(c);
                break;

            case 'H':
            {
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
                r = display->sdCardInit();
                if (r)
                {
                    r = display->image.drawBitmapFromSd(strTemp, x, y);
                    String command = "#H(" + String(r) + ")*";
                    pTxCharacteristic->setValue(command.c_str());
                    pTxCharacteristic->notify();
                }
                else
                {
                    String response = "#H(-1)*";
                    pTxCharacteristic->setValue(response.c_str());
                    pTxCharacteristic->notify();
                }
                break;
            }

            case 'K':
                sscanf(s + 3, "%c", &b);
                if (b == '1')
                {
                    display->clearDisplay();
                }
                break;

            case 'L':
                sscanf(s + 3, "%c", &b);
                if (b == '1')
                {
                    display->display();
                }
                break;

            case 'M':
                sscanf(s + 3, "%d,%d,%d", &y1, &x2, &y2);
                display->display();
                break;

            case 'P':
            {
                sscanf(s + 3, "%c", &b);
                if (b == '?')
                {
                    float battery = display->readBattery();
                    String command = "#P(" + String(battery) + ")*";
                    pTxCharacteristic->setValue(command.c_str());
                    pTxCharacteristic->notify();
                }
                break;
            }
                
            case 'S':
            {
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
                r = display->sdCardInit();
                if (r)
                {
                    r = display->image.draw(strTemp, x, y);

                    String command  = "#H(";
                    command += r;
                    command += ")*\n";

                    pTxCharacteristic->setValue(command.c_str());
                    pTxCharacteristic->notify();
                }
                else
                {
                    pTxCharacteristic->setValue("#H(-1)*\n");
                    pTxCharacteristic->notify();
                }
                break;
            }

            case 'T':
                int t;
                sscanf(s + 3, "%d,%d,%d,%d,%d,%d", &x1, &y1, &x2, &y2, &c, &t);
                display->drawThickLine(x1, y1, x2, y2, c, t);
                break;

            case 'U':
                sscanf(s + 3, "%d,%d,%d,%d,%d", &rx, &ry, &xc, &yc, &c);
                display->drawElipse(rx, ry, xc, yc, c);
                break;

            case 'V':
                sscanf(s + 3, "%d,%d,%d,%d,%d", &rx, &ry, &xc, &yc, &c);
                display->fillElipse(rx, ry, xc, yc, c);
                break;

            case 'W':
                sscanf(s + 3, "%d,%d,%d", &hr, &min, &sec);
                display->rtc.setTime(hr, min, sec);
                break;

            case 'X':
                sscanf(s + 3, "%d,%d,%d,%d", &wday, &day, &mon, &yr);
                display->rtc.setDate(wday, day, mon, yr);
                break;

            case 'Y':
                sscanf(s + 3, "%d", &ep);
                display->rtc.setEpoch(ep);
                break;

            case 'Z':
            {
                sscanf(s + 3, "%c", &b);
                if (b == '?')
                {
                    String command;

                    command += "#Z(";
                    command += display->rtc.getEpoch();
                    command += ")*"; 

                    pTxCharacteristic->setValue(command.c_str());
                    pTxCharacteristic->notify();
                }
                break;
            }

            case 'a':
            {
                sscanf(s + 3, "%d", &k);
                if (k > 6)
                {
                    pTxCharacteristic->setValue("INVALID");
                    pTxCharacteristic->notify();
                    break;
                }
                display->rtc.getRtcData();
                String command;
                command += "#a(";
                switch (k)
                {
                    case 0:
                        command += String(display->rtc.getSecond());
                        break;
                    case 1:
                        command += String(display->rtc.getMinute());
                        break;
                    case 2:
                        command += String(display->rtc.getHour());
                        break;
                    case 3:
                        command += String(display->rtc.getDay());
                        break;
                    case 4:
                        command += String(display->rtc.getWeekday());
                        break;
                    case 5:
                        command += String(display->rtc.getMonth());
                        break;
                    case 6:
                        command += String(display->rtc.getYear());
                        break;
                }
                command += ")*";
                pTxCharacteristic->notify();
                break;
            }

            case 'b':
                sscanf(s + 3, "%d,%d,%d,%d,%d", &as, &am, &ah, &ad, &aw);
                display->rtc.setAlarm(as, am, ah, ad, aw);
                break;

            case 'c':
                sscanf(s + 3, "%d,%d", &ae, &amc);
                display->rtc.setAlarmEpoch(ae, amc);
                break;

            case 'd':
            {
                sscanf(s + 3, "%c", &b);
                if (b == '?')
                {
                    String command;
                    command += "#d(";
                    command += display->rtc.checkAlarmFlag();
                    command += ")*";
                    pTxCharacteristic->setValue(command.c_str());
                    pTxCharacteristic->notify();
                }
                break;
            }

            case 'e':
                sscanf(s + 3, "%c", &b);
                if (b == '1')
                {
                    display->rtc.clearAlarmFlag();
                }
                break;

            case 'f':
            {
                sscanf(s + 3, "%d", &k);
                if (k > 4)
                {
                    pTxCharacteristic->setValue("INVALID");
                    pTxCharacteristic->notify();
                    break;
                }
                {
                    String command;
                    command += "#f(";
                    switch (k)
                    {
                    case 0:
                        command += String(display->rtc.getAlarmSecond());
                        break;
                    case 1:
                        command += String(display->rtc.getAlarmMinute());
                        break;
                    case 2:
                        command += String(display->rtc.getAlarmHour());
                        break;
                    case 3:
                        command += String(display->rtc.getAlarmDay());
                        break;
                    case 4:
                        command += String(display->rtc.getAlarmWeekday());
                        break;
                    }
                    command += ")*";
                    pTxCharacteristic->setValue(command.c_str());
                    pTxCharacteristic->notify();
                }
                break;
            }
            case 'g':
                sscanf(s + 3, "%d,%d,%d,%d", &sc, &v, &ie, &ip);

                if (sc > 3)
                {
                    pTxCharacteristic->setValue("ERROR");
                    pTxCharacteristic->notify();
                    break;
                }
                display->rtc.timerSet(sc, v, ie, ip);
                break;

            case 'h':
            {
                sscanf(s + 3, "%c", &b);
                if (b == '?')
                {
                    String command;
                    command += "#h(";
                    command += display->rtc.checkTimerFlag();
                    command += ")*";
                    pTxCharacteristic->setValue(command.c_str());
                    pTxCharacteristic->notify();
                }
                break;
            }

            case 'i':
                sscanf(s + 3, "%c", &b);
                if (b == '1')
                {
                    display->rtc.clearTimerFlag();
                }
                break;

            case 'j':
                sscanf(s + 3, "%c", &b);
                if (b == '1')
                {
                    display->rtc.disableTimer();
                }
                break;

            case 'k':
            {
                sscanf(s + 3, "%c", &b);
                if (b == '?')
                {
                    String command;
                    command += "#k(";
                    command += display->rtc.isSet();
                    command += ")*";
                    pTxCharacteristic->setValue(command.c_str());
                    pTxCharacteristic->notify();
                }
                break;
            }

            case 'l':
                sscanf(s + 3, "%c", &b);
                if (b == '1')
                {
                    display->rtc.reset();
                }
                break;
            }
            *s = 0;
            *e = 0;
        }
    }
}

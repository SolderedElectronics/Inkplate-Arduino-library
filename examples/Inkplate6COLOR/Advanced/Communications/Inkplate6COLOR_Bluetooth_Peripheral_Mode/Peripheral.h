/*
   Inkplate6COLOR_Bluetooth_Peripheral_Mode example for Soldered Inkplate 6COLOR
   For this example you will need USB cable and an Inkplate 6COLOR
   Select "Soldered Inkplate 6COLOR" from Tools -> Board menu.
   Don't have "Soldered Inkplate 6COLOR" option? Follow our tutorial and add it:
   https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/

   This example shows how to use Inkplate as a peripheral device over Bluetooth.
   More about peripheral mode: https://inkplate.readthedocs.io/en/latest/peripheral-mode.html

   Upload this example to the Inkplate and connect your phone to it via Bluetooth.
   First, you have to pair the Inkplate with your phone in Bluetooth settings in your phone, then go to the
   Serial Bluetooth Terminal app and you can find the Inkplate in the device list. You can use another similar app.
   If Bluetooth starts successfully, you can send commands from your phone. Don't forget you need to send #L(1)* after
   each command to show it on the display (equal to display->display()).

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: https://forum.soldered.com/
   28 March 2023 by Soldered
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

void run(char commandBuffer[], size_t n, Inkplate *display, BluetoothSerial *SerialBT)
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
            System::rtcCountdownSrcClock sc;
            uint32_t ep, ae;
            char b;
            char temp[150];
            switch (*(s + 1))
            {
            case '?':
                SerialBT->print("OK");
                break;

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
                    r = display->drawBitmapFromSd(strTemp, x, y);
                    SerialBT->print("#H(");
                    SerialBT->print(r, DEC);
                    SerialBT->println(")*");
                    SerialBT->flush();
                }
                else
                {
                    SerialBT->println("#H(-1)*");
                    SerialBT->flush();
                }
                break;

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
                sscanf(s + 3, "%c", &b);
                if (b == '?')
                {
                    SerialBT->print("#P(");
                    SerialBT->print(display->readBattery(), 2);
                    SerialBT->println(")*");
                    SerialBT->flush();
                }
                break;

            case 'R':
                sscanf(s + 3, "%c", &b);
                if (b == '?')
                {
                    SerialBT->print("#R(");
                    SerialBT->print(display->getPanelDeepSleepState(), DEC);
                    SerialBT->println(")*");
                    SerialBT->flush();
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
                r = display->sdCardInit();
                if (r)
                {
                    r = display->drawImage(strTemp, x, y);
                    SerialBT->print("#H(");
                    SerialBT->print(r, DEC);
                    SerialBT->println(")*");
                    SerialBT->flush();
                }
                else
                {
                    SerialBT->println("#H(-1)*");
                    SerialBT->flush();
                }
                break;
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
                // sprintf(temp, "display->rtcSetTime(%d, %d, %d);\n\r", hr, min, sec);
                // SerialBT->println(temp);
                display->rtcSetTime(hr, min, sec);
                break;
            case 'X':
                sscanf(s + 3, "%d,%d,%d,%d", &wday, &day, &mon, &yr);
                // sprintf(temp, "display->rtcSetDate(%d, %d, %d, %d);\n\r", wday, day, mon, yr);
                // SerialBT->println(temp);
                display->rtcSetDate(wday, day, mon, yr);
                break;
            case 'Y':
                sscanf(s + 3, "%d", &ep);
                // sprintf(temp, "display->rtcSetEpoch(%d);\n\r", ep);
                // SerialBT->println(temp);
                display->rtcSetEpoch(ep);
                break;
            case 'Z':
                sscanf(s + 3, "%c", &b);
                if (b == '?')
                {
                    SerialBT->print("#Z(");
                    SerialBT->print(display->rtcGetEpoch());
                    SerialBT->println(")*");
                    SerialBT->flush();
                }
                break;
            case 'a':
                sscanf(s + 3, "%d", &k);
                if (k > 6)
                {
                    SerialBT->println("INVALID");
                    break;
                }
                display->rtcGetRtcData();
                SerialBT->print("#a(");
                switch (k)
                {
                case 0:
                    SerialBT->print(display->rtcGetSecond());
                    break;
                case 1:
                    SerialBT->print(display->rtcGetMinute());
                    break;
                case 2:
                    SerialBT->print(display->rtcGetHour());
                    break;
                case 3:
                    SerialBT->print(display->rtcGetDay());
                    break;
                case 4:
                    SerialBT->print(display->rtcGetWeekday());
                    break;
                case 5:
                    SerialBT->print(display->rtcGetMonth());
                    break;
                case 6:
                    SerialBT->print(display->rtcGetYear());
                    break;
                }
                SerialBT->println(")*");
                SerialBT->flush();
                break;
            case 'b':
                sscanf(s + 3, "%d,%d,%d,%d,%d", &as, &am, &ah, &ad, &aw);
                // sprintf(temp, "display->rtcSetAlarm(%d, %d, %d, %d, %d);\n\r", as, am, ah, ad, aw);
                // SerialBT->println(temp);
                display->rtcSetAlarm(as, am, ah, ad, aw);
                break;
            case 'c':
                sscanf(s + 3, "%d,%d", &ae, &amc);
                // sprintf(temp, "display->rtcSetAlarmEpoch(%d, %d);\n\r", ae, amc);
                // SerialBT->println(temp);
                display->rtcSetAlarmEpoch(ae, amc);
                break;
            case 'd':
                sscanf(s + 3, "%c", &b);
                if (b == '?')
                {
                    SerialBT->print("#d(");
                    SerialBT->print(display->rtcCheckAlarmFlag());
                    SerialBT->println(")*");
                    SerialBT->flush();
                }
                break;
            case 'e':
                sscanf(s + 3, "%c", &b);
                if (b == '1')
                {
                    // SerialBT->println("display->rtcClearAlarmFlag()");
                    display->rtcClearAlarmFlag();
                    SerialBT->flush();
                }
                break;
            case 'f':
                sscanf(s + 3, "%d", &k);
                if (k > 4)
                {
                    SerialBT->println("INVALID");
                    break;
                }
                SerialBT->print("#f(");
                switch (k)
                {
                case 0:
                    SerialBT->print(display->rtcGetAlarmSecond());
                    break;
                case 1:
                    SerialBT->print(display->rtcGetAlarmMinute());
                    break;
                case 2:
                    SerialBT->print(display->rtcGetAlarmHour());
                    break;
                case 3:
                    SerialBT->print(display->rtcGetAlarmDay());
                    break;
                case 4:
                    SerialBT->print(display->rtcGetAlarmWeekday());
                    break;
                }
                SerialBT->println(")*");
                SerialBT->flush();
                break;
            case 'g':
                sscanf(s + 3, "%d,%d,%d,%d", &sc, &v, &ie, &ip);

                if (sc > 3)
                {
                    SerialBT->println("ERROR");
                    break;
                }

                // sprintf(temp, "display->rtcTimerSet(%d, %d, %d, %d);\n\r", sc, v, ie, ip);
                // SerialBT->println(temp);
                display->rtcTimerSet(sc, v, ie, ip);
                break;
            case 'h':
                sscanf(s + 3, "%c", &b);
                if (b == '?')
                {
                    SerialBT->print("#h(");
                    SerialBT->print(display->rtcCheckTimerFlag());
                    SerialBT->println(")*");
                    SerialBT->flush();
                }
                break;
            case 'i':
                sscanf(s + 3, "%c", &b);
                if (b == '1')
                {
                    // SerialBT->println("display->rtcClearTimerFlag()");
                    display->rtcClearTimerFlag();
                }
                break;
            case 'j':
                sscanf(s + 3, "%c", &b);
                if (b == '1')
                {
                    // SerialBT->println("display->rtcDisableTimer()");
                    display->rtcDisableTimer();
                }
                break;
            case 'k':
                sscanf(s + 3, "%c", &b);
                if (b == '?')
                {
                    SerialBT->print("#k(");
                    SerialBT->print(display->rtcIsSet());
                    SerialBT->println(")*");
                    SerialBT->flush();
                }
                break;
            case 'l':
                sscanf(s + 3, "%c", &b);
                if (b == '1')
                {
                    // SerialBT->println("display->rtcReset()");
                    display->rtcReset();
                }
                break;
            }
            *s = 0;
            *e = 0;
        }
    }
}

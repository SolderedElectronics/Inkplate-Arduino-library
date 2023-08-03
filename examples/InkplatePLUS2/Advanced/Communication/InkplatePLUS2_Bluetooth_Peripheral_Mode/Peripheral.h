/*
   InkplatePLUS2_Bluetooth_Peripheral_Mode example for Soldered Inkplate PLUS2
   For this example you will need a USB-C cable, Inkplate PLUS2 and smartphone.
   Select "Soldered InkplatePLUS2" from Tools -> Board menu.
   Don't have "Soldered InkplatePLUS2" option? Follow our tutorial and add it:
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
   12 July 2023 by Soldered
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
            uint8_t hr, min, sec, wday, day, mon, k, as, am, ah, ad, aw, amc, v, br, rt[8];
            uint16_t tx1[2], ty1[2], th, tw, tx2, ty2;
            bool ie, ip;
            System::rtcCountdownSrcClock sc;
            uint32_t ep, ae, pwrs;
            char b;
            char temp[150];
            switch (*(s + 1))
            {
            case '?':
                SerialBT->print("OK");
                break;

            case '0':
                sscanf(s + 3, "%d,%d,%d", &x, &y, &c);
                // sprintf(temp, "display->drawPixel(%d, %d, %d)\n\r", x, y, c);
                // SerialBT->print(temp);
                display->drawPixel(x, y, c);
                break;

            case '1':
                sscanf(s + 3, "%d,%d,%d,%d,%d", &x1, &y1, &x2, &y2, &c);
                // sprintf(temp, "display->drawLine(%d, %d, %d, %d, %d)\n\r", x1, y1, x2, y2, c);
                // SerialBT->print(temp);
                display->drawLine(x1, y1, x2, y2, c);
                break;

            case '2':
                sscanf(s + 3, "%d,%d,%d,%d", &x, &y, &l, &c);
                // sprintf(temp, "display->drawFastVLine(%d, %d, %d, %d)\n\r", x, y, l, c);
                // SerialBT->print(temp);
                display->drawFastVLine(x, y, l, c);
                break;

            case '3':
                sscanf(s + 3, "%d,%d,%d,%d", &x, &y, &l, &c);
                // sprintf(temp, "display->drawFastHLine(%d, %d, %d, %d)\n\r", x, y, l, c);
                // SerialBT->print(temp);
                display->drawFastHLine(x, y, l, c);
                break;

            case '4':
                sscanf(s + 3, "%d,%d,%d,%d,%d", &x, &y, &w, &h, &c);
                // sprintf(temp, "display->drawRect(%d, %d, %d, %d, %d)\n\r", x, y, w, h, c);
                // SerialBT->print(temp);
                display->drawRect(x, y, w, h, c);
                break;

            case '5':
                sscanf(s + 3, "%d,%d,%d,%d", &x, &y, &r, &c);
                // sprintf(temp, "display->drawCircle(%d, %d, %d, %d)\n\r", x, y, r, c);
                // SerialBT->print(temp);
                display->drawCircle(x, y, r, c);
                break;

            case '6':
                sscanf(s + 3, "%d,%d,%d,%d,%d,%d,%d", &x1, &y1, &x2, &y2, &x3, &y3, &c);
                // sprintf(temp, "display->drawTriangle(%d, %d, %d, %d, %d, %d, %d)\n\r", x1, y1, x2, y2, x3, y3, c);
                // SerialBT->print(temp);
                display->drawTriangle(x1, y1, x2, y2, x3, y3, c);
                break;

            case '7':
                sscanf(s + 3, "%d,%d,%d,%d,%d,%d", &x, &y, &w, &h, &r, &c);
                // sprintf(temp, "display->drawRoundRect(%d, %d, %d, %d, %d, %d)\n\r", x, y, w, h, r, c);
                // SerialBT->print(temp);
                display->drawRoundRect(x, y, w, h, r, c);
                break;

            case '8':
                sscanf(s + 3, "%d,%d,%d,%d,%d", &x, &y, &w, &h, &c);
                // sprintf(temp, "display->fillRect(%d, %d, %d, %d, %d)\n\r", x, y, w, h, c);
                // SerialBT->print(temp);
                display->fillRect(x, y, w, h, c);
                break;

            case '9':
                sscanf(s + 3, "%d,%d,%d,%d", &x, &y, &r, &c);
                // sprintf(temp, "display->fillCircle(%d, %d, %d, %d)\n\r", x, y, r, c);
                // SerialBT->print(temp);
                display->fillCircle(x, y, r, c);
                break;

            case 'A':
                sscanf(s + 3, "%d,%d,%d,%d,%d,%d,%d", &x1, &y1, &x2, &y2, &x3, &y3, &c);
                // sprintf(temp, "display->fillTriangle(%d, %d, %d, %d, %d, %d, %d)\n\r", x1, y1, x2, y2, x3, y3, c);
                // SerialBT->print(temp);
                display->fillTriangle(x1, y1, x2, y2, x3, y3, c);
                break;

            case 'B':
                sscanf(s + 3, "%d,%d,%d,%d,%d,%d", &x, &y, &w, &h, &r, &c);
                // sprintf(temp, "display->fillRoundRect(%d, %d, %d, %d, %d, %d)\n\r", x, y, w, h, r, c);
                // SerialBT->print(temp);
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
                // SerialBT->print("display->print(\"");
                // SerialBT->print(strTemp);
                // SerialBT->println("\");");
                display->print(strTemp);
                break;

            case 'D':
                sscanf(s + 3, "%d", &c);
                // sprintf(temp, "display->setTextSize(%d)\n", c);
                // SerialBT->print(temp);
                display->setTextSize(c);
                break;

            case 'E':
                sscanf(s + 3, "%d,%d", &x, &y);
                // sprintf(temp, "display->setCursor(%d, %d)\n", x, y);
                // SerialBT->print(temp);
                display->setCursor(x, y);
                break;

            case 'F':
                sscanf(s + 3, "%c", &b);
                // sprintf(temp, "display->setTextWrap(%s)\n", b == 'T' ? "True" : "False");
                // SerialBT->print(temp);
                if (b == 'T')
                    display->setTextWrap(true);
                if (b == 'F')
                    display->setTextWrap(false);
                break;

            case 'G':
                sscanf(s + 3, "%d", &c);
                c &= 3;
                // sprintf(temp, "display->setRotation(%d)\n", c);
                // SerialBT->print(temp);
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
                    // sprintf(temp, "display->drawBitmap(%d, %d, %s)\n", x, y, strTemp);
                    // SerialBT->print(temp);
                }
                else
                {
                    SerialBT->println("#H(-1)*");
                    SerialBT->flush();
                }
                break;

            case 'I':
                sscanf(s + 3, "%d", &c);
                // sprintf(temp, "display->setDisplayMode(%s)\n", c == 0 ? "INKPLATE_1BIT" : "INKPLATE_3BIT");
                // SerialBT->print(temp);
                if (c == INKPLATE_1BIT)
                    display->selectDisplayMode(INKPLATE_1BIT);
                if (c == INKPLATE_3BIT)
                    display->selectDisplayMode(INKPLATE_3BIT);
                break;

            case 'J':
                sscanf(s + 3, "%c", &b);
                if (b == '?')
                {
                    // if (0 == 0) {
                    //  SerialBT->println("#J(0)*");
                    //} else {
                    //  SerialBT->println("#J(1)*");
                    //}
                    if (display->getDisplayMode() == INKPLATE_1BIT)
                    {
                        SerialBT->println("#J(0)*");
                        SerialBT->flush();
                    }
                    if (display->getDisplayMode() == INKPLATE_3BIT)
                    {
                        SerialBT->println("#J(1)*");
                        SerialBT->flush();
                    }
                }
                break;

            case 'K':
                sscanf(s + 3, "%c", &b);
                if (b == '1')
                {
                    // SerialBT->print("display->clearDisplay();\n");
                    display->clearDisplay();
                }
                break;

            case 'L':
                sscanf(s + 3, "%c", &b);
                if (b == '1')
                {
                    // SerialBT->print("display->display();\n");
                    display->display();
                }
                break;

            case 'M':
                sscanf(s + 3, "%d,%d,%d", &y1, &x2, &y2);
                // sprintf(temp, "display->partialUpdate(%d, %d, %d);\n", y1, x2, y2);
                // SerialBT->print(temp);
                display->partialUpdate();
                break;

            case 'N':
                sscanf(s + 3, "%c", &b);
                if (b == '?')
                {
                    SerialBT->print("#N(");
                    SerialBT->print(display->readTemperature(), DEC);
                    // SerialBT->print(23, DEC);
                    SerialBT->println(")*");
                    SerialBT->flush();
                }
                break;

            case 'O':
                sscanf(s + 3, "%d", &c);
                if (c >= 0 && c <= 2)
                {
                    SerialBT->print("#O(");
                    SerialBT->print(display->readTouchpad(c), DEC);
                    // SerialBT->print(0, DEC);
                    SerialBT->println(")*");
                    SerialBT->flush();
                }
                break;

            case 'P':
                sscanf(s + 3, "%c", &b);
                if (b == '?')
                {
                    SerialBT->print("#P(");
                    SerialBT->print(display->readBattery(), 2);
                    // SerialBT->print(3.54, 2);
                    SerialBT->println(")*");
                    SerialBT->flush();
                }
                break;

            case 'Q':
                sscanf(s + 3, "%d", &c);
                c &= 1;
                // if (c == 0) SerialBT->print("display->einkOff();\n");
                // if (c == 1) SerialBT->print("display->einkOn();\n");
                if (c == 0)
                    display->einkOff();
                if (c == 1)
                    display->einkOn();
                break;

            case 'R':
                sscanf(s + 3, "%c", &b);
                if (b == '?')
                {
                    SerialBT->print("#R(");
                    SerialBT->print(display->getPanelState(), DEC);
                    // SerialBT->print(1, DEC);
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
                    // sprintf(temp, "display->drawBitmap(%d, %d, %s)\n", x, y, strTemp);
                    // SerialBT->print(temp);
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
                // sprintf(temp, "display->drawLine(%d, %d, %d, %d, %d)\n\r", x1, y1, x2, y2, c);
                // SerialBT->print(temp);
                display->drawThickLine(x1, y1, x2, y2, c, t);
                break;
            case 'U':
                sscanf(s + 3, "%d,%d,%d,%d,%d", &rx, &ry, &xc, &yc, &c);
                // sprintf(temp, "display->drawLine(%d, %d, %d, %d, %d)\n\r", x1, y1, x2, y2, c);
                // SerialBT->print(temp);
                display->drawElipse(rx, ry, xc, yc, c);
                break;
            case 'V':
                sscanf(s + 3, "%d,%d,%d,%d,%d", &rx, &ry, &xc, &yc, &c);
                // sprintf(temp, "display->drawLine(%d, %d, %d, %d, %d)\n\r", x1, y1, x2, y2, c);
                // SerialBT->print(temp);
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
            case 'm':
                sscanf(s + 3, "%c", &b);
                if (b == '1')
                {
                    display->frontlight(true);
                    // SerialBT->println("display->frontlight(true);");
                }
                else
                {
                    display->frontlight(false);
                    // SerialBT->println("display->frontlight(false);");
                }
                break;
            case 'n':
                sscanf(s + 3, "%d", &br);
                // sprintf(temp, "display->setFrontlight(%d);\n\r", br);
                // SerialBT->println(temp);
                display->setFrontlight(br);
                break;
            case 'o':
                sscanf(s + 3, "%d", &pwrs);
                // sprintf(temp, "display->tsInit(%d);\n\r", pwrs);
                // SerialBT->println(temp);
                if (pwrs == 1)
                    display->tsInit(1);

                if (pwrs == 0)
                    display->tsInit(0);
                break;
            case 'p':
                sscanf(s + 3, "%c", &b);
                if (b == '1')
                {
                    // sprintf(temp, "display->tsShutdown();\n\r");
                    // SerialBT->println(temp);
                    display->tsShutdown();
                }
                break;
            case 'r':
                sscanf(s + 3, "%c", &b);
                if (b == '?')
                {
                    SerialBT->print("#r(");
                    SerialBT->print(display->tsAvailable());
                    SerialBT->println(")*");
                    SerialBT->flush();
                }
                break;
            case 's':
                sscanf(s + 3, "%c", &b);
                if (b == '?')
                {
                    if (display->tsGetData(tx1, ty1) != 0)
                    {
                        SerialBT->print("#s(");
                        SerialBT->print(tx1[0]);
                        SerialBT->print(", ");
                        SerialBT->print(ty1[0]);
                        SerialBT->println(")*");
                        SerialBT->flush();
                    }
                }
                break;
            case 't':
                sscanf(s + 3, "%c", &b);
                if (b == '?')
                {
                    display->tsGetRawData(rt);
                    for (int i = 0; i < 8; ++i)
                    {
                        SerialBT->print("Reg ");
                        SerialBT->println(rt[i], BIN);
                    }
                    SerialBT->flush();
                }
                break;
            case 'u':
                sscanf(s + 3, "%hu,%hu,%hu,%hu", &tx2, &ty2, &tw, &th);
                // sprintf(temp, "display->touchInArea(%hu, %hu, %hu, %hu);\n\r", tx2, ty2, tw, th);
                // SerialBT->println(temp);
                SerialBT->print("#u(");
                SerialBT->print(display->touchInArea(tx2, ty2, tw, th));
                SerialBT->println(")*");
                SerialBT->flush();
                break;
            }
            *s = 0;
            *e = 0;
        }
    }
}

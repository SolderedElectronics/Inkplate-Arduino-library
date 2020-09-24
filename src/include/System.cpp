/*
System.cpp
Inkplate 6 Arduino library
David Zovko, Borna Biro, Denis Vajak, Zvonimir Haramustek @ e-radionica.com
September 24, 2020
https://github.com/e-radionicacom/Inkplate-6-Arduino-library

For support, please reach over forums: forum.e-radionica.com/en
For more info about the product, please check: www.inkplate.io

This code is released under the GNU Lesser General Public License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html
Please review the LICENSE file included with this example.
If you have any questions about licensing, please contact techsupport@e-radionica.com
Distributed as-is; no warranty is given.
*/

#include "System.h"

SPIClass spi2(HSPI);
SdFat sd(&spi2);

void System::setPanelState(uint8_t s)
{
    _panelOn = s;
}

uint8_t System::getPanelState()
{
    return _panelOn;
}

void System::setTemperature(int8_t t)
{
    _temperature = t;
}

int8_t System::readTemperature()
{
    return _temperature;
}

uint8_t System::readTouchpad(uint8_t _pad)
{
    return digitalReadMCP((_pad & 3) + 10);
}

double System::readBattery()
{
    digitalWriteMCP(9, HIGH);
    delay(1);
    int adc = analogRead(35);
    digitalWriteMCP(9, LOW);
    return (double(adc) / 4095 * 3.3 * 2);
}

int16_t System::sdCardInit()
{
    spi2.begin(14, 12, 13, 15);
    setSdCardOk(sd.begin(15, SD_SCK_MHZ(25)));
    return getSdCardOk();
}

SdFat System::getSdFat()
{
    return sd;
}

SPIClass System::getSPI()
{
    return spi2;
}

void System::setSdCardOk(int16_t s)
{
    _sdCardOk = s;
}

int16_t System::getSdCardOk()
{
    return _sdCardOk;
}
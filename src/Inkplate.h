/*
Inkplate.h
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

#ifndef __INKPLATE_H__
#define __INKPLATE_H__

#include "Arduino.h"

#include "SPI.h"
#include "include/Graphics.h"
#include "include/System.h"
#include "libs/SdFat/SdFat.h"

#include "include/defines.h"

extern SPIClass spi2;
extern SdFat sd;

class Inkplate : public System, public Graphics
{
  public:
    Inkplate(uint8_t _mode);
    void begin(void); // In boards
    void clearDisplay();
    void display();
    void preloadScreen();
    void partialUpdate(bool _forced = false);
    void einkOn();
    void einkOff();
    uint8_t readPowerGood();
    void cleanFast(uint8_t c, uint8_t rep);

    bool joinAP(const char *ssid, const char *pass)
    {
        return NetworkClient::joinAP(ssid, pass);
    }
    void disconnect()
    {
        NetworkClient::disconnect();
    };
    bool isConnected()
    {
        return NetworkClient::isConnected();
    };
    int _getRotation()
    {
        return Graphics::getRotation();
    };

  private:
    void precalculateGamma(uint8_t *c, float gamma);

    void display1b();
    void display3b();
    void vscan_start();
    void vscan_end();
    void hscan_start(uint32_t _d = 0);
    void pinsZstate();
    void pinsAsOutputs();

    uint32_t pinLUT[256];
    uint32_t *GLUT;
    uint32_t *GLUT2;

    uint8_t _beginDone = 0;

    const uint8_t waveform3Bit[8][9] = WAVEFORM3BIT;
};

#endif

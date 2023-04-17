/**
 **************************************************
 *
 * @file        Inkplate.h
 * @brief       Basic funtions for controling inkplate
 *
 *              https://github.com/e-radionicacom/Inkplate-Arduino-library
 *              For support, please reach over forums: forum.e-radionica.com/en
 *              For more info about the product, please check: www.inkplate.io
 *
 *              This code is released under the GNU Lesser General Public
 *License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the
 *LICENSE file included with this example. If you have any questions about
 *licensing, please contact techsupport@e-radionica.com Distributed as-is; no
 *warranty is given.
 *
 * @authors     @ Soldered
 ***************************************************/

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

#if defined(ARDUINO_INKPLATE10) || defined(ARDUINO_INKPLATE10V2)
struct waveformData
{
    uint8_t header = 'W';
    uint8_t waveformId;
    uint8_t waveform[8][9];
    uint8_t temp = 20;
    uint8_t checksum;
};
#endif

/**
 * @brief       Base class for inkplate functionalities
 */
class Inkplate : public System, public Graphics
{
  public:
#if defined(ARDUINO_INKPLATECOLOR) || defined(ARDUINO_INKPLATE2) || defined(ARDUINO_INKPLATE4) ||                      \
    defined(ARDUINO_INKPLATE7)
    Inkplate();
    void display(void);
#else
    Inkplate(uint8_t _mode);
    void display(bool leaveOn = false);
#endif

#if defined(ARDUINO_INKPLATE10) || defined(ARDUINO_INKPLATE10V2)
    void changeWaveform(uint8_t *_wf);
    uint8_t calculateChecksum(struct waveformData _w);
    bool getWaveformFromEEPROM(struct waveformData *_w);
    void burnWaveformToEEPROM(struct waveformData _w);
#endif
    bool begin(void); // In boards

    void clearDisplay();
    // void writeRow(uint8_t data);
    uint32_t partialUpdate(bool _forced = false, bool leaveOn = false);

#if defined(ARDUINO_INKPLATECOLOR) || defined(ARDUINO_INKPLATE4) || defined(ARDUINO_INKPLATE7)
    void clean();

    // These 4 functions need to refactored because conflicting functionalities
    void setPanelState(bool _state);
    bool getPanelState();
    void setPanelDeepSleep(bool _state);
    bool getPanelDeepSleepState();

    void setIOExpanderForLowPower();
#elif defined(ARDUINO_INKPLATE2)
    void clean();

    // These 4 functions need to refactored because conflicting functionalities
    void setPanelState(bool _state);
    bool getPanelState();
    void setPanelDeepSleep(bool _state);
    bool getPanelDeepSleepState();
#else
    int einkOn();
    void einkOff();
    void preloadScreen();
    uint8_t readPowerGood();
    void clean(uint8_t c, uint8_t rep);
#endif

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


#if defined(ARDUINO_INKPLATECOLOR) || defined(ARDUINO_INKPLATE2) || defined(ARDUINO_INKPLATE4) ||                      \
    defined(ARDUINO_INKPLATE7)
    bool _panelState = false;

    void resetPanel();
    void sendCommand(uint8_t _command);
    void sendData(uint8_t *_data, int _n);
    void sendData(uint8_t _data);
#else


#if defined(ARDUINO_INKPLATE10) || defined(ARDUINO_INKPLATE10V2)
    void calculateLUTs();
#endif
    void display1b(bool leaveOn = false);
    void display3b(bool leaveOn = false);

    void vscan_start();
    void vscan_end();
    void hscan_start(uint32_t _d = 0);
    void pinsZstate();
    void pinsAsOutputs();

    uint32_t pinLUT[256];
    uint32_t *GLUT;
    uint32_t *GLUT2;
#endif

    uint8_t _beginDone = 0;

#if defined(ARDUINO_INKPLATE10) || defined(ARDUINO_INKPLATE10V2)
    struct waveformData waveformEEPROM;
#endif

#ifdef WAVEFORM3BIT
    uint8_t waveform3Bit[8][9] = WAVEFORM3BIT;
#endif

#if defined(ARDUINO_INKPLATE2) || defined(ARDUINO_INKPLATE4) || defined(ARDUINO_INKPLATE7)
    bool waitForEpd(uint16_t _timeout);
#endif

#if defined(ARDUINO_INKPLATE7)
    void ePaper_Sleep();
    void ePaper_Wake();
#endif
};

#endif

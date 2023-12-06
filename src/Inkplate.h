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

#ifdef ARDUINO_INKPLATE4TEMPERA
#include "include/Buzzer.h"
#include "libs/APDS9960/src/APDS9960-SOLDERED.h"
#include "libs/BME680/src/BME680-SOLDERED.h"
#include "libs/BQ27441/src/BQ27441-G1-SOLDERED.h"
#include "libs/LSM6DS3/src/LSM6DS3-SOLDERED.h"
#endif

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
    bool begin(void);
    void clearDisplay();

#if !defined(ARDUINO_INKPLATECOLOR) || !defined(ARDUINO_INKPLATE4) || !defined(ARDUINO_INKPLATE7) ||                   \
    !defined(ARDUINO_INKPLATE2)
    uint32_t partialUpdate(bool _forced = false, bool leaveOn = false);
    int einkOn();
    void einkOff();
    void preloadScreen();
    uint8_t readPowerGood();
    void clean(uint8_t c, uint8_t rep);
#endif
#if defined(ARDUINO_INKPLATECOLOR)
    void clean();
#endif

    bool connectWiFi(const char *ssid, const char *pass, int timeout = WIFI_TIMEOUT, bool printToSerial = false)
    {
        return NetworkClient::connectWiFi(ssid, pass, timeout, printToSerial);
    };
    bool connectWiFiMulti(int numNetworks, const char **ssids, const char **passwords, int timeout = WIFI_TIMEOUT,
                          bool printToSerial = false)
    {
        return NetworkClient::connectWiFiMulti(numNetworks, ssids, passwords, timeout, printToSerial);
    };
    void setFollowRedirects(followRedirects_t follow)
    {
        NetworkClient::setFollowRedirects(follow);
    };
    void disconnect()
    {
        NetworkClient::disconnect();
    };
    bool isConnected()
    {
        return NetworkClient::isConnected();
    };
    // The default parameters for nptServer here are cast to (char*) to keep the compiler happy
    bool getNTPEpoch(time_t *timeEpoch, int timeZone = 0, char *ntpServer = (char *)"pool.ntp.org",
                     int daylightSavingsOffsetHours = 0)
    {
        return NetworkClient::getNTPEpoch(timeEpoch, timeZone, ntpServer, daylightSavingsOffsetHours);
    }
    bool getNTPDateTime(tm *dateTime, int timeZone = 0, char *ntpServer = (char *)"pool.ntp.org",
                        int daylightSavingsOffsetHours = 0)
    {
        return NetworkClient::getNTPDateTime(dateTime, timeZone, ntpServer, daylightSavingsOffsetHours);
    }
    int _getRotation()
    {
        return Graphics::getRotation();
    };

#ifdef ARDUINO_INKPLATE4TEMPERA
    Buzzer buzzer;            // Buzzer
    APDS_9960 apds9960;       // Gesture sensor
    BME680 bme688;            // Temperature, pressure and humidity sensor
    Soldered_LSM6DS3 lsm6ds3; // Accelerometer/Gyroscope
    BQ_27441 battery;         // Fuel gauge
    void wakePeripheral(uint8_t _peripheral);
    void sleepPeripheral(uint8_t _peripheral);
#endif

#ifdef ARDUINO_INKPLATECOLOR
    void sleepColorPanel();
#endif

#if defined(ARDUINO_INKPLATE6PLUS) || defined(ARDUINO_INKPLATE6PLUSV2)
    void setInkplatePowerMode(uint8_t _mode);
#endif

  private:
    void precalculateGamma(uint8_t *c, float gamma);


#if defined(ARDUINO_INKPLATECOLOR) || defined(ARDUINO_INKPLATE2) || defined(ARDUINO_INKPLATE4) ||                      \
    defined(ARDUINO_INKPLATE7)
    void resetPanel();
    void sendCommand(uint8_t _command);
    void sendData(uint8_t *_data, int _n);
    void sendData(uint8_t _data);
    bool setPanelDeepSleep(bool _state);
    void setIOExpanderForLowPower();
#endif

#if defined(ARDUINO_INKPLATE10) || defined(ARDUINO_INKPLATE10V2)
    void calculateLUTs();
#endif

#if !defined(ARDUINO_INKPLATECOLOR) || !defined(ARDUINO_INKPLATE4) || !defined(ARDUINO_INKPLATE7) ||                   \
    !defined(ARDUINO_INKPLATE2)

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
    void ePaperSleep();
    void ePaperWake();
#endif

#if defined(ARDUINO_INKPLATE6PLUS) || defined(ARDUINO_INKPLATE6PLUSV2)
    uint8_t pwrMode = INKPLATE_NORMAL_PWR_MODE;
#endif

};

#endif
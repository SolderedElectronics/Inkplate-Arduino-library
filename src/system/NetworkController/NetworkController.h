/**
 **************************************************
 * @file        NetworkController.h
 * @brief       Basic funtions for working with network
 *
 *              https://github.com/SolderedElectronics/Inkplate-Arduino-library
 *              For more info about the product, please check: https://docs.soldered.com/inkplate/
 *
 *              This code is released under the GNU Lesser General Public License v3.0:
 *              https://www.gnu.org/licenses/lgpl-3.0.en.html Please review the LICENSE file
 *              included with this example. If you have any questions about licensing, please
 *              contact assistance@soldered.com Distributed as-is; no warranty is given.
 *
 * @authors     @ Soldered
 ***************************************************/

#ifndef __NETWORKCLIENT_H__
#define __NETWORKCLIENT_H__

#include "Arduino.h"
#include "HTTPClient.h"
#include "WiFi.h"
#include "WiFiClientSecure.h"
#include "WiFiMulti.h"
#include "time.h"
#include <string.h>

#define WIFI_TIMEOUT 23

/**
 * @brief       NetworkController class that holds standard functions for working
 * with network
 */
class NetworkController
{
  public:
    bool connectWiFi(const char *ssid, const char *pass, int timeout = WIFI_TIMEOUT, bool printToSerial = false);
    bool connectWiFiMulti(int numNetworks, const char **ssids, const char **passwords, int timeout = WIFI_TIMEOUT,
                          bool printToSerial = false);
    void disconnect();
    bool isConnected();

    uint8_t *downloadFile(const char *url, int32_t *defaultLen);
    uint8_t *downloadFileHTTPS(const char *url, int32_t *defaultLen);
    uint8_t *downloadFile(WiFiClient *url, int32_t len);
    void applyHttpsCertificate(const char *certificate);

    // The default parameters for nptServer here are cast to (char*) to keep the compiler happy
    bool getNTPEpoch(time_t *timeEpoch, int timeZone = 0, char *ntpServer = (char *)"pool.ntp.org",
                     int daylightSavingsOffsetHours = 0);
    bool getNTPDateTime(tm *timeEpoch, int timeZone = 0, char *ntpServer = (char *)"pool.ntp.org",
                        int daylightSavingsOffsetHours = 0);

    void setFollowRedirects(followRedirects_t f);

    void setHTTPSPort(uint16_t port);

    uint16_t getHTTPSPort();


  private:
    WiFiClientSecure *client;
    uint16_t _httpsPort = 443;
    followRedirects_t followRedirects;
    char *getHostFromURL(const char *urlToGetHostFrom);
    char *getPathToResourceFromURL(const char *urlToGetPathToResourceFrom);
    char *certificate;
};

#endif
/**
 * Network.h
 * Inkplate Arduino Library
 * December 21, 2022
 * https://github.com/e-radionicacom/Inkplate-6-Arduino-library
 * For support, please reach over forums: forum.e-radionica.com/en
 * For more info about the product, please check: www.inkplate.io
 * This code is released under the GNU Lesser General Public License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html
 * Please review the LICENSE file included with this example.
 * If you have any questions about licensing, please contact techsupport@e-radionica.com
 * Distributed as-is; no warranty is given.
 */

#include "ArduinoJson.h"
#include "Inkplate.h"
#include <WiFiClientSecure.h>

#ifndef NETWORK_H
#define NETWORK_H

class Network
{
  public:
    // Functions we can access in main file
    void begin(char *ssid, char *pass);
    void getData(int *backers, int *pledged);
    void setTime(struct tm *timeinfo, int timeZone);

  private:
};

#endif

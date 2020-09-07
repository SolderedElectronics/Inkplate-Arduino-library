#ifndef __NETWORK_H__
#define __NETWORK_H__

#include "Arduino.h"
#include "HTTPClient.h"
#include "WiFi.h"

class Network
{
  public:
    bool joinAP(char *ssid, char *pass);
    void disconnect();
    bool isConnected();

    bool downloadFile(uint8_t *buffer, const char *url);

  private:
};

#endif
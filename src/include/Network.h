#ifndef __NETWORK_H__
#define __NETWORK_H__

#include "Arduino.h"
#include "HTTPClient.h"
#include "WiFi.h"
#include "defines.h"

struct bitmapHeader
{
    uint16_t signature;
    uint32_t fileSize;
    uint32_t startRAW;
    uint32_t dibHeaderSize;
    uint32_t width;
    uint32_t height;
    uint16_t color;
    uint32_t compression;
};

class Network
{
  public:
    bool joinAP(const char *ssid, const char *pass);
    void disconnect();
    bool isConnected();

    uint8_t *downloadFile(const char *url, int32_t defaultLen);

  private:
};

#endif
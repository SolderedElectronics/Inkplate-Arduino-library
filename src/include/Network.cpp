#include "Network.h"

bool Network::joinAP(char *ssid, char *pass)
{
    WiFi.mode(WIFI_MODE_STA);
    WiFi.begin(ssid, pass);

    int cnt = 0;
    while (!isConnected())
    {
        if (cnt > 15)
            return 0;
        delay(1000);
        ++cnt;
    }

    return 1;
}

void Network::disconnect()
{
    WiFi.mode(WIFI_OFF);
}

bool Network::isConnected()
{
    return WiFi.status() == WL_CONNECTED;
}

bool Network::downloadFile(uint8_t *buffer, const char *url)
{
    if (!isConnected())
        return 0;

    int ret = 0;
    uint8_t *bufferPtr = buffer;

    bool sleep = WiFi.getSleep();
    WiFi.setSleep(false);

    HTTPClient http;
    http.getStream().setNoDelay(true);
    http.getStream().setTimeout(1);
    http.begin(url);

    int httpCode = http.GET();
    if (httpCode == 200)
    {
        int total = http.getSize();
        int len = total;

        uint8_t buff[128] = {0};

        WiFiClient *stream = http.getStreamPtr();

        while (http.connected() && (len > 0 || len == -1))
        {
            size_t size = stream->available();
            if (size)
            {
                int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));

                memcpy(bufferPtr, buff, c);
                bufferPtr += c;

                if (len > 0)
                    len -= c;
            }
            yield();
        }
    }

    http.end();
    WiFi.setSleep(sleep);
    return ret;
}

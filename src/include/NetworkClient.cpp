#include "NetworkClient.h"

bool NetworkClient::joinAP(const char *ssid, const char *pass)
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

void NetworkClient::disconnect()
{
    WiFi.mode(WIFI_OFF);
}

bool NetworkClient::isConnected()
{
    return WiFi.status() == WL_CONNECTED;
}


uint8_t *NetworkClient::downloadFile(const char *url, int32_t *defaultLen)
{
    if (!isConnected())
        return NULL;

    bool sleep = WiFi.getSleep();
    WiFi.setSleep(false);

    HTTPClient http;
    http.getStream().setNoDelay(true);
    http.getStream().setTimeout(1);
    http.begin(url);

    int httpCode = http.GET();

    int32_t size = http.getSize();
    if (size == -1)
        size = *defaultLen;
    else
        *defaultLen = size;

    uint8_t *buffer = (uint8_t *)ps_malloc(size);
    uint8_t *buffPtr = buffer;

    if (httpCode == HTTP_CODE_OK)
    {
        int32_t total = http.getSize();
        int32_t len = total;

        uint8_t buff[128] = {0};

        WiFiClient *stream = http.getStreamPtr();
        while (http.connected() && (len > 0 || len == -1))
        {
            size_t size = stream->available();

            if (size)
            {
                int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
                memcpy(buffPtr, buff, c);

                if (len > 0)
                    len -= c;
                buffPtr += c;
            }
            yield();
        }
    }
    http.end();
    WiFi.setSleep(sleep);

    return buffer;
}

uint8_t *NetworkClient::downloadFile(WiFiClient *s, int32_t len)
{
    if (!isConnected())
        return NULL;

    bool sleep = WiFi.getSleep();
    WiFi.setSleep(false);

    uint8_t *buffer = (uint8_t *)ps_malloc(len);
    uint8_t *buffPtr = buffer;

    uint8_t buff[128] = {0};

    while (len > 0)
    {
        size_t size = s->available();
        if (size)
        {
            int c = s->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
            memcpy(buffPtr, buff, c);

            if (len > 0)
                len -= c;
            buffPtr += c;
        }
        yield();
    }

    WiFi.setSleep(sleep);

    return buffer;
}
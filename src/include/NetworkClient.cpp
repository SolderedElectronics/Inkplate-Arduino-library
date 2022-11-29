/**
 **************************************************
 * @file        NetworkClient.cpp
 * @brief       Basic funtions for working with network
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
 * @authors     @ e-radionica.com
 ***************************************************/

#include "NetworkClient.h"

/**
 * @brief       joinAP function sets and connects inkplate it on wifi network
 *
 * @param       char *ssid
 *              pointer to network name
 * @param       char *pass
 *              pointer to password
 *
 * @return      true if successful, false if failed
 *
 * @note        usual wait time is max 23 seconds for fail
 */
bool NetworkClient::joinAP(const char *ssid, const char *pass)
{
    WiFi.mode(WIFI_MODE_STA);
    WiFi.begin(ssid, pass);

    delay(3000);
    Serial.print("Connecting to wifi.");
    int cnt = 0;
    while (!isConnected())
    {
        Serial.print(".");
        if (cnt > 20)
        {
            Serial.println();
            return 0;
        }
        delay(1000);
        ++cnt;
    }

    Serial.println();
    return 1;
}

/**
 * @brief       disconnect function turns of inkplate wifi mode
 */
void NetworkClient::disconnect()
{
    WiFi.mode(WIFI_OFF);
}

/**
 * @brief       isConnected function checks wifi status
 *
 * @return      true if connected, false if not
 */
bool NetworkClient::isConnected()
{
    return WiFi.status() == WL_CONNECTED;
}

/**
 * @brief       Download a file via https
 *
 * @param       char *url
 *              pointer to URL link that holds file
 * @param       uint32_t *defaultLen
 *              pointer that holds assumed length of file in bytes, will be
 * checked before download
 *
 * @return      pointer to buffer that holds downloaded file
 */
uint8_t *NetworkClient::downloadFileHTTPS(const char *url, int32_t *defaultLen)
{
    if (!isConnected())
    {
        return NULL;
    }

    WiFiClientSecure client; // For HTTPS
    client.setInsecure();    // Use HTTPS but don't compare certificate
    client.flush();
    client.setTimeout(10);

    Serial.println("Success????");

    bool sleep = WiFi.getSleep();
    WiFi.setSleep(false);

    HTTPClient http;
    http.getStream().setNoDelay(true);
    http.getStream().setTimeout(1);

    http.begin(client, url); // HTTP client initialization

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

        uint8_t buff[512] = {0};

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
            else if (len == -1)
            {
                len = 0;
            }
        }
    }

    http.end();
    client.stop();
    WiFi.setSleep(sleep);

    return buffer;
}

/**
 * @brief       downloadFile function downloads file from wificlient url object
 *
 * @param       WifiClient *s
 *              pointer to WifiClient object that holds data about URL
 * @param       int32_t len
 *              holds assumed length of file in bytes, will be checked before
 * download
 *
 * @return      pointer to buffer that holds downloaded file
 */
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

        uint8_t buff[512] = {0};

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
            else if (len == -1)
            {
                len = 0;
            }
        }
    }
    http.end();
    WiFi.setSleep(sleep);

    return buffer;
}
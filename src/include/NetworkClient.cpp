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
 * @brief       Get the hostname from a given URL. Used in downloadFile functions.
 *
 * @note        For "https://www.test.example.com/resource/123" returns "www.test.example.com"
 *
 * @param       const char * urlToGetHostFrom
 *              Char array which contains the full url
 *
 * @return      Char * string of host name
 */
char *NetworkClient::getHostFromURL(const char *urlToGetHostFrom)
{
    // Check if url is HTTP or HTTPS
    int offsetToGetToBeginningOfHost = 0;

    if (urlToGetHostFrom[4] == 's')
    {
        offsetToGetToBeginningOfHost = 8;
    }
    else if (urlToGetHostFrom[4] == ':')
    {
        offsetToGetToBeginningOfHost = 7;
    }

    // Get the URL without http/https
    const char *urlWithoutHTTPS = &urlToGetHostFrom[offsetToGetToBeginningOfHost];

    // Find where the host name ends
    char *p;
    const char *resourceBegin = "/"; // Host name ends at the first '/'
    p = strstr(urlWithoutHTTPS, resourceBegin);
    int indexOfCharWhenHostEnds = (int)(p - urlWithoutHTTPS);

    // Make the host substring
    char * host = new char[indexOfCharWhenHostEnds];
    memcpy(host, urlWithoutHTTPS, indexOfCharWhenHostEnds);
    host[indexOfCharWhenHostEnds] = '\0'; // Add null terminator

    return host;
}


/**
 * @brief       Get the path to resource from a given URL. Used in downloadFile functions.
 *
 * @note        For "https://www.test.example.com/resource/123" returns "/resource/123"
 *
 * @param       char * urlToGetPathToResourceFrom
 *              Char array which contains the full url
 *
 * @return      Char * string of resource string
 */
char *NetworkClient::getPathToResourceFromURL(const char *urlToGetPathToResourceFrom)
{
    // Check if url is HTTP or HTTPS
    int offsetToGetToBeginningOfHost = 0;

    if (urlToGetPathToResourceFrom[4] == 's')
    {
        offsetToGetToBeginningOfHost = 8;
    }
    else if (urlToGetPathToResourceFrom[4] == ':')
    {
        offsetToGetToBeginningOfHost = 7;
    }

    // Get the URL without http/https
    const char *urlWithoutHTTPS = &urlToGetPathToResourceFrom[offsetToGetToBeginningOfHost];

    // Find where the host name ends
    char *p;
    const char *resourceBegin = "/";
    p = strstr(urlWithoutHTTPS, resourceBegin);
    int indexOfCharWhenHostEnds = (int)(p - urlWithoutHTTPS);
    int resourceLen = strlen(urlWithoutHTTPS) - indexOfCharWhenHostEnds;

    // Make new pathToResource string
    char *pathToResource = new char[resourceLen];
    memcpy(pathToResource, urlWithoutHTTPS + indexOfCharWhenHostEnds, resourceLen);
    pathToResource[resourceLen] = '\0'; // Add null terminator

    return pathToResource;
}


/**
 * @brief       Download a file via https
 *
 * @param       char *url
 *              pointer to URL link that holds file
 *
 * @param       uint32_t *defaultLen
 *              Pointer that holds assumed length of file in bytes, will be
 *              checked before download
 *
 * @return      pointer to buffer that holds downloaded file
 */
uint8_t *NetworkClient::downloadFileHTTPS(const char *url, int32_t *defaultLen)
{
    if (!isConnected())
        return NULL;

    // Get Host name and path to resource from URL
    char *host = getHostFromURL(url);
    char *pathToResource = getPathToResourceFromURL(url);

    // Create a new WiFiClientSecure for a new connection
    client = (WiFiClientSecure *)ps_malloc(sizeof(WiFiClientSecure));
    client = new WiFiClientSecure();
    client->setInsecure(); // Use HTTPS but don't check cert
    client->setHandshakeTimeout(1500);
    client->setTimeout(1500);

    // Connect
    if (!client->connect(host, 443))
    {
        Serial.println(F("WiFiClientSecure connect Error!"));
    }

    // Remember sleep state and then wake up
    bool sleep = WiFi.getSleep();
    WiFi.setSleep(false);

    // Create a new HTTP client and connect using HTTPS
    HTTPClient http;
    //http.getStream().setNoDelay(true);
    http.getStream().setNoDelay(true);
    http.getStream().setTimeout(1000);

    if (!http.begin(*client, host, 443, pathToResource, true))
    {
        Serial.println("HTTPS begin Error!");
    }

    // Make GET
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
        // Read data and store in buffer

        int32_t total = size;
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

    // End connection
    
    http.end();
    client->stop();
    client->~WiFiClientSecure();
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
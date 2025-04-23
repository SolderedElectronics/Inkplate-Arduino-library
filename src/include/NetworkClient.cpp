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
 * @authors     @ Soldered
 ***************************************************/

#include "NetworkClient.h"

/**
 * @brief       Connects Inkplate to a provided WiFi network.
 *
 * @param       char *ssid
 *              Char array of the SSID of the network
 *
 * @param       char *pass
 *              Char array of the password of the WiFi network
 *
 * @param       bool printToSerial
 *              Set to 'True' if you want to print debug messages to serial while connecting.
 *              False by default.
 *              Serial must be initiated beforehand (Serial.begin())!
 *
 * @param       int timeout
 *              Timeout in seconds for connecting to WiFi
 *              23 seconds by default
 *
 * @return      True if successful, false if failed
 */
bool NetworkClient::connectWiFi(const char *ssid, const char *pass, int timeout, bool printToSerial)
{
    // Init WiFi
    WiFi.mode(WIFI_MODE_STA);

    // Connect to WiFi
    if (printToSerial)
    {
        Serial.println();
        Serial.print("Connecting to WiFi network ");
        Serial.print(ssid);
        Serial.println("...");
    }
    WiFi.begin(ssid, pass);
    delay(1000);

    // Check if we're connected for the amount of time set by the timeout
    int cnt = 0;
    while (!isConnected())
    {
        if (printToSerial)
            Serial.print(".");

        if (cnt > timeout)
        {
            // We went over the timeout
            // Connection unsuccessful!
            if (printToSerial)
            {
                Serial.println();
                Serial.print("Connecting to WiFi network ");
                Serial.print(ssid);
                Serial.println(" failed!");
            }
            return false;
        }
        delay(1000);
        ++cnt;
    }

    if (printToSerial)
    {
        Serial.print("Successfully connected to WiFi network ");
        Serial.print(ssid);
        Serial.println("!");
    }

    return true;
}

/**
 * @brief       Connects Inkplate to a list of provided WiFi networks. The one with the strongest signal will be chosen
 * to connect.
 *
 * @param       int numNetworks
 *              The number of different networks you want to use for WiFiMulti
 *
 * @param       char **ssid
 *              Array of char arrays which contain SSID names of multiple WiFi networks
 *
 * @param       char **pass
 *              Array of char arrays which contain passwords of multiple WiFi networks
 *
 * @param       bool printToSerial
 *              Set to 'True' if you want to print debug messages to serial while connecting.
 *              False by default.
 *              Serial must be initiated beforehand (Serial.begin())!
 *
 * @param       int timeout
 *              Timeout in seconds for connecting to WiFi
 *              23 seconds by default
 *
 * @return      True if successful, false if failed
 */
bool NetworkClient::connectWiFiMulti(int numNetworks, const char **ssids, const char **passwords, int timeout,
                                     bool printToSerial)
{
    // Create the WiFiMulti object
    WiFiMulti wifiMulti;

    // Init WiFi
    WiFi.mode(WIFI_MODE_STA);

    // Start to connect to WiFi via WiFimulti
    if (printToSerial)
    {
        Serial.println();
        Serial.println("Connecting to WiFi (via WiFiMulti)...");
    }

    // Add the different networks to WiFiMulti
    for (int i = 0; i < numNetworks; i++)
    {
        if (printToSerial)
        {
            Serial.print("Adding WiFi network ");
            Serial.print(ssids[i]);
            Serial.println("...");
        }

        if (!wifiMulti.addAP(ssids[i], passwords[i]))
        {
            if (printToSerial)
            {
                Serial.print("Couldn't add network ");
                Serial.print(ssids[i]);
                Serial.println("!");
            }
            return false;
        }
    }

    // Connect to strongest available defined network
    if (wifiMulti.run(timeout * 1000) != WL_CONNECTED)
    {
        if (printToSerial)
        {
            Serial.println("Connecting to WiFi via WiFiMulti failed!");
        }
        return false;
    }

    if (printToSerial)
    {
        Serial.print("Successfully connected to WiFi network ");
        Serial.print(WiFi.SSID());
        Serial.println("!");
    }

    return true;
}

/**
 * @brief       Turns off Inkplate's WiFi mode
 */
void NetworkClient::disconnect()
{
    WiFi.mode(WIFI_OFF);
}

/**
 * @brief       Checks WiFi status
 *
 * @returns     True if connected, False if not
 */
bool NetworkClient::isConnected()
{
    return WiFi.status() == WL_CONNECTED;
}

/**
 * @brief       Get the epoch of the current time via a specified NTP server along with other options
 *
 * @param       time_t * timeEpoch
 *              Pointer to where to save the recieved time
 *
 * @param       int timeZone
 *              The specified time zone, default is UTC.
 *              For example, 2 is UTC+2 hours. -7 is UTC -7 hours.
 *
 * @param       char *ntpServer
 *              The specified ntpServer to use. Default is "pool.ntp.org".
 *              Check ntppool.org for a list of available servers.
 *
 * @param       int daylightSavingsOffsetHours
 *              An additional offset used for daylight savings.
 *              Default is 0.
 *
 * @return      True if successful, false if failed
 */
bool NetworkClient::getNTPEpoch(time_t *timeEpoch, int timeZone, char *ntpServer, int daylightSavingsOffsetHours)
{
    // If WiFi is not connected, we can't get the epoch
    if (!isConnected())
    {
        return false;
    }

    // Configure getting the time from NTP
    // Add timeZone offset, daylight savings offset if required
    // Also set the specified NTP server
    int GMTOffsetSeconds = timeZone * 3600;
    int daylightSavingsOffsetSeconds = daylightSavingsOffsetHours * 3600;
    configTime(GMTOffsetSeconds, daylightSavingsOffsetSeconds, ntpServer);

    // Get the time using ESP32 API
    struct tm timeInfo;
    if (!getLocalTime(&timeInfo))
    {
        // Something went wrong when getting the time
        return false;
    }

    // Convert to time_t
    time_t time = mktime(&timeInfo);

    // Save the time to the specified pointer
    *timeEpoch = time;

    return true;
}

/**
 * @brief       Get the current date and time via a specified NTP server along with other options
 *
 * @param       tm * dateTime
 *              Pointer to where to save the recieved date time
 *
 * @param       int timeZone
 *              The specified time zone, default is UTC.
 *              For example, 2 is UTC+2 hours. -7 is UTC -7 hours.
 *
 * @param       char *ntpServer
 *              The specified ntpServer to use. Default is "pool.ntp.org".
 *              Check ntppool.org for a list of available servers.
 *
 * @param       int daylightSavingsOffsetHours
 *              An additional offset used for daylight savings.
 *              Default is 0.
 *
 * @return      True if successful, false if failed
 */
bool NetworkClient::getNTPDateTime(tm *dateTime, int timeZone, char *ntpServer, int daylightSavingsOffsetHours)
{
    // If WiFi is not connected, we can't get the epoch
    if (!isConnected())
    {
        return false;
    }

    // Configure getting the time from NTP
    // Add timeZone offset, daylight savings offset if required
    // Also set the specified NTP server
    int GMTOffsetSeconds = timeZone * 3600;
    int daylightSavingsOffsetSeconds = daylightSavingsOffsetHours * 3600;
    configTime(GMTOffsetSeconds, daylightSavingsOffsetSeconds, ntpServer);

    // Get the time using ESP32 API
    struct tm timeInfo;
    if (!getLocalTime(&timeInfo))
    {
        // Something went wrong when getting the time
        return false;
    }

    // Save the dateTime
    *dateTime = timeInfo;
    return true;
}

/**
 * @brief       Get the hostname from a given URL. Used in downloadFile functions.
 *
 * @note        For "https://www.test.example.com/resource/123" returns "www.test.example.com"
 *
 * @param       const char * urlToGetHostFrom
 *              Char array which contains the full URL
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
    char *host = new char[indexOfCharWhenHostEnds];
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
 *              Char array which contains the full URL
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
 * @brief       Download a file via HTTPS
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
    if (this->certificate)
    {
        client->setCACert(certificate); // Save the certificate if the user has defined it
    }
    else
    {
        client->setInsecure(); // Use HTTPS but don't check cert
    }

    client->setHandshakeTimeout(1500);
    client->setTimeout(1500);

    // Connect
    client->connect(host, 443);

    // Remember sleep state and then wake up
    bool sleep = WiFi.getSleep();
    WiFi.setSleep(false);

    // Create a new HTTP client
    HTTPClient http;
    http.getStream().setNoDelay(true);
    http.getStream().setTimeout(1000);

    // Set the 'follow redirects' option
    // Default is HTTPC_DISABLE_FOLLOW_REDIRECTS
    http.setFollowRedirects(followRedirects);

    // Connect with HTTPS
    http.begin(*client, host, 443, pathToResource, true);

    // Make GET request
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

/**
 * @brief       Download a file via HTTP
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
uint8_t *NetworkClient::downloadFile(const char *url, int32_t *defaultLen)
{
    if (!isConnected())
        return NULL;

    bool sleep = WiFi.getSleep();
    WiFi.setSleep(false);

    HTTPClient http;
    http.getStream().setNoDelay(true);
    http.getStream().setTimeout(1);

    // Set the 'follow redirects' option
    // Default is HTTPC_DISABLE_FOLLOW_REDIRECTS
    http.setFollowRedirects(followRedirects);

    // Connect with HTTP
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

/**
 * @brief       Set if Inkplate should follow redirects when making HTTP requests
 *
 * @param       followRedirects_t f, the various settings are:
 *              -'HTTPC_DISABLE_FOLLOW_REDIRECTS' - no redirection will be followed (default)
 *              -'HTTPC_STRICT_FOLLOW_REDIRECTS' - strict RFC2616, only requests using
 *              GET or HEAD methods will be redirected
 *              -'HTTPC_FORCE_FOLLOW_REDIRECTS' - all redirections will be followed
 *
 * @returns     None
 *
 */
void NetworkClient::setFollowRedirects(followRedirects_t f)
{
    this->followRedirects = f;
}

/**
 * @brief       Applies a certificate that will be checked when communicating with a website
 *
 * @param       const char* certificate, the certificate in a string format
 *
 * @returns     None
 *
 */
void NetworkClient::applyCertificate(const char *certificate)
{
    this->certificate = strdup(certificate);
}

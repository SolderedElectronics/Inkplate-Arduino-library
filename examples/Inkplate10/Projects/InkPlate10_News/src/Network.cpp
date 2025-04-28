#include "Network.h"

String Network::fetchData(String url)
{
    HTTPClient http;
    http.begin(url);
    int httpCode = http.GET();

    if (httpCode > 0)
    {
        return http.getString();
    }
    else
    {
        return "";
    }
}
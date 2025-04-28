#pragma once

#include <Arduino.h>
#include <HTTPClient.h>

class Network
{
public:
    String fetchData(String url);
};
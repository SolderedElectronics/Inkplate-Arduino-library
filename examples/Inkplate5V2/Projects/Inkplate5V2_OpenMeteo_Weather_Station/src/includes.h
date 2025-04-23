#ifndef INCLUDES_H
#define INCLUDES_H

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Inkplate.h>

// custom classes
#include "Network.h"
#include "WeatherData.h"
#include "Gui.h"

// font
#include "fonts/FreeSans9pt7b.h"
#include "fonts/FreeSans12pt7b.h"
#include "fonts/FreeSans18pt7b.h"
#include "fonts/FreeSansBold24pt7b.h"

// all the weather icons
#include "binary_Icons/icon_s_gray.h" 
#include "binary_Icons/icon_s_thermometer.h" 
#include "binary_Icons/icon_s_clear_sky.h" 
#include "binary_Icons/icon_s_fog.h" 
#include "binary_Icons/icon_s_rain.h" 
#include "binary_Icons/icon_s_partly_cloudy.h" 
#include "binary_Icons/icon_s_snow.h" 
#include "binary_Icons/icon_s_storm.h" 
#include "binary_Icons/icon_s_moon.h" 
// all the battery icons
#include "binary_Icons/icon_s_low_battery.h"
#include "binary_Icons/icon_s_half_battery.h"
#include "binary_Icons/icon_s_high_battery.h"
#include "binary_Icons/icon_s_full_battery.h"

#endif
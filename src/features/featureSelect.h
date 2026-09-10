#ifndef _FEATURE_SELECT_H
#define _FEATURE_SELECT_H

#if defined(ARDUINO_INKPLATE6V2) || defined(ARDUINO_INKPLATE5V2) || defined(ARDUINO_INKPLATE13SPECTRA) ||              \
    defined(ARDUINO_ESP32S3_DEV) || defined(ARDUINO_INKPLATE5) || defined(ARDUINO_INKPLATE10V2)
#include "SdFat/SdFat.h"
#include "rtc/rtc.h"
#elif defined(ARDUINO_INKPLATECOLOR) || defined(ARDUINO_INKPLATE6) || defined(ARDUINO_INKPLATE10)
#include "SdFat/SdFat.h"
#include "rtc/rtc.h"
#include "touchpad/touchpad.h"
#elif defined(ARDUINO_INKPLATE6FLICK)
#include "SdFat/SdFat.h"
#include "rtc/rtc.h"
#include "touchscreen/touchCypress/TouchCypress.h"
#include "frontlight/Frontlight.h"
#elif defined(ARDUINO_INKPLATE6PLUS) || defined(ARDUINO_INKPLATE6PLUSV2)
#include "SdFat/SdFat.h"
#include "rtc/rtc.h"
#include "frontlight/Frontlight.h"
#include "touchscreen/touchElan/touchElan.h"
#elif defined(ARDUINO_INKPLATE4TEMPERA)
#include "SdFat/SdFat.h"
#include "rtc/rtc.h"
#include "frontlight/Frontlight.h"
#include "touchscreen/touchElan/touchElan.h"
#include "APDS9960/APDS9960-SOLDERED.h"
#include "BME680/BME680-SOLDERED.h"
#include "BQ27441/BQ27441-G1-SOLDERED.h"
#include "Buzzer/Buzzer.h"
#include "LSM6DS3/LSM6DS3-SOLDERED.h"
#endif
#endif

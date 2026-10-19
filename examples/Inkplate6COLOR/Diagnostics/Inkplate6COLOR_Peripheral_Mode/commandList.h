#ifndef __COMMAND_LIST_H__
#define __COMMAND_LIST_H__

#define CMD_PING  0x0001
// Commands from 0x01 to 0x0A are reserved for future use. Command 0x00 is invalid!

#define CMD_DRAW_PIXEL                  0x000A
#define CMD_DRAW_LINE                   0x000B
#define CMD_DRAW_FASTVLINE              0x000C
#define CMD_DRAW_FASTHLINE              0x000D
#define CMD_DRAW_RECT                   0x000E
#define CMD_DRAW_CIRCLE                 0x000F
#define CMD_DRAW_TRIANGLE               0x0010
#define CMD_DRAW_ROUND_RECT             0x0011
#define CMD_DRAW_THICK_LINE             0x0012
#define CMD_DRAW_ELIPSE                 0x0013
#define CMD_FILL_RECT                   0x0014
#define CMD_FILL_CIRCLE                 0x0015
#define CMD_FILL_TRIANGLE               0x0016
#define CMD_FILL_ROUND_RECT             0x0017
#define CMD_FILL_ELIPSE                 0x0018
#define CMD_PRINT                       0x0019
#define CMD_SET_TEXT_SIZE               0x001A
#define CMD_SET_TEXT_COLOR              0x001B
#define CMD_SET_CURSOR                  0x001C
#define CMD_SET_TEXT_WRAP               0x001D
#define CMD_SET_ROTATION                0x001E
#define CMD_SD_CARD_INIT                0x001F
#define CMD_SD_CARD_SLEEP               0x0020
#define CMD_DRAW_BITMAP                 0x0021
#define CMD_DRAW_IMAGE                  0x0022
#define CMD_SET_MODE                    0x0023
#define CMD_GET_MODE                    0x0024#ifndef __COMMAND_LIST_H__
#define __COMMAND_LIST_H__

#define CMD_PING  0x0001
// Commands from 0x01 to 0x0A are reserved for future use. Command 0x00 is invalid!

#define CMD_DRAW_PIXEL                  0x000A
#define CMD_DRAW_LINE                   0x000B
#define CMD_DRAW_FASTVLINE              0x000C
#define CMD_DRAW_FASTHLINE              0x000D
#define CMD_DRAW_RECT                   0x000E
#define CMD_DRAW_CIRCLE                 0x000F
#define CMD_DRAW_TRIANGLE               0x0010
#define CMD_DRAW_ROUND_RECT             0x0011
#define CMD_DRAW_THICK_LINE             0x0012
#define CMD_DRAW_ELIPSE                 0x0013
#define CMD_FILL_RECT                   0x0014
#define CMD_FILL_CIRCLE                 0x0015
#define CMD_FILL_TRIANGLE               0x0016
#define CMD_FILL_ROUND_RECT             0x0017
#define CMD_FILL_ELIPSE                 0x0018
#define CMD_PRINT                       0x0019
#define CMD_SET_TEXT_SIZE               0x001A
#define CMD_SET_TEXT_COLOR              0x001B
#define CMD_SET_CURSOR                  0x001C
#define CMD_SET_TEXT_WRAP               0x001D
#define CMD_SET_ROTATION                0x001E
#define CMD_SD_CARD_INIT                0x001F
#define CMD_SD_CARD_SLEEP               0x0020
#define CMD_DRAW_BITMAP                 0x0021
#define CMD_DRAW_IMAGE                  0x0022
#define CMD_SET_MODE                    0x0023
#define CMD_GET_MODE                    0x0024
#define CMD_CLEAR_DISPLAY               0x0025
#define CMD_DISPLAY                     0x0026
#define CMD_PARTIAL_UPDATE              0x0027
#define CMD_READ_TEMP                   0x0028
#define CMD_READ_TOUCHPAD               0x0029
#define CMD_READ_BATTERY                0x002A
#define CMD_EINK_PMIC                   0x002B
#define CMD_RTC_SET_TIME                0x002C
#define CMD_RTC_SET_DATE                0x002D
#define CMD_RTC_SET_EPOCH               0x002E
#define CMD_RTC_UPDATE                  0x002F
#define CMD_RTC_GET_DATA_ALL            0x0030
#define CMD_RTC_SET_ALARM               0x0031
#define CMD_RTC_SET_ALARM_EPOCH         0x0032
#define CMD_RTC_CLEAR_AL_FLAG           0x0033
#define CMD_RTC_GET_ALARM_ALL           0x0034
#define CMD_RTC_SET_TIMER               0x0035
#define CMD_RTC_GET_TIMER_FLAG          0x0036
#define CMD_RTC_CLEAR_TIMER_FLAG        0x0037
#define CMD_RTC_DISABLE_TIMER           0x0038
#define CMD_RTC_IS_SET                  0x0039
#define CMD_RTC_RESET                   0x003A
#define CMD_ESP32_DEEPSLEEP             0x003B
#define CMD_ESP32_LIGHTSLEEP            0x003C
#define CMD_TOUCH_INIT                  0x003D
#define CMD_TOUCH_AVAILABLE             0x003E
#define CMD_TOUCH_GET_DATA              0x003F
#define CMD_DRAW_IMAGE_BUFFER           0x0040
#define CMD_CONNECT_WIFI                0x0041
#define CMD_DISCONNECT_WIFI             0x0042
#define CMD_GET_REQUEST                 0x0043
#define CMD_POST_REQUEST                0x0044
// #define CMD_                            0x0042
// #define CMD_                            0x0043
// #define CMD_                            0x0044
// #define CMD_                            0x0045
// #define CMD_                            0x0046
// #define CMD_                            0x0047
#define CMD_CLEAR_DISPLAY               0x0025
#define CMD_DISPLAY                     0x0026
#define CMD_PARTIAL_UPDATE              0x0027
#define CMD_READ_TEMP                   0x0028
#define CMD_READ_TOUCHPAD               0x0029
#define CMD_READ_BATTERY                0x002A
#define CMD_RTC_SET_TIME                0x002C
#define CMD_RTC_SET_DATE                0x002D
#define CMD_RTC_SET_EPOCH               0x002E
#define CMD_RTC_UPDATE                  0x002F
#define CMD_RTC_GET_DATA_ALL            0x0030
#define CMD_RTC_SET_ALARM               0x0031
#define CMD_RTC_SET_ALARM_EPOCH         0x0032
#define CMD_RTC_CLEAR_AL_FLAG           0x0033
#define CMD_RTC_GET_ALARM_ALL           0x0034
#define CMD_RTC_SET_TIMER               0x0035
#define CMD_RTC_GET_TIMER_FLAG          0x0036
#define CMD_RTC_CLEAR_TIMER_FLAG        0x0037
#define CMD_RTC_DISABLE_TIMER           0x0038
#define CMD_RTC_IS_SET                  0x0039
#define CMD_RTC_RESET                   0x003A
#define CMD_ESP32_DEEPSLEEP             0x003B
#define CMD_ESP32_LIGHTSLEEP            0x003C
#define CMD_TOUCH_INIT                  0x003D
#define CMD_TOUCH_AVAILABLE             0x003E
#define CMD_TOUCH_GET_DATA              0x003F
#define CMD_DRAW_IMAGE_BUFFER           0x0040
#define CMD_CONNECT_WIFI                0x0041
#define CMD_DISCONNECT_WIFI             0x0042
#define CMD_GET_REQUEST                 0x0043
#define CMD_POST_REQUEST                0x0044
// #define CMD_                            0x0042
// #define CMD_                            0x0043
// #define CMD_                            0x0044
// #define CMD_                            0x0045
// #define CMD_                            0x0046
// #define CMD_                            0x0047

#endif

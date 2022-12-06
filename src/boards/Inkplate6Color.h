/**
 **************************************************
 *
 * @file        InkplateColor.h
 * @brief       Basic funtions for controling Inkplate 6COLOR
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

#ifndef INKPLATECOLOR_H
#define INKPLATECOLOR_H

#define SD_PMOS_PIN 12

// Connections between ESP32 and color Epaper
#define EPAPER_RST_PIN  19
#define EPAPER_DC_PIN   33
#define EPAPER_CS_PIN   27
#define EPAPER_BUSY_PIN 32
#define EPAPER_CLK      18
#define EPAPER_DIN      23

// Timeout for init of epaper (1.5 sec in this case)
#define INIT_TIMEOUT 1500

// Epaper registers
#define PANEL_SET_REGISTER          0x00
#define POWER_SET_REGISTER          0x01
#define POWER_OFF_SEQ_SET_REGISTER  0x03
#define POWER_OFF_REGISTER          0x04
#define BOOSTER_SOFTSTART_REGISTER  0x06
#define DEEP_SLEEP_REGISTER         0x07
#define DATA_START_TRANS_REGISTER   0x10
#define DATA_STOP_REGISTER          0x11
#define DISPLAY_REF_REGISTER        0x12
#define IMAGE_PROCESS_REGISTER      0x13
#define PLL_CONTROL_REGISTER        0x30
#define TEMP_SENSOR_REGISTER        0x40
#define TEMP_SENSOR_EN_REGISTER     0x41
#define TEMP_SENSOR_WR_REGISTER     0x42
#define TEMP_SENSOR_RD_REGISTER     0x43
#define VCOM_DATA_INTERVAL_REGISTER 0x50
#define LOW_POWER_DETECT_REGISTER   0x51
#define RESOLUTION_SET_REGISTER     0x61
#define STATUS_REGISTER             0x71
#define VCOM_VALUE_REGISTER         0x81
#define VCM_DC_SET_REGISTER         0x02

// Epaper resolution and colors
#define E_INK_WIDTH     600
#define E_INK_HEIGHT    448
#define INKPLATE_BLACK  0b00000000
#define INKPLATE_WHITE  0b00000001
#define INKPLATE_GREEN  0b00000010
#define INKPLATE_BLUE   0b00000011
#define INKPLATE_RED    0b00000100
#define INKPLATE_YELLOW 0b00000101
#define INKPLATE_ORANGE 0b00000110

// PCAL6416A Register map
#define IO_INT_ADDR 0x20
#define IO_EXT_ADDR 0x20

#endif

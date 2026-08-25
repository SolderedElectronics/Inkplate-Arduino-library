#ifndef __INKPLATE13_PINS_H__
#define __INKPLATE13_PINS_H__
// Header guard for the Arduino include
#ifdef ARDUINO_INKPLATE13SPECTRA
#include "soc/gpio_reg.h"
#include "soc/gpio_struct.h"
#define IO_INT_ADDR 0x20

// Color display of the spectra panel
static uint32_t pallete[] = {0x000000, 0xFFFFFF, 0xFFFF00, 0xFF0000, 0x0000FF, 0x00FF00};

// Pin on the internal io expander which controls MOSFET for turning on and off the SD card
#define SD_PMOS_PIN IO_PIN_B2 // 10


// Pinout definition - should be hard-coded.
#define SPECTRA133_DC_PIN    14
#define SPECTRA133_CS_M_PIN  42
#define SPECTRA133_RST_PIN   4
#define SPECTRA133_BUSYN_PIN 7
#define SPECTRA133_SPI_MOSI  40
#define SPECTRA133_SPI_MISO  41
#define SPECTRA133_SPI_SCK   38
#define SPECTRA133_CS_S_PIN  39
#define SPECTRA133_PWR_EN    21
#define SPECTRA133_BS0       6
#define SPECTRA133_BS1       5

// Define the default Spectra6 color indexes.
#define INKPLATE_BLACK  0
#define INKPLATE_WHITE  1
#define INKPLATE_YELLOW 2
#define INKPLATE_RED    3
#define INKPLATE_BLUE   4
#define INKPLATE_GREEN  5

static uint16_t colorPalette[6] = {INKPLATE_BLACK, INKPLATE_WHITE, INKPLATE_YELLOW,
                                   INKPLATE_RED,   0x05,  0x06};


// Screen resolution.
#define E_INK_WIDTH  1200
#define E_INK_HEIGHT 1600

// Register addresses
#define SPECTRA133_REGISTER_PSR             0x00
#define SPECTRA133_REGISTER_PWR             0x01
#define SPECTRA133_REGISTER_POF             0x02
#define SPECTRA133_REGISTER_PON             0x04
#define SPECTRA133_REGISTER_BTST_N          0x05
#define SPECTRA133_REGISTER_BTST_P          0x06
#define SPECTRA133_REGISTER_DTM             0x10
#define SPECTRA133_REGISTER_DRF             0x12
#define SPECTRA133_REGISTER_PLL             0x30
#define SPECTRA133_REGISTER_TSC             0x40
#define SPECTRA133_REGISTER_CDI             0x50
#define SPECTRA133_REGISTER_TCON            0x60
#define SPECTRA133_REGISTER_TRES            0x61
#define SPECTRA133_REGISTER_PTLW            0x83
#define SPECTRA133_REGISTER_AN_TM           0x74
#define SPECTRA133_REGISTER_AGID            0x86
#define SPECTRA133_REGISTER_BUCK_BOOST_VDDN 0xB0
#define SPECTRA133_REGISTER_TFT_VCOM_POWER  0xB1
#define SPECTRA133_REGISTER_EN_BUF          0xB6
#define SPECTRA133_REGISTER_BOOST_VDDP_EN   0xB7
#define SPECTRA133_REGISTER_CCSET           0xE0
#define SPECTRA133_REGISTER_PWS             0xE3
#define SPECTRA133_REGISTER_CMD66           0xF0

// Register Values
static uint8_t SPECTRA133_REGISTER_PSR_V[2] = {0xDF, 0x6B};
static uint8_t SPECTRA133_REGISTER_PWR_V[6] = {0x0F, 0x00, 0x28, 0x2C, 0x28, 0x38};
static uint8_t SPECTRA133_REGISTER_POF_V[1] = {0x00};
static uint8_t SPECTRA133_REGISTER_DRF_V[1] = {0x00};
static uint8_t SPECTRA133_REGISTER_PLL_V[1] = {0x08};
static uint8_t SPECTRA133_REGISTER_CDI_V[1] = {0xF7};
static uint8_t SPECTRA133_REGISTER_TCON_V[2] = {0x03, 0x03};
static uint8_t SPECTRA133_REGISTER_TRES_V[4] = {0x04, 0xB0, 0x03, 0x20};
static uint8_t SPECTRA133_REGISTER_CMD66_V[6] = {0x49, 0x55, 0x13, 0x5D, 0x05, 0x10};
static uint8_t SPECTRA133_REGISTER_EN_BUF_V[1] = {0x07};
static uint8_t SPECTRA133_REGISTER_CCSET_V[1] = {0x01};
static uint8_t SPECTRA133_REGISTER_PWS_V[1] = {0x22};
static uint8_t SPECTRA133_REGISTER_AN_TM_V[9] = {0xC0, 0x1C, 0x1C, 0xCC, 0xCC, 0xCC, 0x15, 0x15, 0x55};
static uint8_t SPECTRA133_REGISTER_AGID_V[1] = {0x10};
static uint8_t SPECTRA133_REGISTER_BTST_P_V[2] = {0xD8, 0x18};
static uint8_t SPECTRA133_REGISTER_BOOST_VDDP_EN_V[1] = {0x01};
static uint8_t SPECTRA133_REGISTER_BTST_N_V[2] = {0xD8, 0x18};
static uint8_t SPECTRA133_REGISTER_BUCK_BOOST_VDDN_V[1] = {0x01};
static uint8_t SPECTRA133_REGISTER_TFT_VCOM_POWER_V[1] = {0x02};

#endif
#endif

#ifndef __INKPLATE7SPECTRA_PINS_H__
#define __INKPLATE7SPECTRA_PINS_H__
// Header guard for the Arduino include
#ifdef ARDUINO_ESP32S3_DEV
#include "soc/gpio_reg.h"
#include "soc/gpio_struct.h"
#define IO_INT_ADDR 0x20

// Color display of the spectra panel
static uint32_t pallete[] = {0x000000, 0xFFFFFF, 0xFFFF00, 0xFF0000, 0x0000FF, 0x00FF00};

// Pin on the internal io expander which controls MOSFET for turning on and off the SD card
#define SD_PMOS_PIN IO_PIN_B2 // 10

// Pinout definition - should be hard-coded.
#define SPECTRA73_DC_PIN    14
#define SPECTRA73_CS_PIN    42
#define SPECTRA73_RST_PIN   4
#define SPECTRA73_BUSYN_PIN 7
#define SPECTRA73_SPI_MOSI  40
#define SPECTRA73_SPI_SCK   38
#define SPECTRA73_PWR_EN    21
#define SPECTRA73_BS0       6
#define SPECTRA73_BS1       5

// microSD card SPI pins.
#define SPECTRA73_SD_SPI_CLK  12
#define SPECTRA73_SD_SPI_MISO 13
#define SPECTRA73_SD_SPI_MOSI 11
#define SPECTRA73_SD_SPI_CS   10

// Battery measurement pins.
#define SPECTRA73_BATT_MOSFET_PIN 9 // Pin on the internal io expander
#define SPECTRA73_BATT_ADC_PIN    1 /

// Define the default Spectra6 color indexes.
#define INKPLATE_BLACK  0
#define INKPLATE_WHITE  1
#define INKPLATE_YELLOW 2
#define INKPLATE_RED    3
#define INKPLATE_BLUE   4
#define INKPLATE_GREEN  5

static uint16_t colorPalette[6] = {INKPLATE_BLACK, INKPLATE_WHITE, INKPLATE_YELLOW,
                                   INKPLATE_RED,   5,  6};


// Screen resolution.
#define E_INK_WIDTH  800
#define E_INK_HEIGHT 480

// Register addresses
#define SPECTRA73_REGISTER_PSR    0x00
#define SPECTRA73_REGISTER_PWR    0x01
#define SPECTRA73_REGISTER_POF    0x02
#define SPECTRA73_REGISTER_PFS    0x03
#define SPECTRA73_REGISTER_PON    0x04
#define SPECTRA73_REGISTER_BTST1  0x05
#define SPECTRA73_REGISTER_BTST2  0x06
#define SPECTRA73_REGISTER_DSLP   0x07
#define SPECTRA73_REGISTER_BTST3  0x08
#define SPECTRA73_REGISTER_DTM    0x10
#define SPECTRA73_REGISTER_DRF    0x12
#define SPECTRA73_REGISTER_IPC    0x13
#define SPECTRA73_REGISTER_PLL    0x30
#define SPECTRA73_REGISTER_TSE    0x41
#define SPECTRA73_REGISTER_CDI    0x50
#define SPECTRA73_REGISTER_TCON   0x60
#define SPECTRA73_REGISTER_TRES   0x61
#define SPECTRA73_REGISTER_VDCS   0x82
#define SPECTRA73_REGISTER_T_VDCS 0x84
#define SPECTRA73_REGISTER_AGID   0x86
#define SPECTRA73_REGISTER_CCSET  0xE0
#define SPECTRA73_REGISTER_PWS    0xE3
#define SPECTRA73_REGISTER_TSSET  0xE6
#define SPECTRA73_REGISTER_CMDH   0xAA

// Register Values
static uint8_t SPECTRA73_REGISTER_CMDH_V[6] = {0x49, 0x55, 0x20, 0x08, 0x09, 0x18};
static uint8_t SPECTRA73_REGISTER_PWR_V[6] = {0x3F, 0x00, 0x32, 0x2A, 0x0E, 0x2A};
static uint8_t SPECTRA73_REGISTER_PSR_V[2] = {0x5F, 0x69};
static uint8_t SPECTRA73_REGISTER_PFS_V[4] = {0x00, 0x54, 0x00, 0x44};
static uint8_t SPECTRA73_REGISTER_BTST1_V[4] = {0x40, 0x1F, 0x1F, 0x2C};
static uint8_t SPECTRA73_REGISTER_BTST2_V[4] = {0x6F, 0x1F, 0x16, 0x25};
static uint8_t SPECTRA73_REGISTER_BTST3_V[4] = {0x6F, 0x1F, 0x1F, 0x22};
static uint8_t SPECTRA73_REGISTER_IPC_V[2] = {0x00, 0x04};
static uint8_t SPECTRA73_REGISTER_PLL_V[1] = {0x02};
static uint8_t SPECTRA73_REGISTER_TSE_V[1] = {0x00};
static uint8_t SPECTRA73_REGISTER_CDI_V[1] = {0x3F};
static uint8_t SPECTRA73_REGISTER_TCON_V[2] = {0x02, 0x00};
static uint8_t SPECTRA73_REGISTER_TRES_V[4] = {0x03, 0x20, 0x01, 0xE0};
static uint8_t SPECTRA73_REGISTER_VDCS_V[1] = {0x1E};
static uint8_t SPECTRA73_REGISTER_T_VDCS_V[1] = {0x01};
static uint8_t SPECTRA73_REGISTER_AGID_V[1] = {0x00};
static uint8_t SPECTRA73_REGISTER_PWS_V[1] = {0x2F};
static uint8_t SPECTRA73_REGISTER_CCSET_V[1] = {0x00};
static uint8_t SPECTRA73_REGISTER_TSSET_V[1] = {0x00};
static uint8_t SPECTRA73_REGISTER_POF_V[1] = {0x00};
static uint8_t SPECTRA73_REGISTER_DRF_V[1] = {0x00};

#endif
#endif

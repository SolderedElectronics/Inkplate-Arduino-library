#ifndef __INKPLATE31_PINS_H__
#define __INKPLATE31_PINS_H__
// Header guard for the Arduino include
#ifdef ARDUINO_INKPLATE31SPECTRA
#include "soc/gpio_reg.h"
#include "soc/gpio_struct.h"

// I/O expander I2C addresses.
// Expander1 carries every panel-power-switch/CS line used by this driver.
// Expander2 is present on the board but not wired to anything by this driver yet -
// it's begin()'d at its address so it doesn't sit in a floating/unknown state, but no
// pins on it are used. See CLAUDE.md.
#define IO_INT_ADDR      0x20
#define IO_INT_ADDR_EXP2 0x21

// Color display of the spectra panel (same Spectra 6 ink as Inkplate13SPECTRA).
static uint32_t pallete[] = {0x000000, 0xFFFFFF, 0xFFFF00, 0xFF0000, 0x0000FF, 0x00FF00};

// Define the default Spectra6 color indexes.
#define INKPLATE_BLACK  0
#define INKPLATE_WHITE  1
#define INKPLATE_YELLOW 2
#define INKPLATE_RED    3
#define INKPLATE_BLUE   5
#define INKPLATE_GREEN  6

static uint16_t colorPalette[6] = {INKPLATE_BLACK, INKPLATE_WHITE, INKPLATE_YELLOW,
                                   INKPLATE_RED,   INKPLATE_BLUE,  INKPLATE_GREEN};

// Screen resolution. Landscape-native (unlike Inkplate13SPECTRA/Inkplate2, which are
// portrait-native panels rotated at the driver level).
#define E_INK_WIDTH  2560
#define E_INK_HEIGHT 1440

//======================================================================================
// Expander1 pin map (PCAL6416A @ IO_INT_ADDR).
//======================================================================================
// Chip-select lines for the 8 source driver ICs. Bit position on the expander does NOT
// match the logical CS index (PCB routing reverses each nibble) - use csExpanderBit[]
// below to translate logical CS 0..7 into the expander bit to toggle.
#define SPECTRA315_EXP_CS3 IO_PIN_A0 // 0
#define SPECTRA315_EXP_CS2 IO_PIN_A1 // 1
#define SPECTRA315_EXP_CS1 IO_PIN_A2 // 2
#define SPECTRA315_EXP_CS0 IO_PIN_A3 // 3
#define SPECTRA315_EXP_CS7 IO_PIN_A4 // 4
#define SPECTRA315_EXP_CS6 IO_PIN_A5 // 5
#define SPECTRA315_EXP_CS5 IO_PIN_A6 // 6
#define SPECTRA315_EXP_CS4 IO_PIN_A7 // 7

// Logical CS index (0..7, matches the panel's own CS0..CS7 / left-to-right chip order)
// -> expander bit. See CLAUDE.md for how this was derived from the schematic.
static const uint8_t csExpanderBit[8] = {3, 2, 1, 0, 7, 6, 5, 4};

#define SPECTRA315_EXP_VBAT_MOS    IO_PIN_B1 // 9
#define SPECTRA315_EXP_SD_ENABLE   IO_PIN_B2 // 10
#define SPECTRA315_EXP_PMIC_PWR_EN IO_PIN_B3 // 11 - enables external 12V buck/boost feeding the IST9201
#define SPECTRA315_EXP_IST9201_EN  IO_PIN_B4 // 12 - IST9201 chip enable (vendor sample's PMIC_EN)
#define SPECTRA315_EXP_VDDP_EN     IO_PIN_B5 // 13
#define SPECTRA315_EXP_VDDN_EN     IO_PIN_B6 // 14
#define SPECTRA315_EXP_VCNP_EN     IO_PIN_B7 // 15

//======================================================================================
// ESP32-S3 direct GPIO pin map.
//======================================================================================
// NOTE: the board's pin list names both IO4/IO5 ("SPI1_D_CX"/"SPI2_D_CX") and
// IO9/IO11/IO13/IO14 ("SI3"/"SI0"/"SI1"/"SI2") in a way that looks like the same quad-SPI
// data signals labeled twice. The SI0-SI3 names match the IST9201/panel datasheet
// directly, so those are used here as the authoritative quad-SPI data pins. If real
// hardware bring-up shows IO4/IO5 are the correct data lines instead, only the four
// defines below need to change.
#define SPECTRA315_BUSY_PIN    6 // DRIVER_BUSY_N, input
#define SPECTRA315_RST_PIN     7 // DRIVER_RST_N, output
#define SPECTRA315_I2C_SDA     8
#define SPECTRA315_SD_CS       10
#define SPECTRA315_SPI_D0      11 // SI0 (quad data 0 / MOSI role)
#define SPECTRA315_SPI_SCK     12 // SPI_SCL (panel SPI clock)
#define SPECTRA315_SPI_D1      13 // SI1 (quad data 1 / MISO role)
#define SPECTRA315_SPI_D2      14 // SI2 (quad data 2 / WP role)
#define SPECTRA315_SPI_D3      9  // SI3 (quad data 3 / HD role)
#define SPECTRA315_SD_MOSI     15
#define SPECTRA315_SD_SCK      16
#define SPECTRA315_EXP1_INT    17 // Shared with WAKE_BTN on this board.
#define SPECTRA315_I2C_SCL     21
#define SPECTRA315_TOGGLE_SEL1 38 // PMIC
#define SPECTRA315_TOGGLE_SEL0 39 // PMIC
#define SPECTRA315_IST9201_PS  40
#define SPECTRA315_IST9201_PG  41
#define SPECTRA315_SD_MISO     42
#define SPECTRA315_VBAT_ADC    1

//======================================================================================
// Panel chip geometry.
//======================================================================================
// The 2560x1440 panel is driven by 8 source-driver ICs in a single row, left to right.
// Each chip's addressable RAM is a fixed 400 (host pixels) x 1440, 4bpp packed
// (400*1440/2 = 288000 bytes), regardless of how much of that width is actually wired to
// visible pixels. Chips 3 and 7 only drive their first 80 columns to real pixels - the
// remaining 320 columns of RAM exist but aren't connected to anything.
// Source: E Ink "EL315TW1 Partial Window Update" app note, confirmed against the
// product's 2560 total width (400*3+80 = 1280 per half, x2 = 2560).
#define SPECTRA315_CHIP_COUNT        8
#define SPECTRA315_CHIP_RAM_WIDTH    400
#define SPECTRA315_CHIP_HEIGHT       1440
#define SPECTRA315_CHIP_BUFFER_BYTES (SPECTRA315_CHIP_RAM_WIDTH * SPECTRA315_CHIP_HEIGHT / 2) // 288000

// Per-chip visible widths: {400, 400, 400, 80, 400, 400, 400, 80}. chipXOffset below is
// the cumulative sum - the global X coordinate where each chip's visible region starts.
// chipXOffset[8] (2560) is the total panel width, used as a sentinel.
static const uint16_t chipXOffset[SPECTRA315_CHIP_COUNT + 1] = {0, 400, 800, 1200, 1280, 1680, 2080, 2480, 2560};

//======================================================================================
// Panel command bytes (register addresses) - from EL315TW1.h.
//======================================================================================
#define SPECTRA315_CMD_PSR          0x00
#define SPECTRA315_CMD_PWR          0x01
#define SPECTRA315_CMD_POF          0x02
#define SPECTRA315_CMD_POFS         0x03
#define SPECTRA315_CMD_PON          0x04
#define SPECTRA315_CMD_DTM          0x10
#define SPECTRA315_CMD_DRF          0x12
#define SPECTRA315_CMD_PLL          0x30
#define SPECTRA315_CMD_TSC          0x40
#define SPECTRA315_CMD_CDI          0x50
#define SPECTRA315_CMD_TCON         0x60
#define SPECTRA315_CMD_TRES         0x61
#define SPECTRA315_CMD_PTLW         0x83
#define SPECTRA315_CMD_DCDC_TM      0xA5
#define SPECTRA315_CMD_CMD66        0xF0
#define SPECTRA315_CMD_VCOM_WOUT_EN 0xB4
#define SPECTRA315_CMD_EN_BUF       0xB6
#define SPECTRA315_CMD_TM_TCON      0xD2
#define SPECTRA315_CMD_CCSET        0xE0
#define SPECTRA315_CMD_PWS          0xE3
#define SPECTRA315_CMD_SPIM         0xE6
#define SPECTRA315_CMD_STATUS       0xF2 // used by checkDriverICStatus()
#define SPECTRA315_CMD_READ_PWR     0x9B
#define SPECTRA315_CMD_READ_VCOM    0x8A

#define SPECTRA315_PTLW_ENABLE  0x01
#define SPECTRA315_PTLW_DISABLE 0x00

//======================================================================================
// Panel register values (vendor magic values) - from EL315TW1.c.
//======================================================================================
static uint8_t SPECTRA315_PSR_V[2] = {0xD3, 0x69};
static uint8_t SPECTRA315_PWR_V[6] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
static uint8_t SPECTRA315_POF_V[1] = {0x01};
static uint8_t SPECTRA315_POFS_MV[4] = {0x00, 0xC0, 0x00, 0xA4};
static uint8_t SPECTRA315_POFS_SV[4] = {0x00, 0xC0, 0x00, 0x95};
static uint8_t SPECTRA315_DRF_V[1] = {0x00};
static uint8_t SPECTRA315_CDI_V[1] = {0xF7};
static uint8_t SPECTRA315_PLL_V[1] = {0x08};
static uint8_t SPECTRA315_TCON_V[2] = {0x03, 0x03};
static uint8_t SPECTRA315_TRES_V[4] = {0x03, 0x20, 0x02, 0xD0};
static uint8_t SPECTRA315_DCDC_TM_V[3] = {0x44, 0x54, 0x00};
static uint8_t SPECTRA315_CMD66_V[6] = {0x49, 0x55, 0x13, 0x5D, 0x05, 0x10};
static uint8_t SPECTRA315_VCOM_WOUT_EN_V[6] = {0x04, 0x00, 0x80, 0xFF, 0xCA, 0x1B};
static uint8_t SPECTRA315_EN_BUF_V[1] = {0x00};
static uint8_t SPECTRA315_TM_TCON_V[7] = {0x00, 0xF4, 0x81, 0x40, 0x00, 0x00, 0x00};
static uint8_t SPECTRA315_CCSET_V[1] = {0x01};
static uint8_t SPECTRA315_PWS_V[1] = {0x22};
static uint8_t SPECTRA315_SPIM_V[1] = {0x00};

#endif
#endif

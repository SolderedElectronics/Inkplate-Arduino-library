# Inkplate 10 panel wiring, ESP32-S3 breakout

Wiring reference for the `inkplate10-s3-test` branch. Everything here is the source
of truth in `src/boards/Inkplate10/pins.h` — if you change a pin there, change it
here too.

Board target: `ARDUINO_ESP32S3_DEV`, octal PSRAM module (N8R8 or similar).

## Data bus, ESP32-S3 to panel

These are the eight parallel data lines the panel source driver latches on each
`CL` pulse. **This is the part that still needs connecting.**

| Panel | ESP32-S3 GPIO | Output register | Bit | `pins.h` |
|---|---|---|---|---|
| D0 | **38** | `GPIO.out1` | 6  | `EPD_D0` |
| D1 | **39** | `GPIO.out1` | 7  | `EPD_D1` |
| D2 | **40** | `GPIO.out1` | 8  | `EPD_D2` |
| D3 | **41** | `GPIO.out1` | 9  | `EPD_D3` |
| D4 | **42** | `GPIO.out1` | 10 | `EPD_D4` |
| D5 | **47** | `GPIO.out1` | 15 | `EPD_D5` |
| D6 | **48** | `GPIO.out1` | 16 | `EPD_D6` |
| D7 | **21** | `GPIO.out`  | 21 | `EPD_D7` |

### Why the bus is split across two registers

The S3 has no run of eight free GPIOs inside one output register. `GPIO.out` covers
GPIO0-31, `GPIO.out1` covers GPIO32-48. Of the pins broken out on this board only
seven usable ones (38-42, 47, 48) land in the high register, so D7 has to come from
the low one.

Every data write therefore touches both registers:

```c
GPIO.out1_w1ts.val = pinLUTH[data];
GPIO.out_w1ts      = pinLUT[data] | CL;
GPIO.out1_w1tc.val = DATA_HIGH;
GPIO.out_w1tc      = DATA_LOW | CL;
```

Four stores per byte instead of two, so roughly half the throughput of the original
ESP32-WROVER build. `DATA_TO_LOW()` / `DATA_TO_HIGH()` in `pins.h` do the scatter,
and `pinLUT` / `pinLUTH` plus `GLUT` / `GLUTH` (and the `GLUT2` pair) are parallel
tables, one per register.

### If you reorder these pins

D0-D4 must stay contiguous and ascending, and D5-D6 must stay contiguous and
ascending, or `DATA_TO_HIGH()` stops working — it covers the byte with two shifted
fields rather than eight individual bit tests:

```c
#define DATA_TO_HIGH(d) \
    ((((uint32_t)(d) & 0x1FUL) << (EPD_D0 - 32)) | ((((uint32_t)(d) >> 5) & 0x03UL) << (EPD_D5 - 32)))
```

Any other arrangement means rewriting that macro bit by bit.

## Control lines, ESP32-S3 to panel

All below GPIO32, so all in the low output register, driven with `w1ts` / `w1tc`
rather than `digitalWrite` — `CKV` toggles once per scan line and `CL` once per
byte, so the register writes matter.

| Panel | ESP32-S3 GPIO | `pins.h` |
|---|---|---|
| CL   | 4  | `CL_PIN` |
| SPV  | 5  | `SPV_PIN` |
| GMOD | 6  | `GMOD_PIN` |
| OE   | 7  | `OE_PIN` |
| CKV  | 10 | `CKV_PIN` |
| LE   | 11 | `LE_PIN` |
| SPH  | 12 | `SPH_PIN` |

## I/O expander, PCAL6416 at 0x20

PMIC control only. Everything else the stock Inkplate 10 hangs off the expanders
(microSD power, battery divider, touchpad, second expander) is not on this board.

| Signal | Expander pin | Direction | Notes |
|---|---|---|---|
| WAKEUP   | A0 (0) | output | TPS65186 must be awake before it answers on I2C |
| PWRUP    | A1 (1) | output | triggers the hardware power-up sequencer |
| VCOM     | A2 (2) | output | asserted only after PWR_GOOD reads 0xFA |
| TPS INT  | A3 (3) | input, pull-up | open drain, used by VCOM EEPROM programming |
| PWR_GOOD | A4 (4) | input, pull-up | open drain, needs the pull-up or it reads 0 forever |

## I2C bus

| Signal | ESP32-S3 GPIO |
|---|---|
| SDA | 8 |
| SCL | 9 |

Arduino core defaults for `esp32s3`, taken by `Wire.begin()` with no arguments.
**Do not reuse 8 or 9 for anything else.**

Devices on the bus:

| Address | Device | Notes |
|---|---|---|
| 0x20 | PCAL6416 I/O expander | always present |
| 0x48 | TPS65186 PMIC | only ACKs while WAKEUP is high |
| 0x51 | 24Cxx EEPROM | panel EEPROM |

The PMIC address is fixed in silicon at 0x48. Confirmed on this board by
`REVID (0x10) = 0x66`.

## Pins deliberately not used

| GPIO | Why |
|---|---|
| 0      | strapping pin, a panel line holding it low at reset drops the board into the bootloader |
| 8, 9   | I2C SDA / SCL |
| 19, 20 | USB D- / D+, using them costs native USB CDC and JTAG |
| 26-32  | SPI flash |
| 33-37  | octal PSRAM |
| 43, 44 | UART0 TX / RX |
| 45, 46 | strapping pins |

Still free after the above: **14**.

## Gotchas

**GPIO48 and GPIO38 drive the onboard RGB LED** on most S3 devkits — which of the
two varies by revision. The WS2812 is a load sitting on that data line. Check your
board and cut the jumper if it has one.

**PSRAM must be enabled.** The framebuffers need about 1.1 MB through `ps_malloc`.
Build with `PSRAM=opi`.

**Compile check:**

```
arduino-cli compile --fqbn esp32:esp32:esp32s3:PSRAM=opi,PartitionScheme=huge_app <sketch>
```

## Bring-up

`Inkplate10_S3_Bringup/` at the repo root is a standalone diagnostic — plain `Wire`
and `digitalWrite`, no Inkplate library. It scans the bus, walks the TPS65186
power-up two different ways, dumps the PMIC registers, and pulses every control and
data pin one at a time for scoping.

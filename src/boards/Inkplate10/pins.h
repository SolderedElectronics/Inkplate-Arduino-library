#ifndef __INKPLATE10_S3_PINS_H__
#define __INKPLATE10_S3_PINS_H__

#if defined(ARDUINO_ESP32S3_DEV)
#include "soc/gpio_reg.h"
#include "soc/gpio_struct.h"

// Only one PCAL6416 on the breakout, at 0x20.
#define IO_INT_ADDR 0x20

// ---------------------------------------------------------------------------
// I/O expander pins (PCAL6416 @ IO_INT_ADDR)
// ---------------------------------------------------------------------------
#define WAKEUP   IO_PIN_A0 // 0
#define PWRUP    IO_PIN_A1 // 1
#define VCOM     IO_PIN_A2 // 2
// TPS65186 INT line. Set to -1 if it is not wired to the expander, the VCOM
// programming routine then falls back to a fixed delay.
#define TPS_INT  IO_PIN_A3 // 3
#define PWR_GOOD IO_PIN_A4 // 4, PWR_GOOD output of the TPS65186

// The TPS65186 I2C address is fixed in silicon at 0x48. Override here only if a
// bus scan proves otherwise.
#define TPS65186_I2C_ADDR 0x48

#define WAKEUP_SET                                                                                                     \
    {                                                                                                                  \
        expander1.digitalWrite(WAKEUP, HIGH, true);                                                                    \
    }
#define WAKEUP_CLEAR                                                                                                   \
    {                                                                                                                  \
        expander1.digitalWrite(WAKEUP, LOW, true);                                                                     \
    }
#define PWRUP_SET                                                                                                      \
    {                                                                                                                  \
        expander1.digitalWrite(PWRUP, HIGH, true);                                                                     \
    }
#define PWRUP_CLEAR                                                                                                    \
    {                                                                                                                  \
        expander1.digitalWrite(PWRUP, LOW, true);                                                                      \
    }
#define VCOM_SET                                                                                                       \
    {                                                                                                                  \
        expander1.digitalWrite(VCOM, HIGH, true);                                                                      \
    }
#define VCOM_CLEAR                                                                                                     \
    {                                                                                                                  \
        expander1.digitalWrite(VCOM, LOW, true);                                                                       \
    }

// ---------------------------------------------------------------------------
// EPD control lines, all on native ESP32-S3 GPIOs below 32, so they all live in
// the low GPIO output register and are driven with w1ts / w1tc.
// ---------------------------------------------------------------------------
#define CL_PIN   4
#define SPV_PIN  5
#define GMOD_PIN 6
#define OE_PIN   7
#define CKV_PIN  10
#define LE_PIN   11
#define SPH_PIN  12

#define CL   (1UL << CL_PIN)
#define SPV  (1UL << SPV_PIN)
#define GMOD (1UL << GMOD_PIN)
#define OE   (1UL << OE_PIN)
#define CKV  (1UL << CKV_PIN)
#define LE   (1UL << LE_PIN)
#define SPH  (1UL << SPH_PIN)

#define OE_SET                                                                                                         \
    {                                                                                                                  \
        GPIO.out_w1ts = OE;                                                                                            \
    }
#define OE_CLEAR                                                                                                       \
    {                                                                                                                  \
        GPIO.out_w1tc = OE;                                                                                            \
    }
#define GMOD_SET                                                                                                       \
    {                                                                                                                  \
        GPIO.out_w1ts = GMOD;                                                                                          \
    }
#define GMOD_CLEAR                                                                                                     \
    {                                                                                                                  \
        GPIO.out_w1tc = GMOD;                                                                                          \
    }
#define SPV_SET                                                                                                        \
    {                                                                                                                  \
        GPIO.out_w1ts = SPV;                                                                                           \
    }
#define SPV_CLEAR                                                                                                      \
    {                                                                                                                  \
        GPIO.out_w1tc = SPV;                                                                                           \
    }
#define CL_SET                                                                                                         \
    {                                                                                                                  \
        GPIO.out_w1ts = CL;                                                                                            \
    }
#define CL_CLEAR                                                                                                       \
    {                                                                                                                  \
        GPIO.out_w1tc = CL;                                                                                            \
    }
#define CKV_SET                                                                                                        \
    {                                                                                                                  \
        GPIO.out_w1ts = CKV;                                                                                           \
    }
#define CKV_CLEAR                                                                                                      \
    {                                                                                                                  \
        GPIO.out_w1tc = CKV;                                                                                           \
    }
#define SPH_SET                                                                                                        \
    {                                                                                                                  \
        GPIO.out_w1ts = SPH;                                                                                           \
    }
#define SPH_CLEAR                                                                                                      \
    {                                                                                                                  \
        GPIO.out_w1tc = SPH;                                                                                           \
    }
#define LE_SET                                                                                                         \
    {                                                                                                                  \
        GPIO.out_w1ts = LE;                                                                                            \
    }
#define LE_CLEAR                                                                                                       \
    {                                                                                                                  \
        GPIO.out_w1tc = LE;                                                                                            \
    }

// ---------------------------------------------------------------------------
// EPD data bus.
//
// The ESP32-S3 has no run of eight free GPIOs inside a single output register,
// so the bus is split: D0-D6 sit in the high register (GPIO32-48) and D7 in the
// low one. Every data write therefore touches both registers. The scatter
// helpers below turn a data byte into the two register masks.
// ---------------------------------------------------------------------------
#define EPD_D0 38
#define EPD_D1 39
#define EPD_D2 40
#define EPD_D3 41
#define EPD_D4 42
#define EPD_D5 47
#define EPD_D6 48
#define EPD_D7 21

// Bit masks inside the low output register (GPIO0-31).
#define DATA_LOW (1UL << EPD_D7)

// Bit masks inside the high output register, bit n maps to GPIO (32 + n).
#define DATA_HIGH                                                                                                      \
    ((1UL << (EPD_D0 - 32)) | (1UL << (EPD_D1 - 32)) | (1UL << (EPD_D2 - 32)) | (1UL << (EPD_D3 - 32)) |               \
     (1UL << (EPD_D4 - 32)) | (1UL << (EPD_D5 - 32)) | (1UL << (EPD_D6 - 32)))

// Kept for code that clears the whole low-register bus in one go.
#define DATA DATA_LOW

// Data byte -> low register mask. Only D7 lives here.
#define DATA_TO_LOW(d) (((uint32_t)(d) & 0x80UL) ? DATA_LOW : 0UL)

// Data byte -> high register mask. D0-D4 are contiguous at GPIO38-42 and D5/D6
// at GPIO47/48, so two shifted fields cover the whole thing.
#define DATA_TO_HIGH(d)                                                                                                \
    ((((uint32_t)(d) & 0x1FUL) << (EPD_D0 - 32)) | ((((uint32_t)(d) >> 5) & 0x03UL) << (EPD_D5 - 32)))

#endif
#endif

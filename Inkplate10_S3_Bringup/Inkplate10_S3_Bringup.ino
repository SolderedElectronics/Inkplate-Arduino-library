/**
 * Inkplate 10 ESP32-S3 breakout bring-up diagnostic.
 *
 * Does not use the Inkplate library at all. It talks to the bus and the pins
 * directly so a failure here points at wiring, not at the driver.
 *
 * 1) Scans I2C and reports every device that ACKs.
 * 2) Walks the TPS65186 power-up sequence by hand, printing the result of every
 *    step and the PWR_GOOD register as the rails come up.
 * 3) Toggles every EPD control line and data line so they can be scoped.
 *
 * Board: ESP32S3 Dev Module, PSRAM: OPI, USB CDC On Boot: Enabled.
 */

#include <Wire.h>

#define EXP_ADDR 0x20 // PCAL6416
#define PMIC_ADDR 0x48

// Expander pins
#define P_WAKEUP 0
#define P_PWRUP 1
#define P_VCOM 2
#define P_INT 3
#define P_PWRGOOD 4

// PCAL6416 registers
#define PCAL_OUT_0 0x02
#define PCAL_IN_0 0x00
#define PCAL_CFG_0 0x06

// Native GPIOs
const uint8_t ctrlPins[] = {4, 5, 6, 7, 10, 11, 12};
const char *ctrlNames[] = {"CL(4)", "SPV(5)", "GMOD(6)", "OE(7)", "CKV(10)", "LE(11)", "SPH(12)"};
const uint8_t dataPins[] = {38, 39, 40, 41, 42, 47, 48, 21};
const char *dataNames[] = {"D0(38)", "D1(39)", "D2(40)", "D3(41)", "D4(42)", "D5(47)", "D6(48)", "D7(21)"};

uint8_t expOut = 0x00;  // shadow of output port 0
uint8_t expCfg = 0xFF;  // shadow of config port 0, 1 = input

bool i2cWriteReg(uint8_t addr, uint8_t reg, uint8_t val)
{
    Wire.beginTransmission(addr);
    Wire.write(reg);
    Wire.write(val);
    return Wire.endTransmission() == 0;
}

int i2cReadReg(uint8_t addr, uint8_t reg)
{
    Wire.beginTransmission(addr);
    Wire.write(reg);
    if (Wire.endTransmission() != 0)
        return -1;
    if (Wire.requestFrom((int)addr, 1) != 1)
        return -1;
    return Wire.read();
}

void expPinMode(uint8_t pin, bool output)
{
    if (output)
        expCfg &= ~(1 << pin);
    else
        expCfg |= (1 << pin);
    i2cWriteReg(EXP_ADDR, PCAL_CFG_0, expCfg);
}

void expWrite(uint8_t pin, bool state)
{
    if (state)
        expOut |= (1 << pin);
    else
        expOut &= ~(1 << pin);
    i2cWriteReg(EXP_ADDR, PCAL_OUT_0, expOut);
}

int expRead(uint8_t pin)
{
    int v = i2cReadReg(EXP_ADDR, PCAL_IN_0);
    if (v < 0)
        return -1;
    return (v >> pin) & 1;
}

void scanBus()
{
    Serial.println(F("\n--- 1. I2C scan ---"));
    int found = 0;
    for (uint8_t a = 1; a < 127; a++)
    {
        Wire.beginTransmission(a);
        if (Wire.endTransmission() == 0)
        {
            Serial.printf("  ACK at 0x%02X", a);
            if (a == 0x20)
                Serial.print(F("   <- PCAL6416 I/O expander"));
            else if (a == 0x48)
                Serial.print(F("   <- TPS65186 PMIC"));
            else if (a >= 0x50 && a <= 0x57)
                Serial.print(F("   <- 24Cxx EEPROM"));
            Serial.println();
            found++;
        }
        delay(2);
    }
    if (!found)
        Serial.println(F("  NOTHING ACKed. Check SDA=8 / SCL=9, pull-ups and ground."));
}

void dumpPmic(const char *when)
{
    Serial.printf("  [%s] ENABLE(0x01)=0x%02X  INT1(0x07)=0x%02X  INT2(0x08)=0x%02X  PG(0x0F)=0x%02X\n", when,
                  i2cReadReg(PMIC_ADDR, 0x01), i2cReadReg(PMIC_ADDR, 0x07), i2cReadReg(PMIC_ADDR, 0x08),
                  i2cReadReg(PMIC_ADDR, 0x0F));
}

void decodeFaults(int int1, int int2)
{
    if (int1 < 0 || int2 < 0)
        return;
    Serial.printf("    INT1 = 0x%02X  INT2 = 0x%02X   (check these against the datasheet,\n", int1, int2);
    Serial.println(F("     the bit names in the TPS65186 INT registers are easy to get wrong)"));
}

// Drives an expander output, then reads the input port back. On a PCAL6416 the
// input register always reflects the actual pin, so a mismatch means the pin is
// being held by something external.
void checkExpanderDrive(uint8_t pin, const char *name)
{
    expWrite(pin, true);
    delay(2);
    int hi = expRead(pin);
    expWrite(pin, false);
    delay(2);
    int lo = expRead(pin);
    Serial.printf("  %s: drive HIGH reads %d, drive LOW reads %d  %s\n", name, hi, lo,
                  (hi == 1 && lo == 0) ? "ok" : "<- PIN IS NOT FOLLOWING, check wiring / short");
}

bool waitPowerGood(uint16_t ms)
{
    unsigned long t = millis();
    while (millis() - t < ms)
    {
        if (i2cReadReg(PMIC_ADDR, 0x0F) == 0xFA)
        {
            Serial.printf("  rails up after %lu ms\n", millis() - t);
            return true;
        }
        delay(5);
    }
    return false;
}

// Writes a value to ENABLE and reports which bits actually stuck. On a healthy
// TPS65186 the rail-enable bits 5:0 are plain R/W.
void enableBitTest()
{
    Serial.println(F("\n  ENABLE (0x01) write/readback test:"));
    const uint8_t vals[] = {0x20, 0x21, 0x23, 0x27, 0x2F, 0x3F, 0xBF};
    for (uint8_t i = 0; i < sizeof(vals); i++)
    {
        bool w = i2cWriteReg(PMIC_ADDR, 0x01, vals[i]);
        delay(5);
        int rb = i2cReadReg(PMIC_ADDR, 0x01);
        Serial.printf("    wrote 0x%02X (%s) -> read 0x%02X %s\n", vals[i], w ? "ack" : "NAK", rb,
                      (rb == vals[i]) ? "" : "<- bits dropped");
    }
    i2cWriteReg(PMIC_ADDR, 0x01, 0x00);
}

// Strobes the temperature conversion and reads the result. A wild value means the
// sensor path is broken, which is enough on its own to make the chip refuse.
void tempTest()
{
    Serial.println(F("\n  Temperature sensor:"));
    i2cWriteReg(PMIC_ADDR, 0x0D, 0x80); // TMST1, start conversion
    delay(20);
    int raw = i2cReadReg(PMIC_ADDR, 0x00);
    int tmst1 = i2cReadReg(PMIC_ADDR, 0x0D);
    Serial.printf("    TMST_VALUE(0x00) = 0x%02X = %d C     TMST1(0x0D) = 0x%02X\n", raw, (int8_t)raw, tmst1);
    if (raw == 0x00 || raw == 0xFF)
        Serial.println(F("    <- implausible, temperature path is not working"));
}

void miscRegs()
{
    Serial.println(F("\n  Other registers:"));
    Serial.printf("    VADJ (0x02) = 0x%02X   (default 0x03)\n", i2cReadReg(PMIC_ADDR, 0x02));
    int vcl = i2cReadReg(PMIC_ADDR, 0x03);
    int vch = i2cReadReg(PMIC_ADDR, 0x04);
    Serial.printf("    VCOM (0x03/0x04) = 0x%02X / 0x%02X  -> %.2f V\n", vcl, vch,
                  -((((vch & 0x01) << 8) | vcl) / 100.0));
    Serial.printf("    UPSEQ0 (0x09) = 0x%02X  UPSEQ1 (0x0A) = 0x%02X\n", i2cReadReg(PMIC_ADDR, 0x09),
                  i2cReadReg(PMIC_ADDR, 0x0A));
    Serial.printf("    DWNSEQ0(0x0B) = 0x%02X  DWNSEQ1(0x0C) = 0x%02X\n", i2cReadReg(PMIC_ADDR, 0x0B),
                  i2cReadReg(PMIC_ADDR, 0x0C));
    Serial.printf("    INT_EN1(0x05) = 0x%02X  INT_EN2(0x06) = 0x%02X\n", i2cReadReg(PMIC_ADDR, 0x05),
                  i2cReadReg(PMIC_ADDR, 0x06));
}

// Tries the power-up again with every other expander pin driven high, then, if
// that helps, bisects to find which one matters. Covers the case where the
// breakout has an enable or level-shifter line on a pin the driver leaves alone.
bool tryPowerUp(uint16_t ms)
{
    i2cReadReg(PMIC_ADDR, 0x07);
    i2cReadReg(PMIC_ADDR, 0x08);
    i2cWriteReg(PMIC_ADDR, 0x01, 0x20);
    i2cWriteReg(PMIC_ADDR, 0x09, 0xE4);
    i2cWriteReg(PMIC_ADDR, 0x0B, 0x1B);
    expWrite(P_PWRUP, true);
    bool ok = waitPowerGood(ms);
    if (!ok)
    {
        expWrite(P_PWRUP, false);
        i2cWriteReg(PMIC_ADDR, 0x01, 0x00);
        delay(50);
    }
    return ok;
}

void hiddenEnableHunt()
{
    Serial.println(F("\n  [C] retry with every spare expander pin driven HIGH"));
    Serial.println(F("      (stock Inkplate 10 drives expander pin 8 high in gpioInit)"));

    // Port 0 pins 5..7 and all of port 1. Ports 0..4 are WAKEUP/PWRUP/VCOM/INT/PWR_GOOD.
    uint8_t cfg1 = 0x00, out1 = 0xFF;
    expCfg &= ~0xE0;
    expOut |= 0xE0;
    i2cWriteReg(EXP_ADDR, PCAL_OUT_0, expOut);
    i2cWriteReg(EXP_ADDR, PCAL_CFG_0, expCfg);
    i2cWriteReg(EXP_ADDR, PCAL_OUT_0 + 1, out1);
    i2cWriteReg(EXP_ADDR, PCAL_CFG_0 + 1, cfg1);
    delay(20);

    if (tryPowerUp(500))
    {
        Serial.println(F("      RAILS CAME UP. One of the spare pins is an enable."));
        Serial.println(F("      Bisecting, one pin at a time:"));
        expWrite(P_PWRUP, false);
        i2cWriteReg(PMIC_ADDR, 0x01, 0x00);
        delay(100);

        for (uint8_t pin = 5; pin < 16; pin++)
        {
            // All spare pins low except this one.
            expOut &= ~0xE0;
            out1 = 0x00;
            if (pin < 8)
                expOut |= (1 << pin);
            else
                out1 = (1 << (pin - 8));
            i2cWriteReg(EXP_ADDR, PCAL_OUT_0, expOut);
            i2cWriteReg(EXP_ADDR, PCAL_OUT_0 + 1, out1);
            delay(20);
            if (tryPowerUp(300))
            {
                Serial.printf("      -> expander pin %d is the enable. Drive it high in gpioInit().\n", pin);
                expWrite(P_PWRUP, false);
                i2cWriteReg(PMIC_ADDR, 0x01, 0x00);
                return;
            }
        }
        Serial.println(F("      No single pin did it, it takes a combination."));
    }
    else
    {
        Serial.println(F("      Still no rails. Not an expander enable pin."));
        Serial.println(F("      -> measure VIN at the TPS while this runs."));
    }

    // Put the spare pins back to inputs.
    expCfg |= 0xE0;
    i2cWriteReg(EXP_ADDR, PCAL_CFG_0, expCfg);
    i2cWriteReg(EXP_ADDR, PCAL_CFG_0 + 1, 0xFF);
}

void powerUpTest()
{
    Serial.println(F("\n--- 2. TPS65186 power-up walk ---"));

    if (i2cReadReg(EXP_ADDR, PCAL_IN_0) < 0)
    {
        Serial.println(F("  Expander at 0x20 does not answer. Stopping."));
        return;
    }

    // WAKEUP, PWRUP, VCOM as outputs low. INT and PWR_GOOD stay inputs.
    expCfg = 0xFF;
    expOut = 0x00;
    i2cWriteReg(EXP_ADDR, PCAL_OUT_0, expOut);
    expPinMode(P_WAKEUP, true);
    expPinMode(P_PWRUP, true);
    expPinMode(P_VCOM, true);

    Serial.println(F("\n  Expander drive check (TPS is asleep, pins should follow freely):"));
    checkExpanderDrive(P_WAKEUP, "A0 WAKEUP");
    checkExpanderDrive(P_PWRUP, "A1 PWRUP ");
    checkExpanderDrive(P_VCOM, "A2 VCOM  ");

    Serial.println(F("\n  WAKEUP -> HIGH"));
    expWrite(P_WAKEUP, true);
    delay(10);

    int rev = i2cReadReg(PMIC_ADDR, 0x10);
    if (rev < 0)
    {
        Serial.println(F("  PMIC at 0x48 silent with WAKEUP high. Re-scanning:"));
        scanBus();
        expWrite(P_WAKEUP, false);
        return;
    }
    Serial.printf("  REVID(0x10) = 0x%02X   (TPS65186 reports 0x65 or 0x66)\n", rev);
    dumpPmic("awake, idle");

    miscRegs();
    tempTest();
    enableBitTest();

    // Confirm the PMIC only appears on the bus while WAKEUP is high.
    Serial.println(F("\n  Re-scan with WAKEUP high (0x48 should appear now):"));
    scanBus();

    // Clear any stale fault latches before trying.
    i2cReadReg(PMIC_ADDR, 0x07);
    i2cReadReg(PMIC_ADDR, 0x08);

    // ---- attempt A: power up with the PWRUP pin, exactly as the driver does ----
    Serial.println(F("\n  [A] power-up via PWRUP pin"));
    Serial.printf("      ENABLE  (0x01 <- 0x20): %s\n", i2cWriteReg(PMIC_ADDR, 0x01, 0x20) ? "ok" : "FAILED");
    Serial.printf("      UPSEQ0  (0x09 <- 0xE4): %s\n", i2cWriteReg(PMIC_ADDR, 0x09, 0xE4) ? "ok" : "FAILED");
    Serial.printf("      DWNSEQ0 (0x0B <- 0x1B): %s\n", i2cWriteReg(PMIC_ADDR, 0x0B, 0x1B) ? "ok" : "FAILED");
    dumpPmic("after writes");
    expWrite(P_PWRUP, true);
    bool okPin = waitPowerGood(500);
    dumpPmic(okPin ? "A up" : "A FAILED");
    if (!okPin)
        decodeFaults(i2cReadReg(PMIC_ADDR, 0x07), i2cReadReg(PMIC_ADDR, 0x08));
    expWrite(P_PWRUP, false);
    delay(200);

    // ---- attempt B: power up in software, ACTIVE bit, PWRUP pin left low ----
    // If B works and A does not, the PWRUP pin is not reaching the TPS.
    Serial.println(F("\n  [B] power-up via ENABLE.ACTIVE bit, PWRUP pin held low"));
    i2cReadReg(PMIC_ADDR, 0x07);
    i2cReadReg(PMIC_ADDR, 0x08);
    bool wAck = i2cWriteReg(PMIC_ADDR, 0x01, 0xBF); // ACTIVE + V3P3 + all rail enables
    delay(5);
    Serial.printf("      ENABLE <- 0xBF (%s), reads back 0x%02X\n", wAck ? "ack" : "NAK",
                  i2cReadReg(PMIC_ADDR, 0x01));
    bool okSw = waitPowerGood(500);
    dumpPmic(okSw ? "B up" : "B FAILED");
    if (!okSw)
        decodeFaults(i2cReadReg(PMIC_ADDR, 0x07), i2cReadReg(PMIC_ADDR, 0x08));

    if (okSw || okPin)
    {
        Serial.println(F("  RAILS UP. VCOM -> HIGH for 2 s"));
        expWrite(P_VCOM, true);
        Serial.printf("  temperature reg 0x00 = %d C\n", i2cReadReg(PMIC_ADDR, 0x00));
        delay(2000);
        expWrite(P_VCOM, false);
    }

    if (!okPin && !okSw)
        hiddenEnableHunt();

    Serial.println(F("\n  VERDICT:"));
    if (okPin)
        Serial.println(F("    PWRUP pin path works. The driver should work as written."));
    else if (okSw)
        Serial.println(F("    Software power-up works, PWRUP pin does not."));
    else
        Serial.println(F("    Neither path works. Fault bits above, or no input supply / inductor."));

    // Shut everything back down.
    i2cWriteReg(PMIC_ADDR, 0x01, 0x00);
    delay(100);
    expWrite(P_WAKEUP, false);
}

void pinToggleTest()
{
    Serial.println(F("\n--- 3. GPIO toggle, scope each pin ---"));
    Serial.println(F("  Every pin gets 20 pulses at roughly 1 kHz, one pin at a time."));

    for (uint8_t i = 0; i < sizeof(ctrlPins); i++)
    {
        Serial.printf("  %s\n", ctrlNames[i]);
        pinMode(ctrlPins[i], OUTPUT);
        for (int n = 0; n < 20; n++)
        {
            digitalWrite(ctrlPins[i], HIGH);
            delay(1);
            digitalWrite(ctrlPins[i], LOW);
            delay(1);
        }
        delay(300);
    }

    for (uint8_t i = 0; i < sizeof(dataPins); i++)
    {
        Serial.printf("  %s\n", dataNames[i]);
        pinMode(dataPins[i], OUTPUT);
        for (int n = 0; n < 20; n++)
        {
            digitalWrite(dataPins[i], HIGH);
            delay(1);
            digitalWrite(dataPins[i], LOW);
            delay(1);
        }
        delay(300);
    }
}

void setup()
{
    Serial.begin(115200);
    delay(3000);
    Serial.println(F("\n\n===== Inkplate 10 ESP32-S3 bring-up ====="));

    Wire.begin();
    Wire.setClock(100000);
    Serial.printf("I2C on SDA=%d SCL=%d\n", SDA, SCL);

    scanBus();
    powerUpTest();
    pinToggleTest();

    Serial.println(F("\n===== done ====="));
}

void loop()
{
}

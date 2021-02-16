#include "../Inkplate.h"
#include "../include/Graphics.h"
#include "../include/defines.h"

#ifdef ARDUINO_INKPLATECOLOR

bool Inkplate::begin(void)
{
    unsigned long _timeout = 0;

    // Check if begin is called already. If it is, that means that panel needs to be reseted in order to be woken up
    // from deep sleep. If is this first time begin has been called, init SPI, init pins, allocate memory for frame
    // buffer and clear frame buffer
    if (!_beginDone)
    {
        _beginDone = true;
        SPI.begin();
        pinMode(EPAPER_BUSY_PIN, INPUT);
        pinMode(EPAPER_RST_PIN, OUTPUT);
        pinMode(EPAPER_DC_PIN, OUTPUT);
        pinMode(EPAPER_CS_PIN, OUTPUT);

        // Allocate memory for internal frame buffer
        DMemory4Bit = (uint8_t *)ps_malloc(E_INK_WIDTH * E_INK_HEIGHT / 2);
        if (DMemory4Bit == NULL)
        {
            // Serial.println("Memory allocation failed, program stops!");
            return false;
        }

        // Color whole frame buffer in white color
        memset(DMemory4Bit, INKPLATE_WHITE | (INKPLATE_WHITE << 4), E_INK_WIDTH * E_INK_HEIGHT / 2);
    }

    // Reset epaper
    resetPanel();

    // Wait for epapper to be ready by reading busy high signal
    _timeout = millis();
    while ((!digitalRead(EPAPER_BUSY_PIN)) && ((millis() - _timeout) < INIT_TIMEOUT))
        ;
    if (!digitalRead(EPAPER_BUSY_PIN))
        return false;

    // Send whole bunch of commands and data
    uint8_t panel_set_data[] = {0xef, 0x08};
    sendCommand(PANEL_SET_REGISTER);
    sendData(panel_set_data, 2);

    uint8_t power_set_data[] = {0x37, 0x00, 0x23, 0x23};
    sendCommand(POWER_SET_REGISTER);
    sendData(power_set_data, 4);

    sendCommand(POWER_OFF_SEQ_SET_REGISTER);
    sendData(0x00);

    uint8_t booster_softstart_data[] = {0xc7, 0xc7, 0x1d};
    sendCommand(BOOSTER_SOFTSTART_REGISTER);
    sendData(booster_softstart_data, 3);

    sendCommand(PLL_CONTROL_REGISTER);
    sendData(0x3c);

    sendCommand(TEMP_SENSOR_REGISTER);
    sendData(0x00);

    sendCommand(VCOM_DATA_INTERVAL_REGISTER);
    sendData(0x37);

    sendCommand(0x60);
    sendData(0x20);

    uint8_t res_set_data[] = {0x02, 0x58, 0x01, 0xc0};
    sendCommand(RESOLUTION_SET_REGISTER);
    sendData(res_set_data, 4);

    sendCommand(0xE3);
    sendData(0xaa);

    delay(100);
    sendCommand(0x50);
    sendData(0x37);

    _panelState = true;
    return true;
}

void Inkplate::display()
{
    if (!_panelState)
        return;

    // Set resolution setting
    uint8_t res_set_data[] = {0x02, 0x58, 0x01, 0xc0};
    sendCommand(0x61);
    sendData(res_set_data, 4);

    // Push pixel data to the epaper RAM
    sendCommand(0x10);
    digitalWrite(EPAPER_DC_PIN, HIGH);
    digitalWrite(EPAPER_CS_PIN, LOW);
    SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
    SPI.transfer(DMemory4Bit, E_INK_WIDTH * E_INK_HEIGHT / 2);
    SPI.endTransaction();
    digitalWrite(EPAPER_CS_PIN, HIGH);

    sendCommand(POWER_OFF_REGISTER);
    while (!(digitalRead(EPAPER_BUSY_PIN)))
        ; // Wait for busy high signal
    sendCommand(DISPLAY_REF_REGISTER);
    while (!(digitalRead(EPAPER_BUSY_PIN)))
        ; // Wait for busy high signal
    sendCommand(POWER_OFF_REGISTER);
    while (digitalRead(EPAPER_BUSY_PIN))
        ; // Wait for busy low signal
    delay(200);
}

void Graphics::writePixel(int16_t x0, int16_t y0, uint16_t _color)
{
    if (x0 > width() - 1 || y0 > height() - 1 || x0 < 0 || y0 < 0)
        return;
    if (_color > 6 || _color < 0)
        return;

    switch (rotation)
    {
    case 1:
        _swap_int16_t(x0, y0);
        x0 = height() - x0 - 1;
        break;
    case 2:
        x0 = width() - x0 - 1;
        y0 = height() - y0 - 1;
        break;
    case 3:
        _swap_int16_t(x0, y0);
        y0 = width() - y0 - 1;
        break;
    }

    int _x = x0 / 2;
    int _x_sub = x0 % 2;
    uint8_t temp;
    temp = *(DMemory4Bit + E_INK_WIDTH / 2 * y0 + _x);
    *(DMemory4Bit + E_INK_WIDTH / 2 * y0 + _x) = pixelMaskGLUT[_x_sub] & temp | (_x_sub ? _color : _color << 4);
}

void Inkplate::clean()
{
    if (!_panelState)
        return;

    // Set resolution setting
    uint8_t res_set_data[] = {0x02, 0x58, 0x01, 0xc0};
    sendCommand(0x61);
    sendData(res_set_data, 4);

    // Push pixel data to the epaper RAM
    sendCommand(0x10);
    digitalWrite(EPAPER_DC_PIN, HIGH);
    digitalWrite(EPAPER_CS_PIN, LOW);
    SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
    for (uint32_t i = 0; i < (E_INK_WIDTH * E_INK_HEIGHT / 2); i++)
    {
        SPI.transfer(INKPLATE_WHITE | INKPLATE_WHITE << 4);
    }
    SPI.endTransaction();
    digitalWrite(EPAPER_CS_PIN, HIGH);

    sendCommand(POWER_OFF_REGISTER);
    while (!(digitalRead(EPAPER_BUSY_PIN)))
        ; // Wait for busy high signal
    sendCommand(DISPLAY_REF_REGISTER);
    while (!(digitalRead(EPAPER_BUSY_PIN)))
        ; // Wait for busy high signal
    sendCommand(POWER_OFF_REGISTER);
    while (digitalRead(EPAPER_BUSY_PIN))
        ; // Wait for busy low signal
    delay(200);
}

void Inkplate::resetPanel()
{
    digitalWrite(EPAPER_RST_PIN, LOW);
    delay(1);
    digitalWrite(EPAPER_RST_PIN, HIGH);
    delay(200);
}

void Inkplate::sendCommand(uint8_t _command)
{
    digitalWrite(EPAPER_DC_PIN, LOW);
    digitalWrite(EPAPER_CS_PIN, LOW);
    SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
    SPI.transfer(_command);
    SPI.endTransaction();
    digitalWrite(EPAPER_CS_PIN, HIGH);
}

void Inkplate::sendData(uint8_t *_data, int _n)
{
    digitalWrite(EPAPER_DC_PIN, HIGH);
    digitalWrite(EPAPER_CS_PIN, LOW);
    SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
    SPI.transfer(_data, _n);
    SPI.endTransaction();
    digitalWrite(EPAPER_CS_PIN, HIGH);
}

void Inkplate::sendData(uint8_t _data)
{
    digitalWrite(EPAPER_DC_PIN, HIGH);
    digitalWrite(EPAPER_CS_PIN, LOW);
    SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
    SPI.transfer(_data);
    SPI.endTransaction();
    digitalWrite(EPAPER_CS_PIN, HIGH);
}

#endif
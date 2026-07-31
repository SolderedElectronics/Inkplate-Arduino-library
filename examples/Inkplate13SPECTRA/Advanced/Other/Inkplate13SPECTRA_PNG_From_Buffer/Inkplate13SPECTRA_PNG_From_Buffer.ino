/**
 **************************************************
 * @file        Inkplate13SPECTRA_PNG_From_Buffer.ino
 * @brief       Display a PNG image loaded into a RAM buffer on Soldered Inkplate 13SPECTRA.
 *
 * @details     Demonstrates how to read a PNG file from an SD card into a RAM
 *              buffer and then display it using drawPngFromBuffer(). The same
 *              technique applies to PNG data received from any source - a network
 *              socket, a serial transfer, a flash partition, etc.
 *
 *              The entire PNG file is loaded into heap memory before decoding,
 *              so make sure the file fits in available RAM (ESP32 has ~300 KB
 *              free heap). PNG resolution should not exceed 1200 x 1600 pixels.
 *              Dithering is enabled by default; pass false as the fifth argument
 *              to disable it. Colors are mapped to the Spectra 6 color e-paper
 *              palette. Expected output is the PNG image shown on the Inkplate
 *              display with color dithering.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 13SPECTRA
 * - Hardware:   Inkplate 13SPECTRA, USB cable, microSD card
 * - Extra:      SD card containing a file named "image.png"
 * - SD card:    FAT / FAT32 format
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 13SPECTRA"
 *    from Tools -> Board.
 * 2) Copy a PNG file named "image.png" to a FAT-formatted SD card.
 * 3) Insert the SD card into the Inkplate.
 * 4) Upload the sketch to Inkplate 13SPECTRA.
 * 5) The PNG is read into RAM and rendered on the e-paper display.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/13spectra/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2026-04-20
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE13SPECTRA
#error "Wrong board selection for this example, please select Soldered Inkplate 13SPECTRA in the boards menu."
#endif

#include "Inkplate.h"

Inkplate display;

void setup()
{
    display.begin();
    display.clearDisplay();
    display.setTextColor(BLACK);
    display.setTextSize(2);

    if (!display.sdCardInit())
    {
        display.println("SD card error!");
        display.display();
        return;
    }

    SdFile file;
    if (!file.open("image.png", O_RDONLY))
    {
        display.println("Cannot open image.png");
        display.display();
        display.sdCardSleep();
        return;
    }

    uint32_t fileSize = file.fileSize();

    // Allocate a buffer large enough for the whole file
    uint8_t *buf = (uint8_t *)malloc(fileSize);
    if (!buf)
    {
        display.println("Not enough RAM for image buffer!");
        display.display();
        file.close();
        display.sdCardSleep();
        return;
    }

    // Read the entire file into the buffer
    file.read(buf, fileSize);
    file.close();
    display.sdCardSleep();

    // Draw the PNG from the in-memory buffer
    if (!display.image.drawPngFromBuffer(buf, fileSize, 0, 0, true, false))
    {
        display.println("PNG decode error");
    }

    free(buf);

    display.display();
}

void loop()
{
    // Nothing...
}

/**
 **************************************************
 * @file        Inkplate13SPECTRA_Dither_Kernels.ino
 * @brief       Switch dithering kernels when drawing an image on Soldered Inkplate 13SPECTRA.
 *
 * @details     Demonstrates how to select a different error-diffusion dithering
 *              kernel before drawing an image from an SD card. The kernel controls
 *              how the error from reducing the source image to the display's
 *              limited color palette is spread to neighboring pixels - different
 *              kernels trade off sharpness, noise and speed.
 *
 *              Available kernels (ImageColor::DitherKernel enum):
 *              FloydSteinberg    - default, classic error diffusion
 *              JarvisJudiceNinke - wider diffusion, smoother, slower
 *              Atkinson          - lighter diffusion, less noise, used on classic Mac
 *              Burkes            - simplified JJN variant
 *              Stucki            - similar to JJN, sharper
 *              SierraLite        - lightweight, fast
 *              ReducedDiffusion  - Floyd-Steinberg pattern at ~69% strength, more
 *                                  vibrant, less washed-out
 *
 *              Call setDitherKernel() before draw() - it only affects draw calls
 *              made afterwards with dithering enabled. This example cycles through
 *              every kernel, redrawing the same image and refreshing the display
 *              once per kernel so the results can be compared.
 *
 * Requirements:
 * - Board:      Soldered Inkplate 13SPECTRA
 * - Hardware:   Inkplate 13SPECTRA, USB cable, SD card
 * - Extra:      SD card with a file named "picture1.jpg"
 * - SD card:    FAT / FAT32 format
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate 13SPECTRA"
 *    from Tools -> Board.
 * 2) Copy picture1.jpg to a FAT-formatted SD card and insert it into the Inkplate.
 * 3) Upload the sketch to Inkplate 13SPECTRA.
 * 4) The image is drawn once per kernel, refreshing the display between each one.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/13spectra/quick-start-guide/
 * @note        Want to learn more about Inkplate? Visit
 *              https://docs.soldered.com/inkplate/
 * @note        Looking to get support? Write on our community forum:
 *              https://community.soldered.com/
 *
 * @author      Soldered
 * @date        2026-09-17
 * @license     GNU GPL V3
 **************************************************/

// Next 3 lines are a precaution, you can ignore those, and the example would also work without them
#ifndef ARDUINO_INKPLATE13SPECTRA
#error "Wrong board selection for this example, please select Soldered Inkplate 13SPECTRA in the boards menu."
#endif

#include "Inkplate.h"

Inkplate display;
SdFile file;

ImageColor::DitherKernel kernels[] = {
    ImageColor::FloydSteinberg, ImageColor::JarvisJudiceNinke, ImageColor::Atkinson,     ImageColor::Burkes,
    ImageColor::Stucki,         ImageColor::SierraLite,        ImageColor::ReducedDiffusion,
};
const char *kernelNames[] = {
    "Floyd-Steinberg", "Jarvis-Judice-Ninke", "Atkinson", "Burkes", "Stucki", "Sierra Lite", "Reduced Diffusion",
};
const uint8_t kernelCount = sizeof(kernels) / sizeof(kernels[0]);

void setup()
{
    display.begin();

    if (!display.sdCardInit())
    {
        display.println("SD card error!");
        display.display();
        return;
    }

    for (uint8_t i = 0; i < kernelCount; i++)
    {
        display.clearDisplay();
        display.setCursor(0, 0);
        display.setTextColor(BLACK);
        display.setTextSize(2);
        display.print("Kernel: ");
        display.println(kernelNames[i]);

        // Select the kernel used for all subsequent dithered draws
        display.image.setDitherKernel(kernels[i]);

        if (!display.image.draw("picture1.jpg", 0, 30, true))
        {
            display.println("Image open error");
        }

        display.display();
        delay(5000);
    }

    display.sdCardSleep();
}

void loop()
{
    // Nothing...
}

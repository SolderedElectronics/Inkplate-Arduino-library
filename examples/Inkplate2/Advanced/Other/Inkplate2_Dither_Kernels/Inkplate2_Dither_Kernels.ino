/**
 **************************************************
 * @file        Inkplate2_Dither_Kernels.ino
 * @brief       Switch dithering kernels when drawing an image on Soldered Inkplate 2.
 *
 * @details     Demonstrates how to select a different error-diffusion dithering
 *              kernel before drawing an image. The kernel controls how the error
 *              from reducing the source image to Inkplate 2's 3-color (black/white/red)
 *              palette is spread to neighboring pixels - different kernels trade off
 *              sharpness, noise and speed.
 *
 *              Inkplate 2 has no SD card slot, so the image is fetched over WiFi
 *              instead of read from an SD card.
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
 * - Board:      Soldered Inkplate 2
 * - Hardware:   Inkplate 2, USB cable
 * - Extra:      WiFi connection + Internet access
 * - Serial:     115200 baud (optional, for connection logs)
 *
 * How to use:
 * 1) In Boards Manager -> Inkplate Boards, select "Soldered Inkplate2"
 *    from Tools -> Board.
 * 2) Enter your WiFi SSID and password (ssid, password).
 * 3) Upload the sketch to Inkplate 2.
 * 4) The image is downloaded and drawn once per kernel, refreshing the display
 *    between each one.
 *
 * @note        Quick start guide:
 *              https://docs.soldered.com/inkplate/2/quick-start-guide/
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
#ifndef ARDUINO_INKPLATE2
#error "Wrong board selection for this example, please select Soldered Inkplate2 in the boards menu."
#endif

#include "Inkplate.h"
#include "WiFi.h"

Inkplate display;

const char ssid[] = "";     // Your WiFi SSID
const char password[] = ""; // Your WiFi password

const char imageUrl[] = "https://raw.githubusercontent.com/SolderedElectronics/Inkplate-Arduino-library/"
                         "master/examples/Inkplate2/Advanced/WEB_WiFi/"
                         "Inkplate2_Show_Pictures_From_Web/mountain.png";

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
    Serial.begin(115200);
    display.begin();
    display.clearDisplay();

    display.setCursor(10, 10);
    display.setTextSize(2);
    display.setTextColor(INKPLATE2_BLACK);

    WiFi.mode(WIFI_MODE_STA);
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(1000);
    }
    Serial.println("Connected!");

    for (uint8_t i = 0; i < kernelCount; i++)
    {
        // Select the kernel used for all subsequent dithered draws
        display.image.setDitherKernel(kernels[i]);

        display.clearDisplay();
        if (!display.image.draw(imageUrl, 0, 0, true, false))
        {
            display.println("Image open error");
        }
        display.display();

        Serial.print("Kernel shown: ");
        Serial.println(kernelNames[i]);
        delay(8000);
    }

    WiFi.mode(WIFI_OFF);
}

void loop()
{
    // Nothing...
}

#include "Inkplate.h"
#include "SdFat.h"

#include "sample1bit.h"
#include "sample3bit.h"

Inkplate display(INKPLATE_1BIT);

#define DELAYMS 1000

const char *images[] = {"1bit.bmp", "4bit.bmp", "8bit.bmp", "16bit.bmp", "24bit.bmp", "32bit.bmp"};
const char *imageUrls[] = {
    "https://raw.githubusercontent.com/nitko12/Inkplate-revision/master/test/bitmaps/1bit.bmp",
    "https://raw.githubusercontent.com/nitko12/Inkplate-revision/master/test/bitmaps/4bit.bmp",
    "https://raw.githubusercontent.com/nitko12/Inkplate-revision/master/test/bitmaps/8bit.bmp",
    "https://raw.githubusercontent.com/nitko12/Inkplate-revision/master/test/bitmaps/16bit.bmp",
    "https://raw.githubusercontent.com/nitko12/Inkplate-revision/master/test/bitmaps/24bit.bmp",
    "https://raw.githubusercontent.com/nitko12/Inkplate-revision/master/test/bitmaps/32bit.bmp",
};
const bool depth[] = {INKPLATE_1BIT, INKPLATE_3BIT, INKPLATE_3BIT, INKPLATE_3BIT, INKPLATE_3BIT, INKPLATE_3BIT};

void setup()
{
    Serial.begin(115200);

    display.begin();

    display.joinAP("", "");

    Serial.println();
    delay(500);
}

void loop()
{
    display.clearDisplay();
    display.display();


    display.selectDisplayMode(INKPLATE_1BIT);
    display.setTextSize(2);
    display.setTextColor(1);
    display.setCursor(100, 100);
    display.print("Displaying 1 bit flash image.");

    display.display();
    display.clearDisplay();
    delay(5000);

    display.drawBitmap(0, 0, sample1bit, sample1bit_w, sample1bit_h, WHITE, BLACK);

    display.display();
    display.clearDisplay();
    delay(5000);

    display.selectDisplayMode(INKPLATE_3BIT);
    display.setTextSize(2);
    display.setTextColor(1);
    display.setCursor(90, 100);
    display.print("Displaying 3 bit flash image.");

    display.display();
    display.clearDisplay();
    delay(5000);

    display.drawBitmap3Bit(0, 0, sample3bit, sample3bit_w, sample3bit_h);

    display.display();
    display.clearDisplay();
    delay(5000);


    if (!display.sdCardInit())
    {
        display.println("Sd card error!");
        delay(1000);
        return;
    }

    for (int i = 0; i < 4; ++i)
    {
        bool dither = i & 1;
        bool invert = i >> 1;

        for (int j = 0; j < 6; ++j)
        {
            display.selectDisplayMode(depth[j]);
            display.setTextSize(2);
            display.setTextColor(1);
            display.setCursor(100, 100);
            display.print("Displaying ");
            display.print(images[j]);
            if (!dither)
                display.print(" non");
            display.print(" dithered and");
            if (!invert)
                display.print(" non");
            display.print(" inverted.");

            display.display();
            display.clearDisplay();
            delay(5000);

            display.drawBitmapFromSd(images[j], 0, 0, dither, invert);
            display.display();
            display.clearDisplay();
            delay(5000);
        }
    }

    for (int i = 0; i < 4; ++i)
    {
        bool dither = i & 1;
        bool invert = i >> 1;

        for (int j = 0; j < 6; ++j)
        {
            display.selectDisplayMode(depth[j]);
            display.setTextSize(2);
            display.setTextColor(1);
            display.setCursor(100, 100);
            display.print("Displaying ");
            display.print(images[j]);
            display.print(" from web");
            if (!dither)
                display.print(" non");
            display.print(" dithered and");
            if (!invert)
                display.print(" non");
            display.print(" inverted.");

            display.display();
            display.clearDisplay();
            delay(5000);

            display.drawBitmapFromWeb(imageUrls[j], 0, 0, dither, invert);
            display.display();
            display.clearDisplay();
            delay(5000);
        }
    }

    delay(5000);
}
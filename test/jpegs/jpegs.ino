#include "Inkplate.h"
#include "SdFat.h"

#include "sample1bit.h"
#include "sample3bit.h"

Inkplate display(INKPLATE_1BIT);

#define DELAYMS 1000

const char *images[] = {"Lenna.jpg", "Lenna.jpg"};
const char *imageUrls[] = {
    "https://raw.githubusercontent.com/nitko12/Inkplate-revision/master/test/jpegs/Lenna.bmp",
    "https://raw.githubusercontent.com/nitko12/Inkplate-revision/master/test/jpegs/Lenna.bmp",
};
const bool depth[] = {INKPLATE_1BIT, INKPLATE_3BIT};

void setup()
{
    Serial.begin(115200);

    display.begin();

    display.joinAP("e-radionica.com", "croduino");

    Serial.println();
    delay(500);
}

void loop()
{
    display.clearDisplay();
    display.display();

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

        for (int j = 0; j < 2; ++j)
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

        for (int j = 0; j < 2; ++j)
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
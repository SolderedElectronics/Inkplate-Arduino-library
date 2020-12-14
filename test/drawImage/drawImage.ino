/*
drawImage.ino
Inkplate 6 Arduino library
Zvonimir Haramustek @ e-radionica.com
September 24, 2020
https://github.com/e-radionicacom/Inkplate-6-Arduino-library

For support, please reach over forums: forum.e-radionica.com/en
For more info about the product, please check: www.inkplate.io

This code is released under the GNU Lesser General Public License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html
Please review the LICENSE file included with this example.
If you have any questions about licensing, please contact techsupport@e-radionica.com
Distributed as-is; no warranty is given.
*/


#include "Inkplate.h"
#include "SdFat.h"

#include "sample1bit.h"
#include "sample3bit.h"

Inkplate display(INKPLATE_1BIT);

#define DELAYMS 1000

const char *imagesBmp[] = {"1bit.bmp",  "4bit.bmp",  "8bit.bmp",  "16bit.bmp",
                           "24bit.bmp", "32bit.bmp", "Lenna.jpg", "Lenna.jpg"};
const char *imagesBmpUrls[] = {
    "https://raw.githubusercontent.com/nitko12/Inkplate-revision/master/test/bitmaps/1bit.bmp",
    "https://raw.githubusercontent.com/nitko12/Inkplate-revision/master/test/bitmaps/4bit.bmp",
    "https://raw.githubusercontent.com/nitko12/Inkplate-revision/master/test/bitmaps/8bit.bmp",
    "https://raw.githubusercontent.com/nitko12/Inkplate-revision/master/test/bitmaps/16bit.bmp",
    "https://raw.githubusercontent.com/nitko12/Inkplate-revision/master/test/bitmaps/24bit.bmp",
    "https://raw.githubusercontent.com/nitko12/Inkplate-revision/master/test/bitmaps/32bit.bmp",
    "https://raw.githubusercontent.com/nitko12/Inkplate-revision/master/test/jpegs/Lenna.jpg",
    "https://raw.githubusercontent.com/nitko12/Inkplate-revision/master/test/jpegs/Lenna.jpg",
};
const bool depth[] = {INKPLATE_1BIT, INKPLATE_3BIT, INKPLATE_3BIT, INKPLATE_3BIT,
                      INKPLATE_3BIT, INKPLATE_3BIT, INKPLATE_1BIT, INKPLATE_3BIT};

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
    // display.clearDisplay();
    // display.display();

    // display.selectDisplayMode(INKPLATE_1BIT);
    // display.setTextSize(2);
    // display.setTextColor(1);
    // display.setCursor(100, 100);
    // display.print("Displaying 1 bit flash image.");

    // display.display();
    // display.clearDisplay();
    // delay(5000);

    // display.drawImage(sample1bit, 0, 0, sample1bit_w, sample1bit_h, WHITE, BLACK);

    // display.display();
    // display.clearDisplay();
    // delay(5000);

    // display.selectDisplayMode(INKPLATE_3BIT);
    // display.setTextSize(2);
    // display.setTextColor(1);
    // display.setCursor(90, 100);
    // display.print("Displaying 3 bit flash image.");

    // display.display();
    // display.clearDisplay();
    // delay(5000);

    // display.drawImage(sample3bit, 0, 0, sample3bit_w, sample3bit_h);

    // display.display();
    // display.clearDisplay();
    // delay(5000);


    // if (!display.sdCardInit())
    // {
    //     display.println("Sd card error!");
    //     delay(1000);
    // }

    for (int i = 0; i < 4; ++i)
    {
        bool dither = i & 1;
        bool invert = i >> 1;

        // for (int i = 0; i < 4; ++i)
        // {
        //     bool dither = i & 1;
        //     bool invert = i >> 1;

        //     display.selectDisplayMode(INKPLATE_1BIT);
        //     display.setTextSize(2);
        //     display.setTextColor(1);
        //     display.setCursor(100, 100);
        //     display.print("Displaying Lenna.jpg 1bit");
        //     if (!dither)
        //         display.print(" non");
        //     display.print(" dithered and");
        //     if (!invert)
        //         display.print(" non");
        //     display.print(" inverted.");

        //     display.display();
        //     display.clearDisplay();
        //     delay(5000);

        //     display.drawImage("Lenna.jpg", 0, 0, dither, invert);
        //     display.display();
        //     display.clearDisplay();
        //     delay(5000);
        // }

        // for (int i = 0; i < 4; ++i)
        // {
        //     bool dither = i & 1;
        //     bool invert = i >> 1;

        //     display.selectDisplayMode(INKPLATE_3BIT);
        //     display.setTextSize(2);
        //     display.setTextColor(1);
        //     display.setCursor(100, 100);
        //     display.print("Displaying Lenna.jpg 3bit");
        //     if (!dither)
        //         display.print(" non");
        //     display.print(" dithered and");
        //     if (!invert)
        //         display.print(" non");
        //     display.print(" inverted.");

        //     display.display();
        //     display.clearDisplay();
        //     delay(5000);

        //     display.drawImage("Lenna.jpg", 0, 0, dither, invert);
        //     display.display();
        //     display.clearDisplay();
        //     delay(5000);
        // }

        // for (int i = 0; i < 4; ++i)
        // {
        //     bool dither = i & 1;
        //     bool invert = i >> 1;

        //     display.selectDisplayMode(INKPLATE_1BIT);
        //     display.setTextSize(2);
        //     display.setTextColor(1);
        //     display.setCursor(100, 100);
        //     display.print("Displaying png.png 1bit");
        //     if (!dither)
        //         display.print(" non");
        //     display.print(" dithered and");
        //     if (!invert)
        //         display.print(" non");
        //     display.print(" inverted.");

        //     display.display();
        //     display.clearDisplay();
        //     delay(5000);

        //     display.drawImage("png.png", 0, 0, dither, invert);
        //     display.display();
        //     display.clearDisplay();
        //     delay(5000);
        // }

        // for (int i = 0; i < 4; ++i)
        // {
        //     bool dither = i & 1;
        //     bool invert = i >> 1;

        //     display.selectDisplayMode(INKPLATE_3BIT);
        //     display.setTextSize(2);
        //     display.setTextColor(1);
        //     display.setCursor(100, 100);
        //     display.print("Displaying png.png 3bit");
        //     if (!dither)
        //         display.print(" non");
        //     display.print(" dithered and");
        //     if (!invert)
        //         display.print(" non");
        //     display.print(" inverted.");

        //     display.display();
        //     display.clearDisplay();
        //     delay(5000);

        //     display.drawImage("png.png", 0, 0, dither, invert);
        //     display.display();
        //     display.clearDisplay();
        //     delay(5000);
        // }

        // web

        display.selectDisplayMode(INKPLATE_1BIT);
        display.setTextSize(2);
        display.setTextColor(1);
        display.setCursor(100, 100);
        display.print("Displaying Lenna.jpg 1bit from web");
        if (!dither)
            display.print(" non");
        display.print(" dithered and");
        if (!invert)
            display.print(" non");
        display.print(" inverted.");

        display.display();
        display.clearDisplay();
        delay(5000);

        Serial.println(display.drawImage("https://raw.githubusercontent.com/e-radionicacom/Inkplate-Arduino-library/"
                                         "inkplate10-integration/test/drawImage/test.jpg",
                                         0, 0, dither, invert));
        display.display();
        display.clearDisplay();
        delay(5000);
    }

    for (int i = 0; i < 4; ++i)
    {
        bool dither = i & 1;
        bool invert = i >> 1;

        display.selectDisplayMode(INKPLATE_3BIT);
        display.setTextSize(2);
        display.setTextColor(1);
        display.setCursor(100, 100);
        display.print("Displaying Lenna.jpg 3bit from web");
        if (!dither)
            display.print(" non");
        display.print(" dithered and");
        if (!invert)
            display.print(" non");
        display.print(" inverted.");

        display.display();
        display.clearDisplay();
        delay(5000);

        Serial.println(display.drawImage("https://raw.githubusercontent.com/e-radionicacom/Inkplate-Arduino-library/"
                                         "inkplate10-integration/test/drawImage/test.jpg",
                                         0, 0, dither, invert));
        display.display();
        display.clearDisplay();
        delay(5000);
    }

    // for (int i = 0; i < 4; ++i)
    // {
    //     bool dither = i & 1;
    //     bool invert = i >> 1;

    //     display.selectDisplayMode(INKPLATE_1BIT);
    //     display.setTextSize(2);
    //     display.setTextColor(1);
    //     display.setCursor(100, 100);
    //     display.print("Displaying png.png 1bit from web");
    //     if (!dither)
    //         display.print(" non");
    //     display.print(" dithered and");
    //     if (!invert)
    //         display.print(" non");
    //     display.print(" inverted.");

    //     display.display();
    //     display.clearDisplay();
    //     delay(5000);

    //     display.drawImage("https://raw.githubusercontent.com/e-radionicacom/Inkplate-6-Arduino-library/revision/test/"
    //                       "drawImage/png.png",
    //                       0, 0, dither, invert);
    //     display.display();
    //     display.clearDisplay();
    //     delay(5000);
    // }

    // for (int i = 0; i < 4; ++i)
    // {
    //     bool dither = i & 1;
    //     bool invert = i >> 1;

    //     display.selectDisplayMode(INKPLATE_3BIT);
    //     display.setTextSize(2);
    //     display.setTextColor(1);
    //     display.setCursor(100, 100);
    //     display.print("Displaying png.png 3bit from web");
    //     if (!dither)
    //         display.print(" non");
    //     display.print(" dithered and");
    //     if (!invert)
    //         display.print(" non");
    //     display.print(" inverted.");

    //     display.display();
    //     display.clearDisplay();
    //     delay(5000);

    //     display.drawImage("https://raw.githubusercontent.com/e-radionicacom/Inkplate-6-Arduino-library/revision/test/"
    //                       "drawImage/png.png",
    //                       0, 0, dither, invert);
    //     display.display();
    //     display.clearDisplay();
    //     delay(5000);
    // }


    // for (int i = 0; i < 4; ++i)
    // {
    //     bool dither = i & 1;
    //     bool invert = i >> 1;

    //     for (int j = 0; j < 6; ++j)
    //     {
    //         display.selectDisplayMode(depth[j]);
    //         display.setTextSize(2);
    //         display.setTextColor(1);
    //         display.setCursor(100, 100);
    //         display.print("Displaying ");
    //         display.print(imagesBmp[j]);
    //         display.print(" from web");
    //         if (!dither)
    //             display.print(" non");
    //         display.print(" dithered and");
    //         if (!invert)
    //             display.print(" non");
    //         display.print(" inverted.");

    //         display.display();
    //         display.clearDisplay();
    //         delay(5000);

    //         display.drawImage(imagesBmpUrls[j], 0, 0, dither, invert);
    //         display.display();
    //         display.clearDisplay();
    //         delay(5000);
    //     }
    // }

    delay(5000);
}
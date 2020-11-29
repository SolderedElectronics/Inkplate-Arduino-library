/*
jpegs.ino
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

Inkplate display(INKPLATE_1BIT);

#define DELAYMS 1000

const char *images[] = {"Lenna.jpg", "Lenna.jpg"};
const char *imageUrls[] = {
    "https://raw.githubusercontent.com/nitko12/Inkplate-revision/master/test/jpegs/Lenna.jpg",
    "https://raw.githubusercontent.com/nitko12/Inkplate-revision/master/test/jpegs/Lenna.jpg",
};
const bool depth[] = {INKPLATE_1BIT, INKPLATE_3BIT};

void setup()
{
    Serial.begin(115200);

    display.begin();

    display.joinAP("TP-Link_66E5", "35503394");

    Serial.println();
    delay(500);
}

void loop()
{
    display.clearDisplay();
    display.display();

    // if (!display.sdCardInit())
    // {
    //     display.println("Sd card error!");
    //     delay(1000);
    //     return;
    // }

    // for (int i = 0; i < 4; ++i)
    // {
    //     bool dither = i & 1;
    //     bool invert = i >> 1;

    //     for (int j = 0; j < 2; ++j)
    //     {
    //         display.selectDisplayMode(depth[j]);
    //         display.setTextSize(2);
    //         display.setTextColor(1);
    //         display.setCursor(100, 100);
    //         display.print("Displaying ");
    //         display.print(images[j]);
    //         if (!dither)
    //             display.print(" non");
    //         display.print(" dithered and");
    //         if (!invert)
    //             display.print(" non");
    //         display.print(" inverted.");

    //         display.display();
    //         display.clearDisplay();
    //         delay(5000);

    //         Serial.println(display.drawJpegFromSd(images[j], 0, 0, dither, invert));
    //         display.display();
    //         display.clearDisplay();
    //         delay(5000);
    //     }
    // }

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

            display.drawJpegFromWeb(imageUrls[j], 0, 0, dither, invert);
            display.display();
            display.clearDisplay();
            delay(5000);
        }
    }

    delay(5000);
}
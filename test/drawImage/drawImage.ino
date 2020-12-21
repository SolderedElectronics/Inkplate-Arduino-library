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

#include "sample1bit.h"
#include "sample3bit.h"

// zsh script for converting filetypes: for x (./jpg/*); do convert $x -depth 24 ./bmp24bit/${x:r:t}.bmp; done

Inkplate display(INKPLATE_3BIT);

const int testImgHeight = 60, testImgWidth = 60;
const int n = E_INK_HEIGHT / testImgHeight, m = E_INK_WIDTH / testImgWidth;

char *formatFolders[] = {"bmp1bit", "bmp4bit", "bmp8bit", "bmp16bit", "bmp24bit", "bmp32bit", "jpg", "png"};
char *formatExtension[] = {"bmp", "bmp", "bmp", "bmp", "bmp", "bmp", "jpg", "png"};

char *formatStr = "https://raw.githubusercontent.com/e-radionicacom/Inkplate-Arduino-library/"
                  "inkplate10-integration/test/drawImage/imageGrid/%s/tile%03d.%s";

void setup()
{
    Serial.println("Commencing test...");
    delay(100);

    Serial.begin(115200);

    display.begin();
    display.joinAP("e-radionica.com", "croduino");
}

void loop()
{
    // -------- BW ---------
    display.clearDisplay();
    for (int i = 3; i < 128; ++i)
    {
        Serial.println(i);
        delay(100);

        bool web = i & 1;            // or sd
        bool dither = i & 2;         // or not to dither
        bool invert = i & 4;         // or not to invert
        bool autoFormat = i & 8;     // or not to auto format
        int format = (i & 112) >> 4; // 1bitBMP 4bitBMP 8bitBMP 16bitBMP 24bitBMP 32bitBMP PNG JPG

        int x = i % m, y = i / m;

        char url[256];
        sprintf(url, formatStr, "jpg", i, "jpg");

        Serial.println(url);
        Serial.println(display.drawImage(url, x * 60, y * 60, 1, 0));
        Serial.printf("%d %d\n", x * 60, y * 60);

        display.display();
    }

    display.display();
    delay(5000);

    // ------- gray --------
    // for (int i = 0; i < 128; ++i)
    // {
    //     bool web = i & 1;        // or sd
    //     bool dither = i & 2;     // or not to dither
    //     bool invert = i & 4;     // or not to invert
    //     bool autoFormat = i & 8; // or not to auto format
    //     bool format = i & 112;   // 1bitBMP 4bitBMP 8bitBMP 16bitBMP 24bitBMP 32bitBMP PNG JPG

    //     drawImage();

    //     int x = i % m, y = i / m;
    // }

    display.display();
    delay(5000);
}
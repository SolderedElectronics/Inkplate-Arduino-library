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

#include "tile129.h"
#include "tile129_g.h"
#include "tile130.h"
#include "tile130_g.h"

#include "sample1bit.h"
#include "sample3bit.h"

// zsh script for converting filetypes: for x (./jpg/*); do convert $x -depth 24 ./bmp24bit/${x:r:t}.bmp; done
// for x (./jpg/*); do ffmpeg -i $x  -pix_fmt rgb565 -y ./bmp16bit/${x:r:t}.bmp; done

Inkplate display(INKPLATE_3BIT);

const int testImgHeight = 60, testImgWidth = 60;
const int n = E_INK_HEIGHT / testImgHeight, m = E_INK_WIDTH / testImgWidth;

char *formatExtension[] = {"bmp", "bmp", "bmp", "bmp", "bmp", "bmp", "jpg", "png"};

char *formatStrWeb = "https://raw.githubusercontent.com/e-radionicacom/Inkplate-Arduino-library/"
                     "inkplate10-integration/test/drawImage/tile%d.%s";

Image::Format formats[] = {Image::BMP, Image::BMP, Image::BMP, Image::BMP,
                           Image::BMP, Image::BMP, Image::JPG, Image::PNG};

char *formatStrSd = "tile%d.%s";

void drawAll();

void setup()
{
    Serial.println("Commencing test...");
    delay(100);

    Serial.begin(115200);

    display.begin();
    display.joinAP("e-radionica.com", "croduino");
    display.sdCardInit();
}

void loop()
{
    // -------- BW ---------
    display.setDisplayMode(INKPLATE_1BIT);
    drawAll();

    delay(5000);

    display.setDisplayMode(INKPLATE_3BIT);
    drawAll();

    delay(5000);
}

void drawAll()
{
    display.clearDisplay();
    for (int i = 0; i < 128; ++i)
    {
        bool web = i & 1;            // or sd
        bool dither = i & 2;         // or not to dither
        bool invert = i & 4;         // or not to invert
        bool autoFormat = i & 8;     // or not to auto format
        int format = (i & 112) >> 4; // 1bitBMP 4bitBMP 8bitBMP 16bitBMP 24bitBMP 32bitBMP PNG JPG

        int x = i % m, y = i / m;

        char url[256];
        sprintf(url, formatStrWeb, formatFolders[format], i + 1, formatExtension[format]);
        char path[128];
        sprintf(path, formatStrSd, formatFolders[format], i + 1, formatExtension[format]);

        Serial.println(web ? url : path);
        Serial.printf("dithered: %d inverted: %d\n", dither, invert);

        if (autoFormat)
        {
            Serial.println(display.drawImage(web ? url : path, x * 60, y * 60, dither, invert));
        }
        else
        {
            Serial.println(display.drawImage(web ? url : path, formats[format], x * 60, y * 60, dither, invert));
        }
        Serial.printf("%d %d\n", x * 60, y * 60);
        display.display();
    }

    int x = (128 % m) * 60, y = (128 / m) * 60;
    if (display.getDisplayMode() == INKPLATE_1BIT)
        display.drawImage(tile129, x, y, tile129_w, tile129_w);
    else
        display.drawImage(tile129_g, x, y, tile129_g_w, tile129_g_w);
    display.display();
    delay(3000);

    x = (129 % m) * 60, y = (129 / m) * 60;
    if (display.getDisplayMode() == INKPLATE_1BIT)
        display.drawImage(tile130, x, y, tile130_w, tile130_w, BLACK);
    else
        display.drawImage(tile130_g, x, y, tile130_g_w, tile130_g_w);
    display.display();
    delay(3000);
}
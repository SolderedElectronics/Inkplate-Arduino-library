/*
drawImage.ino
Inkplate 6 Arduino library
Zvonimir Haramustek @ e-radionica.com
January 4, 2021
https://github.com/e-radionicacom/Inkplate-6-Arduino-library

For support, please reach over forums: forum.e-radionica.com/en
For more info about the product, please check: www.inkplate.io

This code is released under the GNU Lesser General Public License v3.0: https://www.gnu.org/licenses/lgpl-3.0.en.html
Please review the LICENSE file included with this example.
If you have any questions about licensing, please contact techsupport@e-radionica.com
Distributed as-is; no warranty is given.
*/


#include "Inkplate.h"

#include "img.h"
#include "img_g.h"

#define SEQUENTIAL

// zsh script for converting filetypes: for x (./jpg/*); do convert $x -depth 24 ./bmp24bit/${x:r:t}.bmp; done
// for x (./jpg/*); do ffmpeg -i $x  -pix_fmt rgb565 -y ./bmp16bit/${x:r:t}.bmp; done

Inkplate display(INKPLATE_3BIT);

#ifdef ARDUINO_ESP32_DEV
const int testImgHeight = 60, testImgWidth = 60;
char *filename[] = {"img1bit.bmp",  "img4bit.bmp",  "img8bit.bmp", "img16bit.bmp",
                    "img24bit.bmp", "img32bit.bmp", "img.jpg",     "img.png"};
#else // Inkplate10
const int testImgHeight = 92, testImgWidth = 92;
char *filename[] = {"img_2_1bit.bmp",  "img_2_4bit.bmp",  "img_2_8bit.bmp", "img_2_16bit.bmp",
                    "img_2_24bit.bmp", "img_2_32bit.bmp", "img_2_jpg",      "img_2.png"};
#endif

const int n = E_INK_HEIGHT / testImgHeight, m = E_INK_WIDTH / testImgWidth;
char *formatStrWeb = "https://raw.githubusercontent.com/e-radionicacom/Inkplate-Arduino-library/"
                     "inkplate10-integration/test/drawImage/%s";

Image::Format formats[] = {Image::BMP, Image::BMP, Image::BMP, Image::BMP,
                           Image::BMP, Image::BMP, Image::JPG, Image::PNG};

char *formatStrSd = "%s";

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
        sprintf(url, formatStrWeb, filename[format]);
        char path[128];
        sprintf(path, formatStrSd, filename[format]);

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
#ifdef SEQUENTIAL
        display.display();
#endif
    }

    int x = (128 % m) * 60, y = (128 / m) * 60;
    if (display.getDisplayMode() == INKPLATE_1BIT)
        display.drawImage(img, x, y, img_w, img_w);
    else
        display.drawImage(img_g, x, y, img_g_w, img_g_w);
#ifdef SEQUENTIAL
    display.display();
#endif
    delay(3000);

    x = (129 % m) * 60, y = (129 / m) * 60;
    if (display.getDisplayMode() == INKPLATE_1BIT)
        display.drawImage(img, x, y, img_w, img_w, BLACK);
    else
        display.drawImage(img_g, x, y, img_g_w, img_g_w);

    display.display();
    delay(3000);
}
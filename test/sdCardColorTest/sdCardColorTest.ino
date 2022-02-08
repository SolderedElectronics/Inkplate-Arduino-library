
#ifndef ARDUINO_INKPLATECOLOR
#error "Please select Inkplate 6COLOR from board selection"
#endif

#include "Inkplate.h"
#include "SdFat.h"

Inkplate display;
SdFile file;

#define BMP_TEST 1
#define PNG_TEST 1
#define JPG_TEST 1

int invert = 0;
int dither = 1;
int delayTime = 2000;

void setup()
{
    display.begin();
    Serial.begin(115200);
    Serial.print("Sd card init state: ");
    Serial.println(display.sdCardInit());
    display.clearDisplay();
    display.display();

    if (BMP_TEST)
    {
        /*
        Serial.println("1 bit mono bmp.");
        display.drawImage("parrots_mono.bmp", 0, 0, dither, invert);
        display.display();
        delay(delayTime);

        Serial.println("1 bit indexed bmp.");
        display.drawImage("parrots1_indexed.bmp", 0, 0, dither, invert);
        display.display();
        delay(delayTime);
        */
        Serial.println("4 bit indexed bmp.");
        display.drawImage("parrots4_indexed.bmp", 0, 0, dither, invert);
        display.display();
        delay(delayTime);

        Serial.println("8 bit indexed bmp.");
        display.drawImage("parrots8_indexed.bmp", 0, 0, dither, invert);
        display.display();
        delay(delayTime);

        Serial.println("16 bit 5-6-5 RGB bmp.");
        display.drawImage("parrots16_5-6-5_RGB.bmp", 0, 0, dither, invert);
        display.display();
        delay(delayTime);

        Serial.println("16 bit 5-5-5-1 RGB bmp.");
        display.drawImage("parrots16_5-5-5-1_RGB.bmp", 0, 0, dither, invert);
        display.display();
        delay(delayTime);

        Serial.println("16 bit 5-5-5-1 RGBA bmp.");
        display.drawImage("parrots16_5-5-5-1_RGBA.bmp", 0, 0, dither, invert);
        display.display();
        delay(delayTime);

        Serial.println("24_trueColor bmp.");
        display.drawImage("parrots24_trueColor.bmp", 0, 0, dither, invert);
        display.display();
        delay(delayTime);

        Serial.println("32_RGB bmp.");
        display.drawImage("parrots32_RGB.bmp", 0, 0, dither, invert);
        display.display();
        delay(delayTime);

        Serial.println("32_RGBA bmp.");
        display.drawImage("parrots32_RGBA.bmp", 0, 0, dither, invert);
        display.display();
        delay(delayTime);
    }

    if (PNG_TEST)
    {
        Serial.println("mono png.");
        display.drawImage("parrots_mono.png", 0, 0, dither, invert);
        display.display();
        delay(delayTime);

        Serial.println("4 bit png.");
        display.drawImage("parrots4.png", 0, 0, dither, invert);
        display.display();
        delay(delayTime);

        Serial.println("8 bit png.");
        display.drawImage("parrots8.png", 0, 0, dither, invert);
        display.display();
        delay(delayTime);

        Serial.println("24 bit png.");
        display.drawImage("parrots24_RGB.png", 0, 0, dither, invert);
        display.display();
        delay(delayTime);

        Serial.println("32 RGBA png.");
        display.drawImage("parrots32_RGBA.png", 0, 0, dither, invert);
        display.display();
        delay(delayTime);

        Serial.println("48 RGBA png.");
        display.drawImage("parrots48_RGB.png", 0, 0, dither, invert);
        display.display();
        delay(delayTime);

        Serial.println("64 RGBA png.");
        display.drawImage("parrots64_RGBA.png", 0, 0, dither, invert);
        display.display();
        delay(delayTime);
    }

    if (JPG_TEST)
    {
        Serial.println("grayscale jpg.");
        display.drawImage("parrots_grayscale.jpg", 0, 0, dither, invert);
        display.display();
        delay(delayTime);

        Serial.println("24 true color jpg.");
        display.drawImage("parrots24_trueColor.jpg", 0, 0, dither, invert);
        display.display();
        delay(delayTime);

        Serial.println("32 trueColor jpg.");
        display.drawImage("parrots32_trueColor.jpg", 0, 0, dither, invert);
        display.display();
        delay(delayTime);
    }

    Serial.println("paintball");
    display.drawImage("paintball.jpg", 0, 0, dither, invert);
    display.display();
    delay(delayTime);
}

void loop()
{
}
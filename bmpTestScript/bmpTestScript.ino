#include "Inkplate.h"
#include "SdFat.h"  

Inkplate display(INKPLATE_3BIT);
SdFile file;

void setup() {
    Serial.begin(115200);

    display.begin();
    display.clearDisplay();
    display.display();
}

void loop() {
    if (display.sdCardInit()) {
        Serial.println("1");
        display.println("SD Card OK! Reading image...");
        display.partialUpdate();

        if (!display.drawBitmapFromSD("1bit.bmp", 0, 0, 1)) {
            display.println("Image open error");
            display.display();
        }
        display.display();
    }

    delay(5000);
    display.clearDisplay();
    display.clean();
    display.display();

    if (display.sdCardInit()) {
        Serial.println("2");
        display.println("SD Card OK! Reading image...");
        display.partialUpdate();

        if (!display.drawBitmapFromSD("4bit.bmp", 0, 0)) {
            display.println("Image open error");
            display.display();
        }
        display.display();
    }

    delay(5000);
    display.clearDisplay();
    display.clean();
    display.display();

    if (display.sdCardInit()) {
        Serial.println("3");
        display.println("SD Card OK! Reading image...");
        display.partialUpdate();

        if (!display.drawBitmapFromSD("8bit.bmp", 0, 0)) {
            display.println("Image open error");
            display.display();
        }
        display.display();
    }

    delay(5000);
    display.clearDisplay();
    display.clean();
    display.display();

    if (display.sdCardInit()) {
        Serial.println("4");
        display.println("SD Card OK! Reading image...");
        display.partialUpdate();

        if (!display.drawBitmapFromSD("24bit.bmp", 0, 0)) {
            display.println("Image open error");
            display.display();
        }
        display.display();
    }

    delay(5000);
    display.clearDisplay();
    display.clean();
    display.display();
}
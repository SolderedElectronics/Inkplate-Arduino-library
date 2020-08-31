/*
   5_Inkplate_SD_JPEG_pictures example for e-radionica Inkplate6
   For this example you will need a micro USB cable, Inkplate6 and a SD card loaded with
   pyramid.jpg file that can be found inside folder of this example.
   Select "Inkplate 6(ESP32)" from Tools -> Board menu.
   Don't have "Inkplate 6(ESP32)" option? Follow our tutorial and add it:
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   To work with SD card on Inkplate, you will need to add one extra library.
   Download and install it from here: https://github.com/e-radionicacom/Inkplate-6-SDFat-Arduino-Library

   You can open .jpg files that have resoluton smaller than 800x600 or otherwise it won't fit on screen.
   Format your SD card in standard FAT fileformat.

   This example will show you how you can read a .jpg file (picture) from SD card and
   display that image on e-paper display.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   31 August 2020 by e-radionica.com
*/

#include "Inkplate.h"               //Include Inkplate library to the sketch
#include "SdFat.h"                  //Include library for SD card
Inkplate display(INKPLATE_1BIT);    //Create an object on Inkplate library and also set library into 1 Bit mode (Monochrome)
SdFile file;                        //Create SdFile object used for accessing files on SD card

void setup() {
    Serial.begin(115200);

    display.begin();                  //Init Inkplate library (you should call this function ONLY ONCE)
    display.clearDisplay();           //Clear frame buffer of display
    display.display();                //Put clear image on display

    //Init SD card. Display if SD card is init propery or not.
    if (display.sdCardInit()) {
        display.println("SD Card OK! Reading image...");
        display.partialUpdate();

        //If card is properly init, try to load image and display it on e-paper at position X=100, Y=0
        //NOTE: Both drawJpegFromSD methods allow for an optional fourth "invert" parameter. Setting this parameter to true
        //will flip all colors on the image, making black white and white black.
        if (!display.drawJpegFromSD(&display, "pyramid.jpg", 100, 0, 0, 1)) {
            //If is something failed (wrong filename or wrong format), write error message on the screen.
            //You can turn off dithering for somewhat faster image load by changing the last 1 to 0, or removing the 1 argument completely
            display.println("Image open error");
            display.display();
        }
        display.display();
    }
    else {
        //If SD card init not success, display error on screen and stop the program (using infinite loop)
        display.println("SD Card error!");
        display.partialUpdate();
        while (true);
    }
}

void loop() {
    //Nothing...
}

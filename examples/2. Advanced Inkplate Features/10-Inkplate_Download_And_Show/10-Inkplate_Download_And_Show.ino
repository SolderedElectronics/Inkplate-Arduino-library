/*
   10_Inkplate_Download_And_Show example for e-radionica Inkplate6
   For this example you will need a micro USB cable, Inkplate6, an SD card and an
   available WiFi connection.
   Select "Inkplate 6(ESP32)" from Tools -> Board menu.
   Don't have "Inkplate 6(ESP32)" option? Follow our tutorial and add it: 
   https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/

   To work with SD card on Inkplate, you will need to add one extra library.
   Download and install it from here: https://github.com/e-radionicacom/Inkplate-6-SDFat-Arduino-Library
   
   You can open .bmp files that have color depth of 1 bit (monochrome bitmap) and 
   24 bit AND have resoluton smaller than 800x600 or otherwise it won't fit on screen.

   This example will show you how you can download a .bmp file (picture) from the web to the SD card and 
   display that image on e-paper display.

   Want to learn more about Inkplate? Visit www.inkplate.io
   Looking to get support? Write on our forums: http://forum.e-radionica.com/en/
   23 July 2020 by e-radionica.com
*/

#include "Inkplate.h"               //Include Inkplate library to the sketch
#include "WiFi.h"                   //Include library for WiFi
Inkplate display(INKPLATE_1BIT);    //Create an object on Inkplate library and also set library into 1 Bit mode (Monochrome)
SdFile file;                        //Create SdFile object used for accessing files on SD card

const char* ssid     = "ssid";      //Your WiFi SSID
const char* password = "password";  //Your WiFi password

void setup() {
  display.begin();        //Init Inkplate library (you should call this function ONLY ONCE)
  display.clearDisplay(); //Clear frame buffer of display
  display.display();      //Put clear image on display

  display.print("Connecting to WiFi...");
  display.partialUpdate();

  //Connect to the WiFi network.
  WiFi.mode(WIFI_MODE_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    display.print(".");
    display.partialUpdate();
  }
  display.println("\nWiFi OK! Downloading...");
  display.partialUpdate();

  //Draw the first image from web.
  //24 bit example, will take around 20 secs to load.
  //Photo taken by: Roberto Fernandez
  if(!display.drawBitmapFromWeb("https://varipass.org/neowise.bmp", 0, 0)) {
    //If is something failed (wrong filename or wrong bitmap format), write error message on the screen.
    //REMEMBER! You can only use Windows Bitmap file with color depth of 1 or 24 bits with no compression! 
    display.println("Image open error");
    display.display();
  }
  display.display();

  delay(5000);

  //Draw the second image from web.
  //1 bit example, will load very quickly.
  //Photo taken by: Roberto Fernandez
  if(!display.drawBitmapFromWeb("https://varipass.org/neowise_mono.bmp", 0, 0)) {
    //If is something failed (wrong filename or wrong bitmap format), write error message on the screen.
    //REMEMBER! You can only use Windows Bitmap file with color depth of 1 or 24 bits with no compression! 
    display.println("Image open error");
    display.display();
  }
  display.display();

  WiFi.mode(WIFI_OFF);
}

void loop() {
  //Nothing...
}

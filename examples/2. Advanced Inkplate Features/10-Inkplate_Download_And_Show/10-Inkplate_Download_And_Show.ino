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
#include "HTTPClient.h"             //Include library for HTTPClient
#include "WiFi.h"                   //Include library for WiFi
Inkplate display(INKPLATE_1BIT);    //Create an object on Inkplate library and also set library into 1 Bit mode (Monochrome)
SdFile file;                        //Create SdFile object used for accessing files on SD card

const char* ssid     = "YourWiFiSSID"; //Your WiFi SSID
const char* password = "YourPass";     //Your WiFi password

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
  //Monochromatic bitmap with 1 bit depth. Images like this load quickest.
  //NOTE: Both drawBitmapFromWeb methods allow for an optional fourth "invert" parameter. Setting this parameter to true
  //will flip all colors on the image, making black white and white black. This may be necessary when exporting bitmaps from
  //certain softwares.
  //Photo taken by: Roberto Fernandez
  if(!display.drawBitmapFromWeb("https://varipass.org/neowise_mono.bmp", 0, 0, true)) {
    //If is something failed (wrong filename or wrong bitmap format), write error message on the screen.
    //REMEMBER! You can only use Windows Bitmap file with color depth of 1 or 24 bits with no compression! 
    display.println("Image open error");
    display.display();
  }
  display.display();

  //Draw the second image from web, this time using a HTTPClient to fetch the response manually.
  //Full color 24 bit images are large and take a long time to load, will take around 20 secs.
  HTTPClient http;
  //Set parameters to speed up the download process.
  http.getStream().setNoDelay(true);
  http.getStream().setTimeout(1);
  
  //Photo taken by: Roberto Fernandez
  http.begin("https://varipass.org/neowise.bmp");

  //Check response code.
  int httpCode = http.GET();
  if (httpCode == 200) {
    //Get the response length and make sure it is not 0.
    int32_t len = http.getSize();
    if (len > 0) {
      if(!display.drawBitmapFromWeb(http.getStreamPtr(), 0, 0, len)) {
        //If is something failed (wrong filename or wrong bitmap format), write error message on the screen.
        //REMEMBER! You can only use Windows Bitmap file with color depth of 1 or 24 bits with no compression! 
        display.println("Image open error");
        display.display();
      }
      display.display();
    }
    else {
      display.println("Invalid response length");
      display.display();
    }
  }
  else {
    display.println("HTTP error");
    display.display();
  }
  
  http.end();

  WiFi.mode(WIFI_OFF);
}

void loop() {
  //Nothing...
}

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
#include "SdFat.h"                  //Include library for SD card
#include "WiFi.h"                   //Include library for WiFi
#include "HTTPClient.h"             //Include library for HTTP downloading
Inkplate display(INKPLATE_1BIT);    //Create an object on Inkplate library and also set library into 1 Bit mode (Monochrome)
SdFile file;                        //Create SdFile object used for accessing files on SD card

const char* ssid     = "Twilight Sparkle";  //Your WiFi SSID
const char* password = "thori4twily";  //Your WiFi password

//Photo taken by: Paulian Prajitura
char* url = "https://dl.thorinair.net/neowise2.bmp"; //URL of image to download

void setup() {
  Serial.begin(115200);

  display.begin();        //Init Inkplate library (you should call this function ONLY ONCE)
  display.clearDisplay(); //Clear frame buffer of display
  display.display();      //Put clear image on display

  display.println("Connecting to WiFi...");
  display.partialUpdate();

  //Connect to the WiFi network.
  WiFi.mode(WIFI_MODE_STA);
  WiFi.setSleep(false);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  display.println("WiFi OK! Downloading...");
  display.partialUpdate();

  if(!display.drawBitmapFromWeb(url, 0, 0)) {
    //If is something failed (wrong filename or wrong bitmap format), write error message on the screen.
    //REMEMBER! You can only use Windows Bitmap file with color depth of 1 or 24 bits with no compression! 
    display.println("Image open error");
    display.display();
  }
  display.display();

    //HTTPClient http;
//
    //http.begin(url);
//
    ////Get the image, check if connection was successful.
    //uint32_t httpCode = http.GET();
    //if (httpCode == 200) {
    //  
    //  display.println("HTTP opened! Size: " + String(len) + " - Saving to SD...");
    //  display.partialUpdate();
//
//
//
    //  //String str = http.getString();
    //  //int str_len = http.getSize();
    //  //char char_array[str_len];
    //  //str.toCharArray(char_array, str_len);
    //  //http.end();
//
    //  SdFat sd = display.getSdFat();
    //  sd.remove("image.bmp");
    //  File image = sd.open("image.bmp", O_CREAT | O_WRITE);
    //  if (image) {
//
//
//
    //    http.writeToStream(&stream);
////
    //    //String body = http.getString();
////
    //    //for (int i = 0; i < http.getSize(); i++) {
    //    //  //Serial.print(body[i]);
    //    //  image.write(body[i]);
    //    //}
//
    //    //WiFiClient* stream = http.getStreamPtr();
    //    //while (stream->available()) {
    //    //    uint8_t c = stream->read();
    //    //    //Serial.print(c);
    //    //    image.write(c);
    //    //}
////
    //    ////image.write(char_array, str_len); //Write HTTP content to file
//
    //    //WiFiClient * stream = http.getStreamPtr();
    //    //byte buffer[len];
    //    //stream->readBytes(buffer, len);
    //    //for (int i = 0; i < len; i++)
    //    //  image.write(buffer[i]);
//
    //    //uint32_t pos = 0;
    //    //char buff[2048];
    //    //while (pos < len && http.available()) {
    //    //  ESP.wdtFeed();
    //    //  yield();
    //    //  http.seek(pos, fs::SeekMode::SeekSet);
    //    //  const auto read = http.readBytes(buff, sizeof(buff));
    //    //  pos += read;
    //    //  image.write(buff, read);
    //    //}
//
    //    image.close();     //Close the file
    //    display.println("Image saved!");
    //    display.partialUpdate();
//
    //    delay(1000);
//
    //    //Load and display the image on e-paper at position X=0, Y=0
    //    if(!display.drawBitmapFromSD("image.bmp", 0, 0)) {
    //      //If is something failed (wrong filename or wrong bitmap format), write error message on the screen.
    //      //REMEMBER! You can only use Windows Bitmap file with color depth of 1 or 24 bits with no compression! 
    //      display.println("Image open error");
    //      display.display();
    //    }
    //    display.display();
    //  }
    //  else {
    //    //If image file was not open successfully, display error on screen
    //    display.println("File error!");
    //    display.partialUpdate();
    //    return;
    //  }
    //}
    //else {
    //  //If HTTP connection was not successful, display error on screen
    //  display.println("HTTP connection error!");
    //  display.partialUpdate();
    //  return;
    //}
}

void loop() {
  //Nothing...
}

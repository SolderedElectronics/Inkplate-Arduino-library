#include "Inkplate.h"
#include "image1.h"
#include "image2.h"
#include "picture1.h"
#include "picture2.h" https://www.canva.com/photos/MADGwF-1B54-grayscale-photography-of-concrete-buildings/
#include "driver/rtc_io.h"

#define uS_TO_S_FACTOR 1000000  	//Conversion factor for micro seconds to seconds
#define TIME_TO_SLEEP  20       	//Time ESP32 will go to sleep (in seconds) */
RTC_DATA_ATTR int slide = 0;

Inkplate display(INKPLATE_1BIT);	//Set library to be in 1 Bit display mode by default

/*To convert image to bitmap (Array like this one) use LCD image converter program set to 4 bit grayscale. You can use 4 bit bitmap and display library will scale it to 3 bit bitmap and display it.
* Converting image to bitmap:
* Open lcd-image-converter.exe, when it opens click on New Image, than click OK. Go to Image -> Import... and browse image.
* Click on Options -> Conversion... than select next: Preset: Grayscale 4, on Prepare tab select Type: Grayscale, Main Scan Direction: Top to Bottom, Line Scan Direction: Forward
* On Image tab select Block Size: 8 Bits, Byte Order: Little-Endian
* At the end click on Show Preview and copy all HEX data.
* Paste it inside array (beetwen curly brackets) with type: static const uint8_t picture_name[] PROGMEM = {};, where "picture_name" is name of your array that represents picture.
*/
static const uint8_t smile[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f,
  0x00, 0x00, 0x07, 0xf8, 0x00, 0x00, 0x3f,
  0x00, 0x00, 0x7f, 0xff, 0x80, 0x00, 0x3f,
  0x00, 0x01, 0xff, 0xff, 0xe0, 0x00, 0x3f,
  0x00, 0x07, 0xf8, 0x07, 0xf8, 0x00, 0x3f,
  0x00, 0x0f, 0xc0, 0x00, 0xfc, 0x00, 0x3f,
  0x00, 0x1f, 0x00, 0x00, 0x3e, 0x00, 0x3f,
  0x00, 0x7c, 0x00, 0x00, 0x0f, 0x80, 0x3f,
  0x00, 0x78, 0x00, 0x00, 0x07, 0x80, 0x3f,
  0x00, 0xf0, 0x00, 0x00, 0x03, 0xc0, 0x3f,
  0x01, 0xe0, 0x00, 0x00, 0x01, 0xe0, 0x3f,
  0x03, 0xc0, 0x70, 0x01, 0xc0, 0xf0, 0x3f,
  0x03, 0x80, 0xf8, 0x03, 0xe0, 0x70, 0x3f,
  0x07, 0x80, 0xf8, 0x03, 0xe0, 0x78, 0x3f,
  0x07, 0x00, 0xf8, 0x03, 0xe0, 0x38, 0x3f,
  0x0f, 0x00, 0xf8, 0x03, 0xe0, 0x3c, 0x3f,
  0x0e, 0x00, 0x70, 0x01, 0xc0, 0x1c, 0x3f,
  0x0e, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x3f,
  0x0e, 0x00, 0x00, 0x00, 0x00, 0x1e, 0x3f,
  0x1c, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x3f,
  0x1c, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x3f,
  0x1c, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x3f,
  0x1c, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x3f,
  0x1c, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x3f,
  0x1c, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x3f,
  0x1c, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x3f,
  0x1c, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x3f,
  0x0e, 0x18, 0x00, 0x00, 0x06, 0x1e, 0x3f,
  0x0e, 0x3c, 0x00, 0x00, 0x0f, 0x1c, 0x3f,
  0x0e, 0x1f, 0x00, 0x00, 0x3f, 0x1c, 0x3f,
  0x0f, 0x0f, 0xc0, 0x00, 0xfc, 0x3c, 0x3f,
  0x07, 0x03, 0xf0, 0x03, 0xf0, 0x38, 0x3f,
  0x07, 0x81, 0xfe, 0x3f, 0xe0, 0x78, 0x3f,
  0x03, 0x80, 0x7f, 0xff, 0x80, 0x70, 0x3f,
  0x03, 0xc0, 0x1f, 0xfc, 0x00, 0xf0, 0x3f,
  0x01, 0xe0, 0x01, 0xc0, 0x01, 0xe0, 0x3f,
  0x00, 0xf0, 0x00, 0x00, 0x03, 0xc0, 0x3f,
  0x00, 0x78, 0x00, 0x00, 0x07, 0xc0, 0x3f,
  0x00, 0x7c, 0x00, 0x00, 0x0f, 0x80, 0x3f,
  0x00, 0x1f, 0x00, 0x00, 0x3f, 0x00, 0x3f,
  0x00, 0x0f, 0xc0, 0x00, 0xfc, 0x00, 0x3f,
  0x00, 0x07, 0xf8, 0x07, 0xf8, 0x00, 0x3f,
  0x00, 0x01, 0xff, 0xff, 0xe0, 0x00, 0x3f,
  0x00, 0x00, 0x7f, 0xff, 0x80, 0x00, 0x3f,
  0x00, 0x00, 0x0f, 0xfc, 0x00, 0x00, 0x3f,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f,
};

void setup() {
  display.einkOn();
  display.begin();
  switch (slide) {

    //Display 1 bit per pixel image using Adafruit GFX Library
    case 0:
      //First we have to choose 1 Bit mode
      display.selectDisplayMode(INKPLATE_1BIT);

      //Now we write something to buffer
      display.setTextColor(BLACK, WHITE);
      display.setTextSize(2);
      display.setCursor(0, 0);
      display.print(F("Display some Adafruit GFX Graphics! :)\nEach of this graphics can be easly made with just one line of \nArduino code!"));
      display.setCursor(0, 60);
      display.print("Pixels");
      display.setCursor(160, 60);
      display.print("Text");
      display.setCursor(320, 60);
      display.print("Lines");
      display.setCursor(480, 60);
      display.print("Rectangles");
      display.setCursor(640, 60);
      display.print("Circles");
      display.setCursor(0, 300);
      display.print("Triangles");
      display.setCursor(160, 300);
      display.print("Fill Rects");
      display.setCursor(320, 300);
      display.print("Fill circle");
      display.setCursor(480, 300);
      display.print("Fill triang");
      display.setCursor(640, 300);
      display.print("Bitmaps");

      for (int i = 0; i < 5; i++) {
        display.setTextSize(i + i);
        display.setCursor(120, 80 + (i * 8 * i));
        display.print("Hello");
      }
      for (int i = 0; i < 20; i++) {
        esp_random();
        display.drawPixel(random(0, 100), random(80, 200), BLACK);
        display.drawLine(random(300, 450), random(80, 200), random(300, 450), random(80, 200), BLACK);
        display.drawRect(random(460, 550), random(80, 200), random(10, 50), random(10, 100), BLACK);
        display.drawCircle(random(650, 750), random(80, 200), random(3, 25), BLACK);
      }

      for (int i = 0; i < 7; i++) {
        esp_random();
        display.drawTriangle(random(0, 100), random(330, 470), random(0, 100), random(330, 470), random(0, 100), random(330, 470), BLACK);
        display.fillRect(random(150, 250), random(330, 450), random(10, 50), random(10, 100), BLACK);
        display.fillCircle(random(330, 480), random(330, 550), random(3, 25), BLACK);
        display.fillTriangle(random(500, 620), random(330, 600), random(500, 620), random(330, 600), random(500, 620), random(330, 600), BLACK);
        display.drawBitmap((int)random(620, 750), (int)random(330, 500), smile, 50, 50, BLACK);
      }

      break;

    case 1:
      //Now we switch to 3 Bit mode
      display.selectDisplayMode(INKPLATE_3BIT);

      //Now we write something to buffer
      display.setTextColor(0, 15);
      display.setTextSize(2);
      display.setCursor(0, 10);
      display.print(F("Display some Adafruit GFX Graphics! :) Each of this graphics can\nbe easly made with just one line of \nArduino code! BUT now in GRAYSCALE!"));
      display.setCursor(0, 60);
      display.print("Pixels");
      display.setCursor(160, 60);
      display.print("Text");
      display.setCursor(320, 60);
      display.print("Lines");
      display.setCursor(480, 60);
      display.print("Rectangles");
      display.setCursor(640, 60);
      display.print("Circles");
      display.setCursor(0, 300);
      display.print("Triangles");
      display.setCursor(160, 300);
      display.print("Fill Rects");
      display.setCursor(320, 300);
      display.print("Fill circle");
      display.setCursor(480, 300);
      display.print("Fill triang");
      display.setCursor(640, 300);
      display.print("Bitmaps");

      for (int i = 0; i < 5; i++) {
        display.setTextSize(i + i);
        display.setCursor(120, 80 + (i * 8 * i));
        display.setTextColor(6 - i, i + 1);
        display.print("Hello");
      }
      for (int i = 0; i < 20; i++) {
        esp_random();
        display.drawPixel(random(0, 100), random(80, 200), random(0, 10));
        display.drawLine(random(300, 450), random(80, 200), random(300, 450), random(80, 200), random(1, 7));
        display.drawRect(random(460, 550), random(80, 200), random(10, 50), random(10, 100), random(1, 7));
        display.drawCircle(random(650, 750), random(80, 200), random(3, 25), random(1, 7));
      }

      for (int i = 0; i < 7; i++) {
        esp_random();
        display.drawTriangle(random(0, 100), random(330, 470), random(0, 100), random(330, 470), random(0, 100), random(330, 470), random(1, 7));
        display.fillRect(random(150, 250), random(330, 450), random(10, 50), random(10, 100), random(1, 7));
        display.fillCircle(random(330, 480), random(330, 550), random(3, 25), random(1, 7));
        display.fillTriangle(random(500, 620), random(330, 600), random(500, 620), random(330, 600), random(500, 620), random(330, 600), random(1, 7));
        display.drawBitmap((int)random(620, 750), (int)random(330, 500), smile, 50, 50, random(1, 7));
      }
      display.setCursor(0, 580);
      display.setTextSize(2);
      display.setTextColor(0, 7);
      display.print("You can even display grayscale pictures!!!");
      break;

    case 2:
      display.selectDisplayMode(INKPLATE_3BIT);
      display.setTextSize(2);
      display.setCursor(5, 5);
      display.setTextColor(0, 15);
      display.print("Display simple 3 bit color images with just one line of code\nanywhere on epaper display");
      display.drawBitmap3Bit(random(20, 150), random(20, 150), img1, 207, 204);
      display.drawBitmap3Bit(random(300, 500), random(250, 350), img2, 148, 158);
      display.setCursor(0, 580);
      display.print("Even a full size image like this one...");
      break;

    case 3:
      display.selectDisplayMode(INKPLATE_3BIT);
      display.drawBitmap3Bit(0, 0, pic1, 800, 600);
      break;

    case 4:
      display.selectDisplayMode(INKPLATE_3BIT);
      display.drawBitmap3Bit(0, 0, pic2, 800, 600);
      break;
  }
  
  //IMPORTANT!!! Always clean screen before writing new picture to it!
  //There is also similar function called clearDisplay() but this only clear buffer.
  //Since we are using deep sleep of ESP32, ESP32 resets after every wake up, so buffer is clean and there is not any data to clean, so there is no reason to use clearDisplay().
  display.clean();
  display.display();
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  slide++;
  if (slide > 4) slide = 0;
  //This is to reduce current consumption of ESP32 in deep sleep mode. Otherwise, you don't need to use this.
  rtc_gpio_isolate(GPIO_NUM_12);
  esp_deep_sleep_start();
}

void loop() {
  //Nothing

}
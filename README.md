# Inkplate 6 Arduino library

![](https://www.crowdsupply.com/img/040a/inkplate-6-angle-01_png_project-main.jpg)

Arduino library for all-in-one e-paper display Inkplate 6 can be found in this repo. Inkplate 6 is a powerful, Wi-Fi enabled ESP32 based six-inch e-paper display – recycled from a Kindle e-reader. Its main feature is simplicity. Just plug in a USB cable, open Arduino IDE, and change the contents of the screen with few lines of code. Learn more about Inkplate 6 on [Crowd Supply](https://www.crowdsupply.com/e-radionica/inkplate-6).

### Setting up Inkplate 6 in Arduino IDE

In order to get a head start with Inkplate 6, follow these steps:

1. [Install Inkplate 6 board definition](https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/) - add Inkplate 6 as a board into your Arduino IDE. Follow the instructions on the [link](https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/).
2. Install CH340 drivers (if you don't have them yet) - instructions [here](https://e-radionica.com/en/blog/ch340-driver-installation-croduino-basic3-nova2/)
3. Install Inkplate 6 Arduino library - install the library from this repo into your Arduino IDE. If you don't know how, check our [tutorial](https://e-radionica.com/en/blog/arduino-library/#Kako%20instaliraty%20library?).
4. Install [custom SdFat library](https://github.com/e-radionicacom/Inkplate-6-SDFat-Arduino-Library) into your Arduino IDE.
5. You are ready to get started! Select Tools -> Board -> Inkplate 6, as well as correct COM port and upload!

### Code examples

There are many examples in the library that you demonstrate how to use any of the Inkplate functionality.

1. **Basic Inkplate Functionality**

   1.1. Basic BW - use the screen in BW mode (graphics & text)

   1.2. Basic Gray - use the screen in gray mode (graphics & text)

   1.3. Basic Partial Update - do a partial update of the screen on Inkplate 6

   1.4. Basic Custom Fonts - add custom fonts and use them to write text on the screen

2. **Advanced Inkplate Functionality**

   2.1. WiFi HTTP Request - make HTTP request while connected too WiFi

   2.2. Low Power - put Inkplate in sleep in order to save battery

   2.3. Battery Voltage And Temperature - measure battery voltage and temperature of the board

   2.4. Touchpads - get readings from onboard touch pads

   2.5. SD BMP Pictures - show .bmp images from the SD card on the screen

   2.6. Inkplate SD TXT File - read .txt file from the SD card and show it on the screen

   2.7. easyC - use easyC ecosystem (I2C) and read a sensor value

   2.8. MCP23017 Expander - use onboard MCP23017 expander for your own external devices

   2.9. Web Server - make Inkplate a web server

3. **Others**

   3.1. Slave Mode - mode that enables control of the screen from any external device using UART

   3.2. Screen Cleaning - clean the screen in case of image burn-in

### Using Inkplate 6 with another microcontroller

As promised in an [early update](https://www.crowdsupply.com/e-radionica/inkplate-6/updates/successfully-funded-also-third-party-master-controllers-and-partial-updates), Inkplate 6's screen contents can be updated using 3rd controller (such as Raspberry Pi or another microcontroller). The "Slave Mode" (unpopular name right now, we are aware and will change) enables this. All brand new Inkplates come pre-programmed with slave mode and can be used right away.

It is based on UART (serial) communication - connect the Inkplate to "master" board either via USB cable or directly via ESP32 RX and TX pins. Using standard UART at 115200 baud, you can send commands to change screen contents. For example, send \*#H(000,000,"/img.bmp")\*\* to show image img.bmp from SD card on the screen. Find very basic documentation for using it [here](https://github.com/e-radionicacom/Inkplate-6-Arduino-library/blob/master/examples/4.%20Others/1-Inkplate_Slave_Mode/Inkplate_slave_mode_documentation.txt).

### Battery power

Inkplate 6 has two options for powering it. First one is obvious - USB port at side of the board. Just plug any micro USB cable and you are good to go. Second option is battery. Supported batteries are standard Li-Ion/Li-Poly batteries with 3.7V nominal voltage. Connector for the battery is standard 2.00mm pitch JST connector. The onboard charger will charge the battery with 500mA when USB is plugged at the same time. You can use battery of any size or capacity if you don't have a enclosure. If you are using our enclosure, battery size shouldn't exceed 90mm x 40mm (3.5 x 1.57 inch) and 5mm (0.19 inch) in height. [This battery](https://e-radionica.com/en/li-ion-baterija-1200mah.html) is good fit for the Inkplate.

### Micropython

If you are looking for micropython support, it is still work in progress - we are new with it! :) When it's ready, we will let you know with new project update.

### License

This repo uses the source code from Adafruit libraries: [MCP23017](https://github.com/adafruit/Adafruit-MCP23017-Arduino-Library), [Adafruit GFX](https://github.com/adafruit/Adafruit-GFX-Library).

### Where to buy & other

Inkplate 6 is available for purchase via:

- [e-radionica.com](https://e-radionica.com/en/inkplate.html)
- [Crowd Supply](https://www.crowdsupply.com/e-radionica/inkplate-6)
- [Mouser](https://hr.mouser.com/Search/Refine?Keyword=inkplate)
- [Sparkfun](https://www.sparkfun.com/search/results?term=inkplate)

Inkplate 6 is open-source. If you are looking for hardware design of the board, check the [Hardware repo](https://github.com/e-radionicacom/Inkplate-6-hardware). You will find 3D printable [enclosure](https://github.com/e-radionicacom/Inkplate-6-hardware/tree/master/3D%20printable%20case) there, as well as [detailed dimensions](https://github.com/e-radionicacom/Inkplate-6-hardware/tree/master/Technical%20drawings). In this repo you will find code for driving the ED060SC7 e-paper display used by Inkplate.

For all questions and issues, please use our [forum](http://forum.e-radionica.com/en) to ask an question.
For sales & collaboration, please reach us via [e-mail](mailto:kontakt@e-radionica.com).

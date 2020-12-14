# Inkplate 6 Arduino library

![](https://www.crowdsupply.com/img/040a/inkplate-6-angle-01_png_project-main.jpg)

Arduino library for all-in-one e-paper display Inkplate 6 can be found in this repo. Inkplate 6 is a powerful, Wi-Fi enabled ESP32 based six-inch e-paper display – recycled from a Kindle e-reader. Its main feature is simplicity. Just plug in a USB cable, open Arduino IDE, and change the contents of the screen with few lines of code. Learn more about Inkplate 6 on [official website](https://inkplate.io/). Inkplate was crowdfunded on [Crowd Supply](https://www.crowdsupply.com/e-radionica/inkplate-6).

### Setting up Inkplate 6 in Arduino IDE

In order to get a head start with Inkplate 6, follow these steps:

1. [Install Inkplate 6 board definition](https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/) - add Inkplate 6 as a board into your Arduino IDE. Follow the instructions on the [link](https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/).
2. Install CH340 drivers (if you don't have them yet) - instructions [here](https://e-radionica.com/en/blog/ch340-driver-installation-croduino-basic3-nova2/)
3. Install Inkplate 6 Arduino library - install the library from this repo into your Arduino IDE. If you don't know how, check our [tutorial](https://e-radionica.com/en/blog/arduino-library/#Kako%20instaliraty%20library?).
4. You are ready to get started! Select Tools -> Board -> Inkplate 6, as well as correct COM port and upload!

### Code examples

There are many examples in the library that you demonstrate how to use any of the Inkplate functionality.
More are added daily, such as projects or community contributions, so here are just the basic ones.

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

   2.5. SD Pictures - show .bmp, .jpg and .png images from the SD card on the screen

   2.6. Web Pictures - show .bmp, .jpg and .png images from the web

   2.7. Inkplate SD TXT File - read .txt file from the SD card and show it on the screen

   2.8. easyC - use easyC ecosystem (I2C) and read a sensor value

   2.9. MCP23017 Expander - use onboard MCP23017 expander for your own external devices

   2.10. Web Server - make Inkplate a web server

3. **Others**

   3.1. Peripheral Mode - mode that enables control of the screen from any external device using UART

### Using Inkplate 6 with another microcontroller - Peripheral mode

As promised in an [early update](https://www.crowdsupply.com/e-radionica/inkplate-6/updates/successfully-funded-also-third-party-master-controllers-and-partial-updates), Inkplate 6's screen contents can be updated using 3rd controller (such as Raspberry Pi or another microcontroller). The Peripheral mode enables this. All brand new Inkplates come pre-programmed with slave mode and can be used right away.

It is based on UART (serial) communication - connect the Inkplate to "Controller" ("Master", if using old terminology) board either via USB cable or directly via ESP32 RX and TX pins. Using standard UART at 115200 baud, you can send commands to change screen contents. For example, send \*#H(000,000,"/img.bmp")\*\* to show image img.bmp from SD card on the screen. Find very documentation for using it [here](https://inkplate.readthedocs.io/en/latest/slave-mode.html).

### Battery power

Inkplate 6 has two options for powering it. First one is obvious - USB port at side of the board. Just plug any micro USB cable and you are good to go. Second option is battery. Supported batteries are standard Li-Ion/Li-Poly batteries with 3.7V nominal voltage. Connector for the battery is standard 2.00mm pitch JST connector. The onboard charger will charge the battery with 500mA when USB is plugged at the same time. You can use battery of any size or capacity if you don't have a enclosure. If you are using our enclosure, battery size shouldn't exceed 90mm x 40mm (3.5 x 1.57 inch) and 5mm (0.19 inch) in height. [This battery](https://e-radionica.com/en/li-ion-baterija-1200mah.html) is good fit for the Inkplate.

### Linux?

Running Linux? You'll need to run few other lines from your Terminal:

```
apt install python3-pip
pip3 install pyserial
apt install python-is-python3
```

### Micropython

If you are looking for micropython support, please find all details [here](https://github.com/e-radionicacom/Inkplate-6-micropython).

### License

This repo uses the source code from another repositories. All their license files are located in "licences" folder.

### Open-source

All of Inkplate-related development is open-sourced:

- [Arduino library](https://github.com/e-radionicacom/Inkplate-6-Arduino-library)
- [Inkplate 6 hardware](https://github.com/e-radionicacom/Inkplate-6-hardware)
- [micropython Inkplate](https://github.com/e-radionicacom/Inkplate-6-micropython)
- [OSHWA certificate](https://certification.oshwa.org/hr000003.html)

### Where to buy & other

Inkplate 6 is available for purchase via:

- [e-radionica.com](https://e-radionica.com/en/inkplate.html)
- [Crowd Supply](https://www.crowdsupply.com/e-radionica/inkplate-6)
- [Mouser](https://hr.mouser.com/Search/Refine?Keyword=inkplate)
- [Sparkfun](https://www.sparkfun.com/search/results?term=inkplate)
- [Pimoroni](https://shop.pimoroni.com/products/inkplate-6)

For all questions and issues, please use our [forum](http://forum.e-radionica.com/en) to ask an question.
For sales & collaboration, please reach us via [e-mail](mailto:kontakt@e-radionica.com).

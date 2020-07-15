
# Inkplate 6 Arduino library

![](https://www.crowdsupply.com/img/040a/inkplate-6-angle-01_png_project-main.jpg)

Arduino library for all-in-one e-paper display Inkplate 6 can be found in this repo. Inkplate 6 is a powerful, Wi-Fi enabled ESP32 based six-inch e-paper display – recycled from a Kindle e-reader. Its main feature is simplicity. Just plug in a USB cable, open Arduino IDE, and change the contents of the screen with few lines of code. Learn more about Inkplate 6 on [Crowd Supply](https://www.crowdsupply.com/e-radionica/inkplate-6). 

#### Setting up Inkplate 6 in Arduino IDE
In order to get a head start with Inkplate 6, follow these steps:
1. [Install Inkplate 6 board definition](https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/) - add Inkplate 6 as a board into your Arduino IDE. Follow the instructions on the [link](https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/). 
2. Install CH340 drivers (if you don't have them yet) - instructions [here](https://e-radionica.com/en/blog/ch340-driver-installation-croduino-basic3-nova2/)
3. Install Inkplate 6 Arduino library - install the library from this repo into your Arduino IDE. If you don't know how, check our [tutorial](https://e-radionica.com/en/blog/arduino-library/#Kako%20instaliraty%20library?).
4. You are ready to get started! Select Tools -> Board -> Inkplate 6, as well as correct COM port and upload! 

Optional: if you are planning to use the SD card funcionality,  install [custom SdFat library](https://github.com/e-radionicacom/Inkplate-6-SDFat-Arduino-Library) into your Arduino IDE. 

#### Code examples
There are many examples in the library that you demonstrate how to use any of the Inkplate functionality. 

1. **Basic Inkplate Functionality**

  1.1. Basic Monochrome - use the screen in monochrome mode (graphics & text)
  
  1.2. Basic Grayscale - use the screen in grayscale mode (graphics & text)
  
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

#### Misc
Inkplate 6 is available for purchase via:
- [Crowd Supply](https://www.crowdsupply.com/e-radionica/inkplate-6)
- [Mouser](https://hr.mouser.com/Search/Refine?Keyword=inkplate)

Inkplate 6 is open-source. If you are looking for hardware design of the board, check the [Hardware repo](https://github.com/e-radionicacom/Inkplate-6-hardware). You will find 3D printable [enclosure](https://github.com/e-radionicacom/Inkplate-6-hardware/tree/master/3D%20printable%20case) there, as well as [detailed dimensions](https://github.com/e-radionicacom/Inkplate-6-hardware/tree/master/Technical%20drawings). In this repo you will find code for driving the ED060SC7 e-paper display used by Inkplate.

For all questions and issues, please open an issue or thread on [our forums](http://forum.e-radionica.com/en/).
For sales & collaboration, please reach us via [e-mail](mailto:kontakt@e-radionica.com).

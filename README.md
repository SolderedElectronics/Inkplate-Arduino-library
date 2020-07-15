
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
#### Misc
Inkplate 6 is available for purchase via:
- [Crowd Supply](https://www.crowdsupply.com/e-radionica/inkplate-6)
- [Mouser](https://hr.mouser.com/Search/Refine?Keyword=inkplate)

Inkplate 6 is open-source. If you are looking for hardware design of the board, check the [Hardware repo](https://github.com/e-radionicacom/Inkplate-6-hardware). You will find 3D printable [enclosure](https://github.com/e-radionicacom/Inkplate-6-hardware/tree/master/3D%20printable%20case) there, as well as [detailed dimensions](https://github.com/e-radionicacom/Inkplate-6-hardware/tree/master/Technical%20drawings). In this repo you will find code for driving the ED060SC7 e-paper display used by Inkplate.

For all questions and issues, please open an issue or thread on [our forums](http://forum.e-radionica.com/en/).
For sales & collaboration, please reach us via [e-mail](mailto:kontakt@e-radionica.com).

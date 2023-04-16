# Soldered Inkplate Arduino library

[![Compile Sketches](https://github.com/e-radionicacom/Inkplate-Arduino-library/actions/workflows/compile.yml/badge.svg?branch=master)](https://github.com/e-radionicacom/Inkplate-Arduino-library/actions/workflows/compile.yml)

<p align="center">
  <img src="https://raw.githubusercontent.com/SolderedElectronics/Inkplate-Arduino-library/dev/extras/InkplateImage.jpg">
</p>

Arduino library for all-in-one e-paper display family named Inkplate can be found in this repo. Inkplate is a series of powerful, Wi-Fi and Bluetooth enabled, ESP32 based e-paper display products. Its main feature is simplicity. Just plug in a USB cable, open Arduino IDE, and change the contents of the screen with few lines of code. Inkplate family currently includes Inkplate 10, Inkplate 6 and Inkplate 6PLUS, Inkplate 6COLOR and Inkplate 2. 
Inkplate 6 was crowdfunded on [Crowd Supply](https://www.crowdsupply.com/e-radionica/inkplate-6), as well as [Inkplate 10](https://www.crowdsupply.com/e-radionica/inkplate-10), [Inkplate 6PLUS](https://www.crowdsupply.com/e-radionica/inkplate-6plus) and [Inkplate 6COLOR](https://www.crowdsupply.com/soldered/inkplate-6color). Inkplate 2 was funded on [Kickstarter](https://www.kickstarter.com/projects/solderedelectronics/inkplate-2-a-easy-to-use-arduino-compatible-e-paper). All available to purchase from [soldered.com](https://soldered.com/categories/inkplate/). 

### Setting up Inkplate in Arduino IDE

In order to get a head start with Inkplate (any), follow these steps:

1. [Install Inkplate board definition](https://github.com/SolderedElectronics/Dasduino-Board-Definitions-for-Arduino-IDE/blob/master/README.md) - add Inkplate 6/10/6PLUS/6COLOR/2 as a board into your Arduino IDE. Follow the instructions on the [link](https://soldered.com/learn/add-inkplate-6-board-definition-to-arduino-ide/).
2. Install CH340 drivers (if you don't have them yet) - instructions [here](https://soldered.com/learn/ch340-driver-installation-croduino-basic3-nova2/)
3. Install Inkplate Arduino library - Install the library from the Arduino Library Manager (just type "Inkplate" and click "Install") or install it with this repo. If you don't know how, check our [tutorial](https://soldered.com/learn/arduino-library/#Kako%20instaliraty%20library?).
4. You are ready to get started! Select Tools -> Board -> Inkplate Boards and then pick correct one, as well as correct COM port and upload!

### Code examples

There are many examples in the library that you demonstrate how to use any of the Inkplate functionality. Use Basic folder to get familiar with principles of using the Inkplate, such as modes (BW and Gray), how to write graphics and text or just basic capabilites such as touchscreen or front lighting (if your Inkplate supports it). In Advanced folder, you'll learn how to make HTTP/HTTPS requests, utilise low power, use touchpads or SD card, read RTC, etc. Finally, get easily started on some projects. Some of the projects are Google calendar, Weather Station, Image Frame, Crypto Currency Tracker and more. Examples and projects are added regularly by us or from community contributions. There is also a diagnostics folder with all tools for more advanced users (such as VCOM programming, waveform selection, etc).

### Documentation

Find Inkplate documentation [here](https://inkplate.readthedocs.io/en/latest/arduino.html). 


### Using Inkplate with another microcontroller - Peripheral mode

Inkplate screen contents can be updated using 3rd controller (such as Raspberry Pi or another microcontroller). The Peripheral mode enables this. All brand new Inkplates come pre-programmed with slave mode and can be used right away.

It is based on UART (serial) communication - connect the Inkplate to "Controller" ("Master", if using old terminology) board either via USB cable or directly via ESP32 RX and TX pins. Using standard UART at 115200 baud, you can send commands to change screen contents. For example, send \*#H(000,000,"/img.bmp")\*\* to show image img.bmp from SD card on the screen. Find very documentation for using it [here](https://inkplate.readthedocs.io/en/latest/peripheral-mode.html).

### Battery power

Inkplate boards has two options for powering it. First one is obvious - USB port at side of the board. Just plug any micro USB cable and you are good to go. Second option is battery. Supported batteries are standard Li-Ion/Li-Poly batteries with 3.7V nominal voltage. Connector for the battery is standard 2.00mm pitch JST connector (except on Inkplate 2, it uses SMD solder pads for battery terminals). The onboard charger will charge the battery with 500mA when USB is plugged at the same time. You can use battery of any size or capacity if you don't have a enclosure. If you are using our enclosure, battery size shouldn't exceed 90mm x 40mm (3.5 x 1.57 inch) and 5mm (0.19 inch) in height (excluding Inkplate 2, it uses [this battery](https://soldered.com/product/li-ion-baterija-600mah-3-7v/). [This battery](https://soldered.com/product/li-ion-battery-1200mah-3-7v/) is good fit for the Inkplate. Also, Inkplate's hardware is specially optimized for low power consumption in deep sleep mode, making it extremely suitable for battery applications.

#### ⚠️ WARNING
Please check the polarity on the battery JST connector! Some batteries that can be purchased from the web have reversed polarity that can damage Inkplate board! You are safe if you are using the pouch battery from [soldered.com](https://soldered.com/categories/power-sources-batteries/batteries/lithium-batteries/) or Inkplate with the built-in battery . 

#### ℹ NOTE
CR2032 battery is only for RTC backup. Inkplate cannot be powered with it.

### Linux?

Running Linux? You'll need to run few other lines from your Terminal:

```
apt install python3-pip
pip3 install pyserial
apt install python-is-python3
```

### Micropython

If you are looking for micropython support, please find all details [here](https://github.com/SolderedElectronics/Inkplate-micropython).

### License

This repo uses the source code from another repositories. All their license files are located in "licences" folder.

### Open-source

All of Inkplate-related development is open-sourced:

- [Arduino library](https://github.com/SolderedElectronics/Inkplate-Arduino-library)
- Hardware design:
  - Soldered Inkplate 2 (comming soon!)
  - Soldered Inkplate 6 (comming soon!)
  - Soldered Inkplate 6PLUS (comming soon!)
  - [Soldered Inkplate 10](https://github.com/SolderedElectronics/Soldered-Inkplate-10-hardware-design)
  - Soldered Inkplate 6COLOR (comming soon!)
  - [e-radionica.com Inkplate 6](https://github.com/SolderedElectronics/Inkplate-6-hardware)
  - [e-radionica.com Inkplate 10](https://github.com/SolderedElectronics/Inkplate-10-hardware)
  - [e-radionica.com Inkplate 6PLUS](https://github.com/SolderedElectronics/Inkplate-6PLUS-Hardware)
  - e-radionica.com Inkplate 6COLOR (comming soon!)
- [micropython Inkplate](https://github.com/SolderedElectronics/Inkplate-micropython)
- [OSHWA cerfiticates](https://certification.oshwa.org/list.html?q=inkplate)

### Where to buy & other

Inkplate boards are available for purchase via:

- [soldered.com](https://soldered.com/categories/inkplate/)
- [Crowd Supply](https://www.crowdsupply.com/soldered)
- [Mouser](https://hr.mouser.com/Search/Refine?Keyword=inkplate)

For all questions and issues please reach us via [e-mail](mailto:hello@soldered.com) or our [contact form](https://soldered.com/contact/).

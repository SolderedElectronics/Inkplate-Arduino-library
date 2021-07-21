# Inkplate Arduino library

![](https://www.crowdsupply.com/img/cf95/compare-peripheral_jpg_project-body.jpg)

Arduino library for all-in-one e-paper display family named Inkplate can be found in this repo. Inkplate is a series of powerful, Wi-Fi enabled ESP32-based e-paper display products. Its main feature is simplicity. Just plug in a USB cable, open Arduino IDE, and change the contents of the screen with few lines of code. Inkplate family currently includes Inkplate 10, Inkplate 6 and Inkplate 6PLUS - learn more about Inkplates on [official website](https://inkplate.io/). 
Inkplate 6 was crowdfunded on [Crowd Supply](https://www.crowdsupply.com/e-radionica/inkplate-6), as well as [Inkplate 10](https://www.crowdsupply.com/e-radionica/inkplate-10) and [Inkplate 6PLUS](https://www.crowdsupply.com/e-radionica/inkplate-6plus).

### Setting up Inkplate in Arduino IDE

In order to get a head start with Inkplate (any), follow these steps:

1. [Install Inkplate board definition](https://github.com/e-radionicacom/Croduino-Board-Definitions-for-Arduino-IDE/blob/master/README.md) - add Inkplate 6/10/6PLUS as a board into your Arduino IDE. Follow the instructions on the [link](https://e-radionica.com/en/blog/add-inkplate-6-to-arduino-ide/).
2. Install CH340 drivers (if you don't have them yet) - instructions [here](https://e-radionica.com/en/blog/ch340-driver-installation-croduino-basic3-nova2/)
3. Install Inkplate Arduino library - install the library from this repo into your Arduino IDE. If you don't know how, check our [tutorial](https://e-radionica.com/en/blog/arduino-library/#Kako%20instaliraty%20library?).
4. You are ready to get started! Select Tools -> Board -> Inkplate (pick correct one), as well as correct COM port and upload!

### Code examples

There are many examples in the library that you demonstrate how to use any of the Inkplate functionality. Use Basic folder to get familiar with principles of using the Inkplate, such as modes (BW and Gray), how to write graphics and text or just basic capabilites such as touchscreen or front lighting (if your Inkplate supports it). In Advanced folder, you'll learn how to make HTTP/HTTPS requests, utilise low power, use touchpads or SD card. Finally, get easily started on some projects. More are added regularly, such as projects or community contributions.

### Documentation

Find Inkplate documentation [here](https://inkplate.readthedocs.io/en/latest/arduino.html). 


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

If you are looking for micropython support, please find all details [here](https://github.com/e-radionicacom/Inkplate-micropython).

### License

This repo uses the source code from another repositories. All their license files are located in "licences" folder.

### Open-source

All of Inkplate-related development is open-sourced:

- [Arduino library](https://github.com/e-radionicacom/Inkplate-6-Arduino-library)
- Hardware design: [Inkplate 6](https://github.com/e-radionicacom/Inkplate-6-hardware), [Inkplate 10](https://github.com/e-radionicacom/Inkplate-10-hardware)
- [micropython Inkplate](https://github.com/e-radionicacom/Inkplate-6-micropython)
- OSHWA cerfiticate: [Inkplate 6](https://certification.oshwa.org/hr000003.html), [Inkplate 10](https://certification.oshwa.org/hr000006.html)

### Where to buy & other

Inkplate 6 is available for purchase via:

- [e-radionica.com](https://e-radionica.com/en/inkplate.html)
- [Crowd Supply](https://www.crowdsupply.com/e-radionica/inkplate-6)
- [Mouser](https://hr.mouser.com/Search/Refine?Keyword=inkplate)
- [Pimoroni](https://shop.pimoroni.com/products/inkplate-6)

For all questions and issues, please use our [forum](http://forum.e-radionica.com/en) to ask an question.
For sales & collaboration, please reach us via [e-mail](mailto:kontakt@e-radionica.com).

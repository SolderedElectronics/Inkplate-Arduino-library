# BME280 and BME680 EasyC Arduino library

[![Make docs and publish to GitHub Pages](https://github.com/SolderedElectronics/Soldered-BME280-BME680-Gas-Sensor-Arduino-Library/actions/workflows/make_docs.yml/badge.svg?branch=dev)](https://github.com/SolderedElectronics/Soldered-BME280-BME680-Gas-Sensor-Arduino-Library/actions/workflows/make_docs.yml)
[![Arduino Library Manager Compatibility](https://github.com/SolderedElectronics/Soldered-BME280-BME680-Gas-Sensor-Arduino-Library/actions/workflows/arduino_lint.yml/badge.svg?branch=dev)](https://github.com/SolderedElectronics/Soldered-BME280-BME680-Gas-Sensor-Arduino-Library/actions/workflows/arduino_lint.yml)


| ![Enviromental & air quality sensor BME680 breakout](https://upload.wikimedia.org/wikipedia/commons/8/8f/Example_image.svg) |
| :-------------------------------------------------------------------------------------------------------------------------: |
|               [Enviromental & air quality sensor BME680 breakout](https://www.solde.red/333035)                             |

| ![Enviromental sensor BME280 breakout](https://upload.wikimedia.org/wikipedia/commons/8/8f/Example_image.svg) |
| :-----------------------------------------------------------------------------------------------------------: |
|                      [Enviromental sensor BME280 breakout](https://www.solde.red/333036)                      |

The BME680 is an atmospheric sensor that measures four values: temperature, pressure, humidity and air quality as an IAQ index. Using that data is can calculate elevation. It is really simple to use considering that it's using I2C communication protocol and it's dimensions are really small so it can fit anywhere. Everything you need to know about the atmospheric conditions and the quality of the air - you can find out using this sensor (ideal for wather station projects!) This board comes with two easyC connectors and provides easy connections.

- Voltage: 1.71V - 3.6V
- Pressure measurement range: 30kPa - 110kPa
- Pressure measurement error: +-12Pa
- Temperature measurement range: -40C - 85C
- Temperature measurement error: +-1C
- Humidity measurement range: 0% - 100%
- Humidity measurement error: +-3%
- IAQ index measurement range: 0 - 500
- Measuring consumption: max. 849uA
- Consumption in idle state: 0.15uA
- Dimensions: 22 x 22 mm (https://www.soldered.com/en/easyC)

BME280 is an atmospheric sensor which measures 3 values: temperature, pressure, and humidity. Besides, it is possible to calculate elevation. It is simple to use since it communicates via I2C and it is extra small sized so it can be put anywhere. Everything you need to know about the atmospheric condition - this sensor can measure for you; just ideal for meteo station projects! This board comes with two easyC connectors and provides easy connections.

- Voltage: 3.3V
- Pressure range: 30kPa - 110kPa
- Pressure error: +-12Pa
- Temperature range: -40C - 80C
- Temperature error: +-1C
- Humidity range: 0% - 100%
- Humidity error: +-2%
- Current while measuring: 1mA
- Current while idle: <5uA
- Communication: I2C
- Dimensions: 22 x 22 mm

### Repository Contents

- **/src** - source files for the library (.h & .cpp)
- **/examples** - examples for using the library
- **_other_** - _keywords_ file highlights function words in your IDE, _library.properties_ enables implementation with Arduino Library Manager.

### Hardware design

You can find hardware design for this board in [_Enviromental & air quality sensor BME680 breakout_](https://github.com/SolderedElectronics/NAZIVPROIZVODA-hardware-design) hardware repository.

You can find hardware design for this board in [_Enviromental sensor BME280 breakout_](https://github.com/SolderedElectronics/NAZIVPROIZVODA-hardware-design) hardware repository.

### Documentation

Access Arduino library documentation [here](https://SolderedElectronics.github.io/Soldered-BME280-BME680-Gas-Sensor-Arduino-Library/).

- Tutorial for using the BME280 and BME680 board
- Installing an Arduino library

### Board compatibility

The library is compatible with board & microcontroller families shown in green below:

[![Compile Sketches](http://github-actions.40ants.com/e-radionicacom/Soldered-BME280-BME680-Gas-Sensor-Arduino-Library/matrix.svg?branch=dev&only=Compile%20Sketches)](https://github.com/SolderedElectronics/Soldered-BME280-BME680-Gas-Sensor-Arduino-Library/actions/workflows/compile_test.yml)

### About Soldered

<img src="https://raw.githubusercontent.com/e-radionicacom/Soldered-Generic-Arduino-Library/dev/extras/Soldered-logo-color.png" alt="soldered-logo" width="500"/>

At Soldered, we design and manufacture a wide selection of electronic products to help you turn your ideas into acts and bring you one step closer to your final project. Our products are intented for makers and crafted in-house by our experienced team in Osijek, Croatia. We believe that sharing is a crucial element for improvement and innovation, and we work hard to stay connected with all our makers regardless of their skill or experience level. Therefore, all our products are open-source. Finally, we always have your back. If you face any problem concerning either your shopping experience or your electronics project, our team will help you deal with it, offering efficient customer service and cost-free technical support anytime. Some of those might be useful for you:

- [Web Store](https://www.soldered.com/shop)
- [Tutorials & Projects](https://soldered.com/learn)
- [Community & Technical support](https://soldered.com/community)

### Original source
​
This library is possible thanks to original [BME680 library](https://github.com/Zanduino/BME680) library. Thank you, Zanshin Arduino.

### Open-source license

Soldered invests vast amounts of time into hardware & software for these products, which are all open-source. Please support future development by buying one of our products.

Check license details in the LICENSE file. Long story short, use these open-source files for any purpose you want to, as long as you apply the same open-source licence to it and disclose the original source. No warranty - all designs in this repository are distributed in the hope that they will be useful, but without any warranty. They are provided "AS IS", therefore without warranty of any kind, either expressed or implied. The entire quality and performance of what you do with the contents of this repository are your responsibility. In no event, Soldered (TAVU) will be liable for your damages, losses, including any general, special, incidental or consequential damage arising out of the use or inability to use the contents of this repository.

## Have fun!

And thank you from your fellow makers at Soldered Electronics.

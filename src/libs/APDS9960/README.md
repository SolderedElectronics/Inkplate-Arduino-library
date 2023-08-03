# Color and gesture sensor APDS-9960 breakout Arduino library

[![Make docs and publish to GitHub Pages](https://github.com/SolderedElectronics/Soldered-APDS9960-Light-Gesture-Color-Sensor-Arduino-Library/actions/workflows/make_docs.yml/badge.svg?branch=dev)](https://github.com/SolderedElectronics/Soldered-APDS9960-Light-Gesture-Color-Sensor-Arduino-Library/actions/workflows/make_docs.yml)
[![Arduino Library Manager Compatibility](https://github.com/SolderedElectronics/Soldered-Generic-Arduino-Library/actions/workflows/arduino_lint.yml/badge.svg?branch=dev)](https://github.com/SolderedElectronics/Soldered-Generic-Arduino-Library/actions/workflows/arduino_lint.yml)


| ![Color and gesture sensor APDS-9960 breakout](https://upload.wikimedia.org/wikipedia/commons/8/8f/Example_image.svg) |
| :-------------------------------------------------------------------------------------------------------------------: |
|                      [Color and gesture sensor APDS-9960 breakout](https://www.solde.red/333002)                      |

The APDS-9960 is a small sensor that measures four things at once. It is possible to recognize the movements you make in front of it (up, down, left, right) and it can identify the vicinity of the object, so you have two more movements: to the sensor and from the sensor. In addition, it has an ambient light sensor and a color sensor which gives red, green, and blue (RGB) colors as output. This sensor with many features was even built into the Samsung Galaxy S5 smartphone.

All gestures are detected at a distance of 10 to 20cm  and it has built-in UV and IR filters to make sure it won't be confused by other sources of light. Connected using well-known I2C communication.

- Voltage: 3.3V
- Voltage I2C: 3.3V
- Connectors: 2x easyC
- Dimensions: 22 x 36 mm
  (https://www.soldered.com/en/easyC)

### Repository Contents

- **/src** - source files for the library (.h & .cpp)
- **/examples** - examples for using the library
- **_other_** - _keywords_ file highlights function words in your IDE, _library.properties_ enables implementation with Arduino Library Manager.

### Hardware design

You can find hardware design for this board in [_Color and gesture sensor APDS-9960 breakout_](https://github.com/SolderedElectronics/Color---gesture-sensor-APDS-9960-breakout-hardware-design) hardware repository.

### Documentation

Access Arduino library documentation [here](https://SolderedElectronics.github.io/Soldered-APDS9960-Light-Gesture-Color-Sensor-Arduino-Library/).

- Tutorial for using the Color and gesture sensor board
- Installing an Arduino library

### Board compatibility

The library is compatible with board & microcontroller families shown in green below:

[![Compile Sketches](http://github-actions.40ants.com/e-radionicacom/Soldered-APDS9960-Light-Gesture-Color-Sensor-Arduino-Library/matrix.svg?branch=dev&only=Compile%20Sketches)](https://github.com/SolderedElectronics/Soldered-APDS9960-Light-Gesture-Color-Sensor-Arduino-Library/actions/workflows/compile_test.yml)

### About Soldered

<img src="https://raw.githubusercontent.com/e-radionicacom/Soldered-Generic-Arduino-Library/dev/extras/Soldered-logo-color.png" alt="soldered-logo" width="500"/>

At Soldered, we design and manufacture a wide selection of electronic products to help you turn your ideas into acts and bring you one step closer to your final project. Our products are intented for makers and crafted in-house by our experienced team in Osijek, Croatia. We believe that sharing is a crucial element for improvement and innovation, and we work hard to stay connected with all our makers regardless of their skill or experience level. Therefore, all our products are open-source. Finally, we always have your back. If you face any problem concerning either your shopping experience or your electronics project, our team will help you deal with it, offering efficient customer service and cost-free technical support anytime. Some of those might be useful for you:

- [Web Store](https://www.soldered.com/shop)
- [Tutorials & Projects](https://soldered.com/learn)
- [Community & Technical support](https://soldered.com/community)

### Original source
​
This library is possible thanks to original [Arduino_APDS9960](https://github.com/arduino-libraries/Arduino_APDS9960) library. Thank you, Arduino.

### Open-source license

Soldered invests vast amounts of time into hardware & software for these products, which are all open-source. Please support future development by buying one of our products.

Check license details in the LICENSE file. Long story short, use these open-source files for any purpose you want to, as long as you apply the same open-source licence to it and disclose the original source. No warranty - all designs in this repository are distributed in the hope that they will be useful, but without any warranty. They are provided "AS IS", therefore without warranty of any kind, either expressed or implied. The entire quality and performance of what you do with the contents of this repository are your responsibility. In no event, Soldered (TAVU) will be liable for your damages, losses, including any general, special, incidental or consequential damage arising out of the use or inability to use the contents of this repository.

## Have fun!

And thank you from your fellow makers at Soldered Electronics.

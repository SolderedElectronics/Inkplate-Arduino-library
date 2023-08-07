SparkFun BQ27441-G1A LiPo Fuel Gauge Arduino Library
========================================

[![SparkFun Battery Babysitter](https://cdn.sparkfun.com/assets/parts/1/1/3/3/1/13777-01.jpg)](https://www.sparkfun.com/products/13777)

[*SparkFun Battery Babysitter (featuring the BQ27441-G1A LiPo Fuel Gauge) (PRT-13777)*](https://www.sparkfun.com/products/13777)

Texas Instrument's [BQ27441-G1A](http://www.ti.com/product/BQ27441-G1) is a self-calibrating, I2C-based fuel gauge for lithium polymer (LiPo) batteries -- it measures your battery's voltage to estimate its charge percentage and remaining capacity. The chip is also hooked up to a current-sensing resistor, which allows it to measure current and power! The fuel gauge makes up half of the [SparkFun Battery Babysitter](https://www.sparkfun.com/products/13777).

This Arduino library abstracts away all of the low-level I2C communication, so you can easily initialize the fuel gauge then read voltage, state-of-charge, current, power, and capacity. It also implements all of the chip's low-battery, and SoC-change alerts on the GPOUT pin.

Thanks to:

* [keton](https://github.com/keton) for adding extended config methods and an example showing how to use them

Repository Contents
-------------------

* **/examples** - Example sketches for the library (.ino). Run these from the Arduino IDE. 
* **/src** - Source files for the library (.cpp, .h).
* **keywords.txt** - Keywords from this library that will be highlighted in the Arduino IDE. 
* **library.properties** - General library properties for the Arduino package manager. 

Documentation
--------------

* **[Installing an Arduino Library Guide](https://learn.sparkfun.com/tutorials/installing-an-arduino-library)** - Basic information on how to install an Arduino library.
* **[Product Repository](https://github.com/sparkfun/Battery_Babysitter)** - Main repository (including hardware files) for the SparkFun Battery Babysitter (which features the BQ27441-G1A ).
* **[Hookup Guide](https://learn.sparkfun.com/tutorials/battery-babysitter-hookup-guide)** - Basic hookup guide for the SparkFun Battery Babysitter.

Products that use this Library 
---------------------------------

* [SparkFun Battery Babysitter - PRT-13777](https://www.sparkfun.com/products/13777)- The Battery Babysitter is an all-in-one battery-charger and monitor, featuring the BQ27441-G1A and BQ24075 charger.

Version History
---------------

* [1.0.0](https://github.com/sparkfun/SparkFun_BQ27441_Arduino_Library/releases/tag/V_1.0.0) - Initial release of the BQ27441 Arduino Library.

License Information
-------------------

This product is _**open source**_! 

Please review the LICENSE.md file for license information. 

If you have any questions or concerns on licensing, please contact techsupport@sparkfun.com.

Distributed as-is; no warranty is given.

- Your friends at SparkFun.

_<COLLABORATION CREDIT>_

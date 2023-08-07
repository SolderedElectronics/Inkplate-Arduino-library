SparkFun LSM6DS3 Arduino Library
========================================

![LSM6DS3 Breakout](https://cdn.sparkfun.com/assets/learn_tutorials/4/1/6/perspective.jpg)

[*LSM6DS3 Breakout (SEN-13339)*](https://www.sparkfun.com/products/13339)

This is an arduino IDE library to control the LSM6DS3.  It can be configured to use I2C or SPI with 2 instances per I2C channel or any number of SPI instances.  The top-level driver, class LSM6DS3, contains an inner driver LSM6DS3Core, a settings struct, and float-based math functions for conversion from raw to meaningful numbers.

This has been tested with Arduino Uno, ESP32, ESP8266, and Teensy 3.1 architectures.

Repository Contents
-------------------

* **/examples** - Example sketches for the library (.ino). Run these from the Arduino IDE. 
* **/extras** - Contains class diagrams for the driver.  Ignored by IDE.
* **/src** - Source files for the library (.cpp, .h).
* **keywords.txt** - Keywords from this library that will be highlighted in the Arduino IDE. 
* **library.properties** - General library properties for the Arduino package manager. 

Example Briefs
--------------

* FifoExample - Demonstrates using the built-in buffer to burst-collect data - **Good demonstration of settings**
* InterruptFreeFall - Embedded function demonstrating free-fall detection
* InterruptHWTapConfig - Embedded function demonstrating tap and double-tap detection
* LowLevelExample - Demonstrates using only the core driver without math and settings overhead
* MemoryPagingExample - Demonstrates switching between memory pages
* MinimalistExample - The **easiest** configuration
* MultiI2C - Using two LSM6DS3s over I2C
* MultiSPI - Using two LSM6DS3s over SPI
* Pedometer - Embedded function demonstrating step-counting feature

Documentation
--------------

* **[Installing an Arduino Library Guide](https://learn.sparkfun.com/tutorials/installing-an-arduino-library)** - Basic information on how to install an Arduino library.
* **[Product Repository](https://github.com/sparkfun/LSM6DS3_Breakout/)** - Main repository (including hardware files) for the LSM6DS3 Breakout.
* **[Hookup Guide](https://learn.sparkfun.com/tutorials/lsm6ds3-breakout-hookup-guide)** - Basic hookup guide for the LSM6DS3 Breakout.

Products that use this Library 
---------------------------------

* [SEN-13339](https://www.sparkfun.com/products/13339)- LSM6DS3 Breakout board

Version History
---------------

* [V 1.0.0](https://github.com/sparkfun/SparkFun_LSM6DS3_Arduino_Library/releases/tag/V_1.0.0) -- Initial commit of Arduino 1.6-compatible library.
* [V 1.0.1](https://github.com/sparkfun/SparkFun_LSM6DS3_Arduino_Library/releases/tag/V_1.0.1) -- Adds SPI compatibility to the ESP32 and ESP8266 boards for this product. 

License Information
-------------------

This product is _**open source**_! 

Please review the LICENSE.md file for license information. 

If you have any questions or concerns on licensing, please contact techsupport@sparkfun.com.

Distributed as-is; no warranty is given.

- Your friends at SparkFun.


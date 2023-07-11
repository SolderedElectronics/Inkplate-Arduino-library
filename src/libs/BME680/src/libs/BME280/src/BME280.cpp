/*!
 * @file BME280.cpp
 * @section BME280cpp_intro_section Description
 *
 * Arduino Library for the BME280 Bosch sensor\n\n
 * See main library header file for details
 */
#include "BME280.h"
BME280_Class::BME280_Class() {}   ///< Empty & unused class constructor
BME280_Class::~BME280_Class() {}  ///< Empty & unused class destructor
bool BME280_Class::begin() {
  /*!
   * @brief   Begin method to start I2C communications
   * @details It is overloaded to allow for 3 different connection types to be used - I2C, Hardware
   * SPI and Software SPI. When called with no parameters the I2C mode is enabled and the I2C bus is
   * scanned for the first BME280 (typically at 0x76 or 0x77 unless an I2C expander is used to remap
   * the address.
   * @return  returns "true" when the class initialized correctly
   */
  return begin(I2C_STANDARD_MODE);  // Initialize I2C with slow speed
}  // of method begin()
bool BME280_Class::begin(const uint32_t i2cSpeed) {
  /*!
   * @brief     Begin method to start I2C communications
   * @details   It is overloaded to allow for 3 different connection types to be used - I2C,
   * Hardware SPI and Software SPI. When called with no parameters the I2C mode is enabled and the
   * I2C bus is scanned for the first BME280 (typically at 0x76 or 0x77 unless an I2C expander is
   * used to remap the address.
   * @param[in] i2cSpeed I2C speed rate in baud
   * @return  returns "true" when the class initialized correctly
   */
  Wire.begin();                                            // Start I2C as master device
  Wire.setClock(i2cSpeed);                                 // Set I2C bus speed
  for (_I2CAddress = 0; _I2CAddress < 127; _I2CAddress++)  // loop all possible addresses
  {
    Wire.beginTransmission(_I2CAddress);  // Check current address for BME280
    if (Wire.endTransmission() == 0)      // If no error we have a device
    {
      if (readByte(BME280_CHIPID_REG) == BME280_CHIPID)  // check for correct chip id
      {
        getCalibration();  // get the calibration values
        return true;
      }             // of if-then device is really a BME280
    }               // of if-then we have found a device
  }                 // of for-next each I2C address loop
  _I2CAddress = 0;  // Set to 0 to denote no I2C found
  return false;
}  // of method begin()
bool BME280_Class::begin(const uint8_t chipSelect)  // Use hardware SPI for comms
{
  /*!
   * @brief     Begin method to start hardware SPI communications
   * @details   It is overloaded to allow for 3 different connection types to be used - I2C,
   * Hardware SPI and Software SPI. When called with no parameters the I2C mode is enabled and the
   * I2C bus is scanned for the first BME280 (typically at 0x76 or 0x77 unless an I2C expander is
   * used to remap the address.
   * @param[in] chipSelect Hardware SPI CS chip
   * @return  returns "true" when the class initialized correctly
   */
  _cs = chipSelect;                                  // Store value for future use
  digitalWrite(_cs, HIGH);                           // High means ignore master
  pinMode(_cs, OUTPUT);                              // Make the chip select pin output
  SPI.begin();                                       // Start hardware SPI
  if (readByte(BME280_CHIPID_REG) == BME280_CHIPID)  // check for correct chip id
  {
    getCalibration();  // get the calibration values
    return true;       // return success
  }                    // of if-then device is really a BME280
  return false;        // return failure if we get here
}  // of method begin()
bool BME280_Class::begin(const uint8_t chipSelect, const uint8_t mosi, const uint8_t miso,
                         const uint8_t sck) {
  /*!
   * @brief     Begin method to start software SPI communications
   * @details   It is overloaded to allow for 3 different connection types to be used - I2C,
   * Hardware SPI and Software SPI. When called with no parameters the I2C mode is enabled and the
   * I2C bus is scanned for the first BME280 (typically at 0x76 or 0x77 unless an I2C expander is
   * used to remap the address.
   * @param[in] chipSelect Hardware SPI CS chip
   * @param[in] mosi Master-Out Slave-In pin
   * @param[in] miso Master-In Slave-Out pin
   * @param[in] sck  System Clock
   * @return  returns "true" when the class initialized correctly
   */
  _cs   = chipSelect;
  _mosi = mosi;
  _miso = miso;
  _sck  = sck;                                       // Store SPI pins
  digitalWrite(_cs, HIGH);                           // High means ignore master
  pinMode(_cs, OUTPUT);                              // Make the chip select pin output
  pinMode(_sck, OUTPUT);                             // Make system clock pin output
  pinMode(_mosi, OUTPUT);                            // Make master-out slave-in output
  pinMode(_miso, INPUT);                             // Make master-in slave-out input
  if (readByte(BME280_CHIPID_REG) == BME280_CHIPID)  // check for correct chip id
  {
    getCalibration();  // get the calibration values
    return true;
  }  // of if-then device is really a BME280
  return false;
}  // of method begin()
void BME280_Class::getCalibration() {
  /*!
   * @brief     reads the calibration register data into local variables for use in converting
   * readings
   */
  getData(BME280_T1_REG, _cal_dig_T1);
  getData(BME280_T2_REG, _cal_dig_T2);
  getData(BME280_T3_REG, _cal_dig_T3);
  getData(BME280_P1_REG, _cal_dig_P1);
  getData(BME280_P2_REG, _cal_dig_P2);
  getData(BME280_P3_REG, _cal_dig_P3);
  getData(BME280_P4_REG, _cal_dig_P4);
  getData(BME280_P5_REG, _cal_dig_P5);
  getData(BME280_P6_REG, _cal_dig_P6);
  getData(BME280_P7_REG, _cal_dig_P7);
  getData(BME280_P8_REG, _cal_dig_P8);
  getData(BME280_P9_REG, _cal_dig_P9);
  getData(BME280_H1_REG, _cal_dig_H1);
  getData(BME280_H2_REG, _cal_dig_H2);
  getData(BME280_H3_REG, _cal_dig_P3);
  uint8_t tempVar;  // Single-Byte temporary variable
  getData(BME280_H4_REG, tempVar);
  _cal_dig_H4 = tempVar << 4;
  getData(BME280_H4_REG + 1, tempVar);
  _cal_dig_H4 |= tempVar & 0xF;
  getData(BME280_H5_REG + 2, tempVar);
  _cal_dig_H5 = tempVar << 4;
  getData(BME280_H5_REG, tempVar);
  _cal_dig_H5 |= tempVar >> 4;
  getData(BME280_H6_REG, _cal_dig_H6);
  _cal_dig_H5 = (readByte(BME280_H5_REG + 1) << 4) | (readByte(BME280_H5_REG) >> 4);
  _cal_dig_H6 = readByte(BME280_H6_REG);
}  // of method getCalibration()
uint8_t BME280_Class::readByte(const uint8_t addr) {
  /*!
   * @brief     interlude function to the getData() function. Reads 1 byte from the given address
   * @param[in] addr Address to read data from
   * @return    returns byte of data read
   */
  uint8_t returnValue;         // Storage for returned value
  getData(addr, returnValue);  // Read just one byte
  return (returnValue);        // Return byte just read
}  // of method readByte()

uint8_t BME280_Class::mode(const uint8_t operatingMode) {
  /*!
   * @brief     sets the current mode bits or returns the current value if the parameter isn't used
   * @param[in] operatingMode Device operating mode to set
   * @return    new mode
   */
  uint8_t controlRegister = readByte(BME280_CONTROL_REG);  // Get the control register
  if (operatingMode == UINT8_MAX)
    return (controlRegister & B00000011);                   // Return setting if no parameter
  _mode           = operatingMode & B00000011;              // Mask 2 bits in input parameter
  controlRegister = (controlRegister & B11111100) | _mode;  // set the new value
  putData(BME280_CONTROL_REG, controlRegister);             // Write value back to register
  return (_mode);
}  // of method mode()

bool BME280_Class::setOversampling(const uint8_t sensor, const uint8_t sampling) {
  /*!
   * @brief     sets the oversampling mode for the sensor
   * @details   see enumerated sensorTypes for list of values. Set to a valid oversampling rate as
   * defined in the enumerated type oversamplingTypes. If either value is out of range or another
   * error occurs then the return value is false.
   * @param[in] sensor Which sensor to set
   * @param[in] sampling Sampling rate
   * @return    Always returns "true"
   */
  if (sensor >= UnknownSensor || sampling >= UnknownOversample)
    return (false);                                        // return error if out of range
  uint8_t originalControl = readByte(BME280_CONTROL_REG);  // Read the control register
  while (readByte(BME280_CONTROL_REG) != 0)
    putData(BME280_CONTROL_REG, 0);  // Put BME280 into sleep mode
  if (sensor == HumiditySensor)      // If we have a humidity setting
  {
    putData(BME280_CONTROLHUMID_REG, sampling);  // Update humidity register
  } else if (sensor == TemperatureSensor)        // otherwise if we have temperature
  {
    originalControl = (originalControl & B00011111) | (sampling << 5);  // Update the register bits
  } else {
    originalControl = (originalControl & B11100011) | (sampling << 2);  // Update the register bits
  }                                              // of if-then-else temperature reading
  putData(BME280_CONTROL_REG, originalControl);  // Write value to the register
  return (true);
}  // of method setOversampling()

uint8_t BME280_Class::getOversampling(const uint8_t sensor, const bool actual) {
  /*!
   * @brief     retrieves the oversampling value for the sensor
   * @details   see enumerated sensorTypes for list of values.
   * @param[in] sensor Which sensor to retrieve
   * @param[in] actual return the actual value if set, otherwise return the raw value
   * @return    return value
   */
  uint8_t returnValue;                      // Get space for return value
  if (sensor >= UnknownSensor) return (0);  // return a zero if out of range
  if (sensor == HumiditySensor)             // If we have a humidity setting, read the buffer bits
    returnValue = readByte(BME280_CONTROLHUMID_REG) & B00000111;
  else if (sensor == TemperatureSensor)  // otherwise if we have temperature
    returnValue = readByte(BME280_CONTROL_REG) >> 5;
  else
    returnValue = (readByte(BME280_CONTROL_REG) >> 2) & B00000111;
  if (actual)  // If the actual flag has been set then return the oversampling
  {
    if (returnValue == 3)
      returnValue = 4;
    else if (returnValue == 4)
      returnValue = 8;
    else if (returnValue > 4)
      returnValue = 16;
  }                      // of if-then we return the actual count
  return (returnValue);  // return oversampling bits
}  // of method getOversampling()

void BME280_Class::readSensors() {
  /*!
   * @brief     reads all 3 sensor values from the registers
   * @details   Read all 3 in one operation and then proceeds to convert the raw temperature,
   * pressure and humidity readings into standard metric units in the BME280's documentation but the
   * math used below was taken from Adafruit's Adafruit_BME280_Library at
   * https://github.com/adafruit/Adafruit_BME280_Library. I think it can be refactored into more
   * efficient code at some point in the future, but it does work correctly
   */
  uint8_t registerBuffer[8];
  int64_t i, j, p;
  if ((_mode == ForcedMode || _mode == ForcedMode2) && mode() == SleepMode)
    mode(_mode);  // Force a reading if necessary
  while ((readByte(BME280_STATUS_REG) & B00001001) != 0)
    ;                                                // wait for measurement to complete
  getData(BME280_PRESSUREDATA_REG, registerBuffer);  // read all 8 bytes in one go
                                                     //*******************************//
                                                     // First compute the temperature //
                                                     //*******************************//
  _Temperature = (int32_t)registerBuffer[3] << 12 | (int32_t)registerBuffer[4] << 4 |
                 (int32_t)registerBuffer[5] >> 4;
  i = ((((_Temperature >> 3) - ((int32_t)_cal_dig_T1 << 1))) * ((int32_t)_cal_dig_T2)) >> 11;
  j = (((((_Temperature >> 4) - ((int32_t)_cal_dig_T1)) *
         ((_Temperature >> 4) - ((int32_t)_cal_dig_T1))) >>
        12) *
       ((int32_t)_cal_dig_T3)) >>
      14;
  _tfine       = i + j;
  _Temperature = (_tfine * 5 + 128) >> 8;  // In centi-degrees Celsius
                                           //*******************************//
                                           // Now compute the pressure      //
                                           //*******************************//
  _Pressure = (int32_t)registerBuffer[0] << 12 | (int32_t)registerBuffer[1] << 4 |
              (int32_t)registerBuffer[2] >> 4;
  i = ((int64_t)_tfine) - 128000;
  j = i * i * (int64_t)_cal_dig_P6;
  j = j + ((i * (int64_t)_cal_dig_P5) << 17);
  j = j + (((int64_t)_cal_dig_P4) << 35);
  i = ((i * i * (int64_t)_cal_dig_P3) >> 8) + ((i * (int64_t)_cal_dig_P2) << 12);
  i = (((((int64_t)1) << 47) + i)) * ((int64_t)_cal_dig_P1) >> 33;
  if (i == 0)
    _Pressure = 0;  // avoid division by 0 exception
  else {
    p         = 1048576 - _Pressure;
    p         = (((p << 31) - j) * 3125) / i;
    i         = (((int64_t)_cal_dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    j         = (((int64_t)_cal_dig_P8) * p) >> 19;
    p         = ((p + i + j) >> 8) + (((int64_t)_cal_dig_P7) << 4);
    _Pressure = p >> 8;  // in pascals
  }                      // of if pressure would cause error
                         //**********************************//
                         // And finally compute the humidity //
                         //**********************************//
  _Humidity = (int32_t)registerBuffer[6] << 8 | (int32_t)registerBuffer[7];
  i         = (_tfine - ((int32_t)76800));
  i = (((((_Humidity << 14) - (((int32_t)_cal_dig_H4) << 20) - (((int32_t)_cal_dig_H5) * i)) +
         ((int32_t)16384)) >>
        15) *
       (((((((i * ((int32_t)_cal_dig_H6)) >> 10) *
            (((i * ((int32_t)_cal_dig_H3)) >> 11) + ((int32_t)32768))) >>
           10) +
          ((int32_t)2097152)) *
             ((int32_t)_cal_dig_H2) +
         8192) >>
        14));
  i = (i - (((((i >> 15) * (i >> 15)) >> 7) * ((int32_t)_cal_dig_H1)) >> 4));
  i = (i < 0) ? 0 : i;
  i = (i > 419430400) ? 419430400 : i;
  _Humidity = (uint32_t)(i >> 12) * 100 / 1024;  // in percent * 100
}  // of method readSensors()

uint8_t BME280_Class::iirFilter(const uint8_t iirFilterSetting) {
  /*!
   * @brief     Set iir filter
   * @details   when called with no parameters returns the current IIR Filter setting, otherwise
   * when called with one parameter will set the IIR filter value and return the new setting
   * @param[in] iirFilterSetting Set iir
   * @return    Inactive time time
   */
  uint8_t returnValue = readByte(BME280_CONFIG_REG);  // Get control register byte
  if (iirFilterSetting == UINT8_MAX)
    return ((returnValue >> 2) & B00000111);           // return the current setting
  returnValue = returnValue & B11100011;               // Get control reg, mask IIR bits
  returnValue |= (iirFilterSetting & B00000111) << 2;  // use 3 bits of iirFilterSetting
  putData(BME280_CONFIG_REG, returnValue);             // Write new control register value
  returnValue = (returnValue >> 2) & B00000111;        // Extract IIR filter setting
  return (returnValue);                                // Return IIR Filter setting
}  // of method iirFilter()

uint8_t BME280_Class::inactiveTime(const uint8_t inactiveTimeSetting) {
  /*!
   * @brief     Return the inactive time setting
   * @details   when called with no parameters returns the current inactive time setting, otherwise
   * uses the parameter to set the inactive time
   * @param[in] inactiveTimeSetting
   * @return    inactive time setting
   */
  uint8_t returnValue = readByte(BME280_CONFIG_REG);  // Get control register
  if (inactiveTimeSetting != UINT8_MAX)               // If we have a specified value
  {
    returnValue = (returnValue & B00011111) |
                  (inactiveTimeSetting << 5);  // use 3 bits of inactiveTimeSetting
    putData(BME280_CONFIG_REG, returnValue);   // Write new control register value
  }                                            // of if-then we have specified a new setting
  return (returnValue >> 5);                   // Return inactive time setting
}  // of method inactiveTime()

uint32_t BME280_Class::measurementTime(const uint8_t measureTimeSetting) {
  /*!
   * @brief     returns the time in microseconds for a measurement cycle with the current settings
   * @details   A cycle includes a temperature, pressure and humidity reading plus the wait time
   * @param[in] measureTimeSetting
   * @return    measurement cycle time
   */
  uint32_t Time1, Time2;
  uint32_t returnValue = inactiveTime();  // Get inactive time value
  switch (returnValue)                    // Set inactive time according to
  {
    case inactiveHalf:
      returnValue = 500;
      break;
    case inactive63ms:
      returnValue = 62500;
      break;
    case inactive125ms:
      returnValue = 125000;
      break;
    case inactive250ms:
      returnValue = 250000;
      break;
    case inactive500ms:
      returnValue = 500000;
      break;
    case inactive1000ms:
      returnValue = 1000000;
      break;
    case inactive10ms:
      returnValue = 10000;
      break;
    case inactive20ms:
      returnValue = 20000;
      break;
  }                                          // of switch for inactive time code
  if (measureTimeSetting != TypicalMeasure)  // Set timing factors for Typ / Max
  {
    returnValue += 1250;
    Time1 = 2300;
    Time2 = 575;
  } else {
    returnValue += 1000;
    Time1 = 2000;
    Time2 = 500;
  }  // of if-then-else typical time or maximum time
  if (getOversampling(TemperatureSensor))
    returnValue += Time1 * getOversampling(TemperatureSensor, true);
  if (getOversampling(PressureSensor))
    returnValue += (Time1 * getOversampling(PressureSensor, true)) + Time2;
  if (getOversampling(HumiditySensor))
    returnValue += (Time1 * getOversampling(HumiditySensor, true)) + Time2;
  return (returnValue);
}  // of method measurementTime()

void BME280_Class::getSensorData(int32_t &temp, int32_t &hum, int32_t &press) {
  /*!
   * @brief      returns the most recent temperature, humidity and pressure readings
   * @param[out] temp  temperature value from device
   * @param[out] hum   humidity value from device
   * @param[out] press pressure value from device
   */
  readSensors();         // Get compensated data from BME280
  temp  = _Temperature;  // Copy global variable to parameter
  hum   = _Humidity;
  press = _Pressure;
}  // of method getSensorData()

void BME280_Class::reset() {
  /*!
   * @brief      performs a device reset, as if it were powered down and back up again
   */
  putData(BME280_SOFTRESET_REG, BME280_SOFTWARE_CODE);  // writing code here resets device
  if (_I2CAddress)
    begin();  // Start device again if I2C
  else if (_sck)
    begin(_cs, _mosi, _miso, _sck);  // Use software serial again
  else
    begin(_cs);  // otherwise it must be hardware SPI
}  // of method reset()

/*! @file Zanshin_BME680.cpp
    @section Zanshin_BME680_intro_section Description

    Arduino Library for the Bosch BME680 sensor\n\n
    See the main library header file for all details
*/

#include "Zanshin_BME680.h"  // Include the library header definition

/***************************************************************************************************
** Declare class-internal and not publically visible constants                                    **
***************************************************************************************************/
const uint8_t BME680_STATUS_REGISTER{0x1D};            ///< Device status register
const uint8_t BME680_GAS_HEATER_REGISTER0{0x5A};       ///< Heater Register 0 address
const uint8_t BME680_GAS_DURATION_REGISTER0{0x64};     ///< Heater Register 0 address
const uint8_t BME680_CONTROL_GAS_REGISTER1{0x70};      ///< Gas control register on/off
const uint8_t BME680_CONTROL_GAS_REGISTER2{0x71};      ///< Gas control register settings
const uint8_t BME680_CONTROL_HUMIDITY_REGISTER{0x72};  ///< Humidity control register
const uint8_t BME680_SPI_REGISTER{0x73};               ///< Status register for SPI memory
const uint8_t BME680_CONTROL_MEASURE_REGISTER{0x74};   ///< Temp, Pressure control register
const uint8_t BME680_CONFIG_REGISTER{0x75};            ///< Configuration register
const uint8_t BME680_CHIPID_REGISTER{0xD0};            ///< Chip-Id register
const uint8_t BME680_SOFTRESET_REGISTER{0xE0};         ///< Reset when 0xB6 is written here
const uint8_t BME680_CHIPID{0x61};                     ///< Hard-coded value 0x61 for BME680
const uint8_t BME680_RESET_CODE{0xB6};                 ///< Reset when this put in reset reg
const uint8_t BME680_MEASURING_BIT_POSITION{5};        ///< Bit position for measuring flag
const uint8_t BME680_I2C_MIN_ADDRESS{0x76};            ///< Minimum possible address for BME680
const uint8_t BME680_I2C_MAX_ADDRESS{0x77};            ///< Minimum possible address for BME680
const uint8_t BME680_SPI_MEM_PAGE_POSITION{4};         ///< Bit position for memory page value
const uint8_t BME680_HUMIDITY_MASK{0xF8};              ///< Mask is binary B11111000
const uint8_t BME680_PRESSURE_MASK{0xE3};              ///< Mask is binary B11100011
const uint8_t BME680_TEMPERATURE_MASK{0x1F};           ///< Mask is binary B00011111
/***************************************************************************************************
** Declare the constants used for calibration                                                     **
***************************************************************************************************/
const uint8_t BME680_COEFF_SIZE1{25};                 ///< First array with coefficients
const uint8_t BME680_COEFF_SIZE2{16};                 ///< Second array with coefficients
const uint8_t BME680_COEFF_START_ADDRESS1{0x89};      ///< start address for array 1
const uint8_t BME680_COEFF_START_ADDRESS2{0xE1};      ///< start address for array 2
const uint8_t BME680_HUM_REG_SHIFT_VAL{4};            ///< Ambient humidity shift value
const uint8_t BME680_BIT_H1_DATA_MSK{0x0F};           ///< Mask for humidity
const uint8_t BME680_T2_LSB_REG{1};                   ///< Register for temperature calibration
const uint8_t BME680_T2_MSB_REG{2};                   ///< Register for temperature calibration
const uint8_t BME680_T3_REG{3};                       ///< Register for temperature calibration
const uint8_t BME680_P1_LSB_REG{5};                   ///< Register for pressure calibration
const uint8_t BME680_P1_MSB_REG{6};                   ///< Register for pressure calibration
const uint8_t BME680_P2_LSB_REG{7};                   ///< Register for pressure calibration
const uint8_t BME680_P2_MSB_REG{8};                   ///< Register for pressure calibration
const uint8_t BME680_P3_REG{9};                       ///< Register for pressure calibration
const uint8_t BME680_P4_LSB_REG{11};                  ///< Register for pressure calibration
const uint8_t BME680_P4_MSB_REG{12};                  ///< Register for pressure calibration
const uint8_t BME680_P5_LSB_REG{13};                  ///< Register for pressure calibration
const uint8_t BME680_P5_MSB_REG{14};                  ///< Register for pressure calibration
const uint8_t BME680_P7_REG{15};                      ///< Register for pressure calibration
const uint8_t BME680_P6_REG{16};                      ///< Register for pressure calibration
const uint8_t BME680_P8_LSB_REG{19};                  ///< Register for pressure calibration
const uint8_t BME680_P8_MSB_REG{20};                  ///< Register for pressure calibration
const uint8_t BME680_P9_LSB_REG{21};                  ///< Register for pressure calibration
const uint8_t BME680_P9_MSB_REG{22};                  ///< Register for pressure calibration
const uint8_t BME680_P10_REG{23};                     ///< Register for pressure calibration
const uint8_t BME680_H2_MSB_REG{0};                   ///< Register for humidity calibration
const uint8_t BME680_H2_LSB_REG{1};                   ///< Register for humidity calibration
const uint8_t BME680_H1_LSB_REG{1};                   ///< Register for humidity calibration
const uint8_t BME680_H1_MSB_REG{2};                   ///< Register for humidity calibration
const uint8_t BME680_H3_REG{3};                       ///< Register for humidity calibration
const uint8_t BME680_H4_REG{4};                       ///< Register for humidity calibration
const uint8_t BME680_H5_REG{5};                       ///< Register for humidity calibration
const uint8_t BME680_H6_REG{6};                       ///< Register for humidity calibration
const uint8_t BME680_H7_REG{7};                       ///< Register for humidity calibration
const uint8_t BME680_T1_LSB_REG{8};                   ///< Register for gas calibration
const uint8_t BME680_T1_MSB_REG{9};                   ///< Register for gas calibration
const uint8_t BME680_GH2_LSB_REG{10};                 ///< Register for gas calibration
const uint8_t BME680_GH2_MSB_REG{11};                 ///< Register for gas calibration
const uint8_t BME680_GH1_REG{12};                     ///< Register for gas calibration
const uint8_t BME680_GH3_REG{13};                     ///< Register for gas calibration
const uint8_t BME680_ADDR_RES_HEAT_RANGE_ADDR{0x02};  ///< Register for gas calibration
const uint8_t BME680_RHRANGE_MSK{0x30};               ///< Register for gas calibration
const uint8_t BME680_ADDR_RES_HEAT_VAL_ADDR{0x00};    ///< Register for gas calibration
const uint8_t BME680_ADDR_RANGE_SW_ERR_ADDR{0x04};    ///< Register for gas calibration
const uint8_t BME680_RSERROR_MSK{0xF0};               ///< Register for gas calibration

BME680_Class::BME680_Class() {
  /*!
   * @brief   Class constructor
   * @details Currently empty and unused
   */
}  // of class constructor
BME680_Class::~BME680_Class() {
  /*!
   * @brief   Class destructor
   * @details Currently empty and unused
   */
}  // of class destructor
/***********************************************************************************************
** The "begin()" function is overloaded to allow for variations in I2C and SPI initialization **
***********************************************************************************************/
bool BME680_Class::begin() {
  /*!
  * @brief   starts communications with device (overloaded)
  * @details When called with no parameters the I2C protocol with I2C_STANDARD_MODE speed is
             selected. The I2C network is searched for the first BME680 device.
  * return "true" if successful otherwise false
  */
  return begin(I2C_STANDARD_MODE, 0);  // Initialize I2C with standard speed, allow any i2c address
}  // of method begin()
bool BME680_Class::begin(const uint32_t i2cSpeed) {
  /*!
  @brief   starts communications with device (overloaded)
  @details When called with a 32-bit parameter it is assumed that the I2C protocol is to be used
           and the speed setting is determined by the parameter.
  @param[in] i2cSpeed I2C Speed value
  return "true" if successful otherwise false
  */
  return begin(i2cSpeed, 0);  // Initialize I2C with given speed, allow any i2c address
}  // of method begin()
bool BME680_Class::begin(const uint32_t i2cSpeed, const uint8_t i2cAddress) {
  /*!
  @brief   starts communications with device (overloaded)
  @details When called with a 32-bit parameter it is assumed that the I2C protocol is to be used
           and the speed setting is determined by the parameter. If the i2cAddress is 0 then check
           both, otherwise check only only the specified address
  @param[in] i2cSpeed I2C Speed value
  @param[in] i2cAddress I2C Address, use 0 to self-determine
  return "true" if successful otherwise false
  */
  Wire.begin();             // Start I2C as master
  Wire.setClock(i2cSpeed);  // and set bus speed
  _I2CSpeed = i2cSpeed;
  for (_I2CAddress = BME680_I2C_MIN_ADDRESS; _I2CAddress <= BME680_I2C_MAX_ADDRESS;
       _I2CAddress++) {                                  // Loop through all I2C addresses
    if (i2cAddress == 0 || _I2CAddress == i2cAddress) {  // Only check if relevant
      Wire.beginTransmission(_I2CAddress);               // Check for BME680 here
      if (Wire.endTransmission() == 0) {                 // We have found a device that could be
        return commonInitialization();                   // a BME680, so perform common init
      }                                                  // of if-then we have found a device
    }               // of if-then check all or a specific address
  }                 // of for-next each I2C address loop
  _I2CAddress = 0;  // Set to denote no device found
  _I2CSpeed   = 0;  // Set to denote no device found
  return false;     // return failure if we get here
}  // of method begin()
bool BME680_Class::begin(const uint8_t chipSelect) {
  /*!
  @brief     starts communications with device (overloaded)
  @details   When called with a single 8-bit parameter is assumed that hardware SPI is to be used
             if the value is not 0x76 or 0x77, in which case it is assumed that it is the I2C addr
  @param[in] chipSelect Arduino Pin number for the Slave-Select pin or the I2C address
  return     "true" if successful otherwise false
  */
  if (chipSelect == BME680_I2C_MIN_ADDRESS || chipSelect == BME680_I2C_MAX_ADDRESS) {
    // If 0x76 or 0x77 then we have I2C
    return begin(I2C_STANDARD_MODE, chipSelect);  // Std speed with explicit address
  }                                               // if-then we have an I2C call
  _cs = chipSelect;                               // Store value for future use
  digitalWrite(_cs, HIGH);                        // High means ignore master
  pinMode(_cs, OUTPUT);                           // Make the chip select pin output
  SPI.begin();                                    // Start hardware SPI
  return commonInitialization();                  // Perform common initialization
}  // of method begin()
bool BME680_Class::begin(const uint8_t chipSelect, const uint8_t mosi, const uint8_t miso,
                         const uint8_t sck) {
  /*!
  @brief   starts communications with device (overloaded)
  @details When called with 4 parameters then software SPI is assumed
  @param[in] chipSelect Arduino Pin number for the Slave-Select pin
  @param[in] mosi       Arduino Pin number for the Master-out-Slave-in pin
  @param[in] miso       Arduino Pin number for the Master-in-Slave-out pin
  @param[in] sck        Arduino Pin number for the System clock pin
  return "true" if successful otherwise false
  */
  _cs   = chipSelect;
  _mosi = mosi;
  _miso = miso;
  _sck  = sck;                    // Store SPI pins
  digitalWrite(_cs, HIGH);        // High means ignore master
  pinMode(_cs, OUTPUT);           // Make the chip select pin output
  pinMode(_sck, OUTPUT);          // Make system clock pin output
  pinMode(_mosi, OUTPUT);         // Make master-out slave-in output
  pinMode(_miso, INPUT);          // Make master-in slave-out input
  return commonInitialization();  // Perform common initialization
}  // of method begin()
bool BME680_Class::commonInitialization() {
  /*!
  @brief   Common BME680 initialization function
  @details Called from all of the overloaded "begin()" functions once the communications protocol
           has been selected return "true" if successful otherwise false
  */
  uint8_t SPI_Register = readByte(BME680_SPI_REGISTER);  // Read SPI reg. byte
  if (_I2CAddress == 0 &&
      bitRead(SPI_Register, BME680_SPI_MEM_PAGE_POSITION)) {    // Wrong mode for ID
    bitWrite(SPI_Register, BME680_SPI_MEM_PAGE_POSITION, 0);    // Turn off Page bit to go to Page 0
    putData(BME680_SPI_REGISTER, SPI_Register);                 // Write value to register
  }                                                             // of if-then we are in SPI mode
  if (readByte(BME680_CHIPID_REGISTER) == BME680_CHIPID) {      // check for correct chip id
    getCalibration();                                           // get the calibration values
    if (_I2CAddress == 0) {                                     // If using SPI, switch to correct
      bitWrite(SPI_Register, BME680_SPI_MEM_PAGE_POSITION, 1);  // Page "1" again
      putData(BME680_SPI_REGISTER, SPI_Register);               // Update register value
    }                                                           // of if-then SPI mode
    uint8_t workRegister = readByte(BME680_CONTROL_MEASURE_REGISTER);  // Read the control measure
    putData(BME680_CONTROL_MEASURE_REGISTER,
            (uint8_t)(workRegister | 1));  // Trigger 1st measurement
    return true;
  }  // of if-then device is really a BME680
  else
    return false;
}  // of method commonInitialization
uint8_t BME680_Class::readByte(const uint8_t addr) const {
  /*!
  @brief   Read a single byte from the given address
  @details interlude function to the "getData()" function. This is called so that the return value
           is the byte of data that has just been read rather than the number of bytes read
           returned by "getData()"
  param[in] addr Address of device
  return    single byte of read from addr
  */
  uint8_t returnValue;         // Storage for returned value
  getData(addr, returnValue);  // Read just one byte
  return (returnValue);        // Return byte just read
}  // of method readByte()
void BME680_Class::reset() {
  /*!
   * @brief   Performs a device reset
   */
  putData(BME680_SOFTRESET_REGISTER, BME680_RESET_CODE);  // write reset code to device
  delay(2);                                               // Datasheet states 2ms Start-up time
  if (_I2CAddress) {                                      // Branch depending on if using I2C or SPI
    begin(_I2CSpeed, _I2CAddress);                        // If I2C start device with same settings
  } else {
    if (_sck) {                        // If _sck is set the use software SPI
      begin(_cs, _mosi, _miso, _sck);  // Use software SPI again
    } else {
      begin(_cs);  // otherwise it must be hardware SPI
    }              // if-then-else using sw or hw SPI
  }                // if-then-else using I2C
}  // of method reset()
void BME680_Class::getCalibration() {
  /*!
  @brief    reads the calibration register data into local variables for use in converting readings
  @details  The calibration registers are read into a temporary array and then parsed into the
            appropriate calibration variables, this was taken from the example BOSCH software and
            minimizes register reads, but makes it rather difficult to read. This will be redone
            for legibility at some point in the future
  param[in] addr Address of device
  return    single byte read
  */
  /*************************************
  ** Temperature related coefficients **
  *************************************/
  uint8_t coeff_arr1[BME680_COEFF_SIZE1] = {0};      // Define temp array 1 and initialize with 0x
  uint8_t coeff_arr2[BME680_COEFF_SIZE2] = {0};      // Define temp array 2 and initialize with 0s
  getData(BME680_COEFF_START_ADDRESS1, coeff_arr1);  // Split reading registers into 2
  getData(BME680_COEFF_START_ADDRESS2, coeff_arr2);  // one 25 bytes and the other 16
  _T1 = (uint16_t)(CONCAT_BYTES(coeff_arr2[BME680_T1_MSB_REG], coeff_arr2[BME680_T1_LSB_REG]));
  _T2 = (int16_t)(CONCAT_BYTES(coeff_arr1[BME680_T2_MSB_REG], coeff_arr1[BME680_T2_LSB_REG]));
  _T3 = (int8_t)(coeff_arr1[BME680_T3_REG]);
  /*************************************
  ** Pressure related coefficients    **
  *************************************/
  _P1  = (uint16_t)(CONCAT_BYTES(coeff_arr1[BME680_P1_MSB_REG], coeff_arr1[BME680_P1_LSB_REG]));
  _P2  = (int16_t)(CONCAT_BYTES(coeff_arr1[BME680_P2_MSB_REG], coeff_arr1[BME680_P2_LSB_REG]));
  _P3  = (int8_t)coeff_arr1[BME680_P3_REG];
  _P4  = (int16_t)(CONCAT_BYTES(coeff_arr1[BME680_P4_MSB_REG], coeff_arr1[BME680_P4_LSB_REG]));
  _P5  = (int16_t)(CONCAT_BYTES(coeff_arr1[BME680_P5_MSB_REG], coeff_arr1[BME680_P5_LSB_REG]));
  _P6  = (int8_t)(coeff_arr1[BME680_P6_REG]);
  _P7  = (int8_t)(coeff_arr1[BME680_P7_REG]);
  _P8  = (int16_t)(CONCAT_BYTES(coeff_arr1[BME680_P8_MSB_REG], coeff_arr1[BME680_P8_LSB_REG]));
  _P9  = (int16_t)(CONCAT_BYTES(coeff_arr1[BME680_P9_MSB_REG], coeff_arr1[BME680_P9_LSB_REG]));
  _P10 = (uint8_t)(coeff_arr1[BME680_P10_REG]);
  /**********************************
  ** Humidity related coefficients **
  **********************************/
  _H1 = (uint16_t)(
      ((uint16_t)coeff_arr2[BME680_H1_MSB_REG] << BME680_HUM_REG_SHIFT_VAL) |
      ((coeff_arr2[BME680_H1_LSB_REG] >> BME680_HUM_REG_SHIFT_VAL) & BME680_BIT_H1_DATA_MSK));
  _H2 = (uint16_t)(
      ((uint16_t)coeff_arr2[BME680_H2_MSB_REG] << BME680_HUM_REG_SHIFT_VAL) |
      ((coeff_arr2[BME680_H2_LSB_REG] >> BME680_HUM_REG_SHIFT_VAL) & BME680_BIT_H1_DATA_MSK));
  _H3 = (int8_t)coeff_arr2[BME680_H3_REG];
  _H4 = (int8_t)coeff_arr2[BME680_H4_REG];
  _H5 = (int8_t)coeff_arr2[BME680_H5_REG];
  _H6 = (uint8_t)coeff_arr2[BME680_H6_REG];
  _H7 = (int8_t)coeff_arr2[BME680_H7_REG];
  /************************************
  ** Gas heater related coefficients **
  ************************************/
  _G1 = (int8_t)coeff_arr2[BME680_GH1_REG];
  _G2 = (int16_t)(CONCAT_BYTES(coeff_arr2[BME680_GH2_MSB_REG], coeff_arr2[BME680_GH2_LSB_REG]));
  _G3 = (int8_t)coeff_arr2[BME680_GH3_REG];
  uint8_t temp_var = 0;
  getData(BME680_ADDR_RES_HEAT_RANGE_ADDR, temp_var);
  _res_heat_range = ((temp_var & BME680_RHRANGE_MSK) / 16);
  getData(BME680_ADDR_RES_HEAT_VAL_ADDR, temp_var);
  _res_heat = (int8_t)temp_var;
  getData(BME680_ADDR_RANGE_SW_ERR_ADDR, temp_var);
  _rng_sw_err = ((int8_t)temp_var & (int8_t)BME680_RSERROR_MSK) / 16;
}  // of method getCalibration()
uint8_t BME680_Class::setOversampling(const uint8_t sensor, const uint8_t sampling) const {
  /*!
  @brief   sets the oversampling mode for the sensor
  @details See enumerated "sensorTypes" for a list of values. Set to a valid oversampling rate as
           defined in the enumerated type oversamplingTypes. If either value is out of range or
           another error occurs then the return value is false.
  param[in] sensor Enumerated sensor type
  param[in] sampling Sampling rate from enumerated type
  return "true" if successful otherwise false
  */
  if (sensor >= UnknownSensor ||
      (sampling != UINT8_MAX &&
       sampling >= UnknownOversample)) {  // return an error if sensor or oversample is out of range
    return (UINT8_MAX);
  }
  uint8_t tempRegister;            // Temporary register variable
  uint8_t returnValue = sampling;  // Return sampling value
  waitForReadings();               // Ensure any active reading is finished
  switch (sensor) {                // Depending upon which sensor is chosen
    case HumiditySensor:           // Set the humidity oversampling
    {
      tempRegister = readByte(BME680_CONTROL_HUMIDITY_REGISTER);  // Read the register contents
      if (sampling == UINT8_MAX) {                                // If we just want to read values

        returnValue = tempRegister & ~BME680_HUMIDITY_MASK;  // Set return value
      } else {
        tempRegister &= BME680_HUMIDITY_MASK;  // Mask bits to 0
        tempRegister |= sampling;              // Add in the sampling bits
        putData(BME680_CONTROL_HUMIDITY_REGISTER,
                (uint8_t)tempRegister);  // Update humidity bits 0:2
      }                                  // if-then return current value or set new value
      break;
    }                     // of HumiditySensor
    case PressureSensor:  // Set the pressure oversampling
    {
      tempRegister = readByte(BME680_CONTROL_MEASURE_REGISTER);  // Read the register contents
      if (sampling == UINT8_MAX) {  // If we just want to read the values
        returnValue = (tempRegister & ~BME680_PRESSURE_MASK) >> 2;  // Set return value
      } else {
        tempRegister &= BME680_PRESSURE_MASK;  // Mask bits to 0
        tempRegister |= (sampling << 2);       // Add in sampling bits at offset
        putData(BME680_CONTROL_MEASURE_REGISTER, (uint8_t)tempRegister);  // Update register
      }  // if-then return current value or set new value
      break;
    }                        // of PressureSensor
    case TemperatureSensor:  // Set the temperature oversampling
    {
      tempRegister = readByte(BME680_CONTROL_MEASURE_REGISTER);  // Read the register contents
      if (sampling == UINT8_MAX) {  // If we just want to read the values
        returnValue = (tempRegister & ~BME680_TEMPERATURE_MASK) >> 5;  // Set return value
      } else {
        tempRegister &= BME680_TEMPERATURE_MASK;  // Mask bits to 0
        tempRegister |= (sampling << 5);          // Add in the sampling bits at offset
        putData(BME680_CONTROL_MEASURE_REGISTER,
                (uint8_t)tempRegister);  // Update humidity bits 5:7
      }                                  // if-then return current value or set new value
      break;
    }                             // of TemperatureSensor
    default: return (UINT8_MAX);  // Return an error if no match
  }                               // of switch the sensor type
  return (returnValue);           // Otherwise return current value
}  // of method setOversampling()
uint8_t BME680_Class::setIIRFilter(const uint8_t iirFilterSetting) const {
  /*!
   @brief   Set or return the current IIR filter setting
   @details When called with no parameters returns the current IIR Filter setting, otherwise
   when called with one parameter will set the IIR filter value and return the new setting
   param[in] iirFilterSetting New setting
   return   IIR Filter setting
   */
  waitForReadings();                                       // Ensure any active reading is finished
  uint8_t returnValue = readByte(BME680_CONFIG_REGISTER);  // Get control register byte contents
  if (iirFilterSetting != UINT8_MAX)                       // If the value is to be changed
  {                                                        //
    returnValue = returnValue & B11100011;                 // mask IIR bits
    returnValue |= (iirFilterSetting & B00000111) << 2;    // use 3 bits of iirFilterSetting
    putData(BME680_CONFIG_REGISTER, returnValue);          // Write new control register value
  }  // if the value is to be changed                                   //
  returnValue = (returnValue >> 2) & B00000111;  // Extract IIR filter setting from register
  return (returnValue);                          // Return IIR Filter setting
}  // of method setIIRFilter()
uint8_t BME680_Class::getSensorData(int32_t& temp, int32_t& hum, int32_t& press, int32_t& gas,
                                    const bool waitSwitch) {
  /*!
   @brief   Returns the most recent temperature, humidity and pressure readings
   param[out] temp       Temperature reading
   param[out] hum        Humidity reading
   param[out] press      Pressure reading
   param[out] gas        Gas reading
   param[in]  waitSwitch (Optional) When set will not return until reading is finished
   */
  uint8_t status = readSensors(waitSwitch);  // Get compensated data from BME680
  temp           = _Temperature;             // Copy global variables to parameters
  hum            = _Humidity;                // Copy global variables to parameters
  press          = _Pressure;                // Copy global variables to parameters
  gas            = _Gas;                     // Copy global variables to parameters
  return (status);                           // Nonzero if gas is invalid or unstable
}  // of method getSensorData()
uint8_t BME680_Class::getI2CAddress() const {
  /*!
   @brief   Returns the I2C address of the BME680
   return   I2C Address from private variables
   */
  return (_I2CAddress);
}  // of method getI2CAddress()
uint8_t BME680_Class::readSensors(const bool waitSwitch) {
  /*!
   @brief   reads all 4 sensor values from the registers in one operation and then proceeds to
            convert the raw temperature, pressure & humidity readings into standard metric units
   @details The formula is written in the BME680's documentation but the math used below was
            taken from Adafruit's Adafruit_BME680_Library at
            https://github.com/adafruit/Adafruit_BME680. I think it can be refactored into more
            efficient code at some point in the future, but it does work correctly.
   param[in] waitSwitch (Optional) When set will not return until reading is finished
   */
  /*! Lookup table for the possible gas range values */
  const uint32_t lookupTable1[16] = {
      UINT32_C(2147483647), UINT32_C(2147483647), UINT32_C(2147483647), UINT32_C(2147483647),
      UINT32_C(2147483647), UINT32_C(2126008810), UINT32_C(2147483647), UINT32_C(2130303777),
      UINT32_C(2147483647), UINT32_C(2147483647), UINT32_C(2143188679), UINT32_C(2136746228),
      UINT32_C(2147483647), UINT32_C(2126008810), UINT32_C(2147483647), UINT32_C(2147483647)};
  const uint32_t lookupTable2[16] = {
      UINT32_C(4096000000), UINT32_C(2048000000), UINT32_C(1024000000), UINT32_C(512000000),
      UINT32_C(255744255),  UINT32_C(127110228),  UINT32_C(64000000),   UINT32_C(32258064),
      UINT32_C(16016016),   UINT32_C(8000000),    UINT32_C(4000000),    UINT32_C(2000000),
      UINT32_C(1000000),    UINT32_C(500000),     UINT32_C(250000),     UINT32_C(125000)};

  uint8_t  buff[15], gas_range = 0;                          // declare array for registers
  int64_t  var1, var2, var3, var4, var5, var6, temp_scaled;  // Work variables
  uint32_t adc_temp, adc_pres;                               // Raw ADC temperature and pressure
  uint16_t adc_hum, adc_gas_res;                             // Raw ADC humidity and gas
  if (waitSwitch) waitForReadings();      // Doesn't return until the readings are finished
  getData(BME680_STATUS_REGISTER, buff);  // read all 15 bytes in one go
  adc_pres = (uint32_t)(((uint32_t)buff[2] << 12) | ((uint32_t)buff[3] << 4) |
                        ((uint32_t)buff[4] >> 4));  // put the 3 bytes of Pressure
  adc_temp = (uint32_t)(((uint32_t)buff[5] << 12) | ((uint32_t)buff[6] << 4) |
                        ((uint32_t)buff[7] >> 4));  // put the 3 bytes of Temperature
  adc_hum =
      (uint16_t)(((uint32_t)buff[8] << 8) | (uint32_t)buff[9]);  // put the 2 bytes of Humidity
  adc_gas_res =
      (uint16_t)((uint32_t)buff[13] << 2 | (((uint32_t)buff[14]) >> 6));  // put the 2 bytes of Gas
  gas_range = buff[14] & 0X0F;                                            // Retrieve the range
                                //*******************************//
                                // First compute the temperature //
                                //*******************************//
  var1         = ((int32_t)adc_temp >> 3) - ((int32_t)_T1 << 1);  // Perform calibration/adjustment
  var2         = (var1 * (int32_t)_T2) >> 11;                     // of Temperature values according
  var3         = ((var1 >> 1) * (var1 >> 1)) >> 12;               // to formula defined by Bosch
  var3         = ((var3) * ((int32_t)_T3 << 4)) >> 14;
  _tfine       = (int32_t)(var2 + var3);
  _Temperature = (int16_t)(((_tfine * 5) + 128) >> 8);
  //*******************************//
  // Now compute the pressure      //
  //*******************************//
  var1 = (((int32_t)_tfine) >> 1) - 64000;
  var2 = ((((var1 >> 2) * (var1 >> 2)) >> 11) * (int32_t)_P6) >> 2;
  var2 = var2 + ((var1 * (int32_t)_P5) << 1);
  var2 = (var2 >> 2) + ((int32_t)_P4 << 16);
  var1 = (((((var1 >> 2) * (var1 >> 2)) >> 13) * ((int32_t)_P3 << 5)) >> 3) +
         (((int32_t)_P2 * var1) >> 1);
  var1      = var1 >> 18;
  var1      = ((32768 + var1) * (int32_t)_P1) >> 15;
  _Pressure = 1048576 - adc_pres;
  _Pressure = (int32_t)((_Pressure - (var2 >> 12)) * ((uint32_t)3125));

  if (_Pressure >= INT32_C(0x40000000))  // Issue #26
    _Pressure = ((_Pressure / (uint32_t)var1) << 1);
  else
    _Pressure = ((_Pressure << 1) / (uint32_t)var1);
  var1 = ((int32_t)_P9 * (int32_t)(((_Pressure >> 3) * (_Pressure >> 3)) >> 13)) >> 12;
  var2 = ((int32_t)(_Pressure >> 2) * (int32_t)_P8) >> 13;
  var3 = ((int32_t)(_Pressure >> 8) * (int32_t)(_Pressure >> 8) * (int32_t)(_Pressure >> 8) *
          (int32_t)_P10) >>
         17;
  _Pressure = (int32_t)(_Pressure) + ((var1 + var2 + var3 + ((int32_t)_P7 << 7)) >> 4);
  //**********************//
  // Compute the humidity //
  //**********************//
  temp_scaled = (((int32_t)_tfine * 5) + 128) >> 8;
  var1        = (int32_t)(adc_hum - ((int32_t)((int32_t)_H1 << 4))) -
         (((temp_scaled * (int32_t)_H3) / ((int32_t)100)) >> 1);
  var2 =
      ((int32_t)_H2 *
       (((temp_scaled * (int32_t)_H4) / ((int32_t)100)) +
        (((temp_scaled * ((temp_scaled * (int32_t)_H5) / ((int32_t)100))) >> 6) / ((int32_t)100)) +
        (int32_t)(1 << 14))) >>
      10;
  var3      = var1 * var2;
  var4      = (int32_t)_H6 << 7;
  var4      = ((var4) + ((temp_scaled * (int32_t)_H7) / ((int32_t)100))) >> 4;
  var5      = ((var3 >> 14) * (var3 >> 14)) >> 10;
  var6      = (var4 * var5) >> 1;
  _Humidity = (((var3 + var6) >> 10) * ((int32_t)1000)) >> 12;
  if (_Humidity > 100000) /* Cap at 100%rH */
    _Humidity = 100000;
  else if (_Humidity < 0)
    _Humidity = 0;
  //**********************//
  // Compute the Gas      //
  //**********************//
  uint64_t uvar2;
  var1  = (int64_t)((1340 + (5 * (int64_t)_rng_sw_err)) * ((int64_t)lookupTable1[gas_range])) >> 16;
  uvar2 = (((int64_t)((int64_t)adc_gas_res << 15) - (int64_t)(16777216)) + var1);
  var3  = (((int64_t)lookupTable2[gas_range] * (int64_t)var1) >> 9);
  _Gas  = (uint32_t)((var3 + ((int64_t)uvar2 >> 1)) / (int64_t)uvar2);
  triggerMeasurement();      // trigger the next measurement
  return (buff[14] & 0X30);  // Return nonzero if gas or heat stabilization is invalid
}  // of method readSensors()
void BME680_Class::waitForReadings() const {
  /*!
   @brief   Only returns once a measurement on the BME680 has completed
   */
  while (measuring()) {}  // loop until any active measurment is complete
}  // of method waitForReadings
bool BME680_Class::setGas(uint16_t GasTemp, uint16_t GasMillis) const {
  /*!
   * @brief    sets the gas measurement target temperature and heating time
   * param[in] GasTemp  Target temperature in Celsius
   * param[in] GasMillis Milliseconds to turn on heater
   * return Always returns "true"
   */
  waitForReadings();  // Ensure any active reading is finished
  uint8_t gasRegister = readByte(BME680_CONTROL_GAS_REGISTER2);  // Read current register values
  if (GasTemp == 0 || GasMillis == 0) {
    // If either input variable is zero //
    putData(BME680_CONTROL_GAS_REGISTER1, (uint8_t)B00001000);  // Turn off gas heater
    putData(BME680_CONTROL_GAS_REGISTER2,
            (uint8_t)(gasRegister & B11101111));  // Turn off gas measurements
  } else {
    putData(BME680_CONTROL_GAS_REGISTER1, (uint8_t)0);  // Turn off heater bit to turn on
    uint8_t heatr_res;
    int32_t var1, var2, var3, var4, var5, heatr_res_x100;
    if (GasTemp < 200)
      GasTemp = 200;
    else if (GasTemp > 400)
      GasTemp = 400;  // Clamp temperature to min/max

    var1 = (((int32_t)(_Temperature / 100) * _H3) / 1000) << 8;
    var2 = (_G1 + 784) * (((((_G2 + 154009) * GasTemp * 5) / 100) + 3276800) / 10);  // Issue #26
    var3 = var1 + (var2 / 2);
    var4 = (var3 / (_res_heat_range + 4));
    var5 = (131 * _res_heat) + 65536;
    heatr_res_x100 = (int32_t)(((var4 / var5) - 250) * 34);
    heatr_res      = (uint8_t)((heatr_res_x100 + 50) / 100);
    putData(BME680_GAS_HEATER_REGISTER0, heatr_res);
    uint8_t factor = 0;
    uint8_t durval;
    if (GasMillis >= 0xfc0)
      durval = 0xff;  // Max duration
    else {
      while (GasMillis > 0x3F) {
        GasMillis = GasMillis >> 2;
        factor += 1;
      }  // of while loop
      durval = (uint8_t)(GasMillis + (factor * 64));
    }                                                   // of if-then-else duration exceeds max
    putData(BME680_CONTROL_GAS_REGISTER1, (uint8_t)0);  // then turn off gas heater
    putData(BME680_GAS_DURATION_REGISTER0, durval);
    putData(BME680_CONTROL_GAS_REGISTER2, (uint8_t)(gasRegister | B00010000));
  }  // of if-then-else turn gas measurements on or off
  return true;
}  // of method setGas()
bool BME680_Class::measuring() const {
  /*!
   * @brief Returns whether the BME680 is currently measuring
   * return "true" if a measurement is active, otherwise "false"
   */
  bool result(false);
  if ((readByte(BME680_STATUS_REGISTER) & _BV(BME680_MEASURING_BIT_POSITION)) != 0) {
    result = true;
  }  // if-then device is currently measuring
  return result;
}  // of method "measuring()"
void BME680_Class::triggerMeasurement() const {
  /*!
   * @brief Trigger a new measurement on the BME680
   */
  uint8_t workRegister = readByte(BME680_CONTROL_MEASURE_REGISTER);  // Read the control measure
  putData(BME680_CONTROL_MEASURE_REGISTER,
          (uint8_t)(workRegister | 1));  // Trigger start of next measurement
}  // of method "triggerMeasurement()"
/******************************************************************************
SparkFunLSM6DS3.cpp
LSM6DS3 Arduino, Teensy Driver, SparkFun ESP32 and ESP8266

Marshall Taylor @ SparkFun Electronics
May 20, 2015
https://github.com/sparkfun/LSM6DS3_Breakout
https://github.com/sparkfun/SparkFun_LSM6DS3_Arduino_Library

Resources:
Uses Wire.h for i2c operation
Uses SPI.h for SPI operation
Either can be omitted if not used

Development environment specifics:
Arduino IDE 1.6.4
Teensy loader 1.23
ESPTool 2.6 for ESP32

This code is released under the [MIT License](http://opensource.org/licenses/MIT).

Please review the LICENSE.md file included with this example. If you have any questions 
or concerns with licensing, please contact techsupport@sparkfun.com.

Distributed as-is; no warranty is given.
******************************************************************************/

//See SparkFunLSM6DS3.h for additional topology notes.

#include "SparkFunLSM6DS3.h"

//****************************************************************************//
//
//  LSM6DS3Core functions.
//
//  Construction arguments:
//  ( uint8_t busType, uint8_t inputArg ),
//
//    where inputArg is address for I2C_MODE and chip select pin
//    number for SPI_MODE
//
//  For SPI, construct LSM6DS3Core myIMU(SPI_MODE, 10);
//  For I2C, construct LSM6DS3Core myIMU(I2C_MODE, 0x6B);
//
//  Default construction is I2C mode, address 0x6B.
//
//****************************************************************************//
LSM6DS3Core::LSM6DS3Core( uint8_t busType, uint8_t inputArg) : commInterface(I2C_MODE), I2CAddress(0x6B), chipSelectPin(10)
{
	commInterface = busType;
	if( commInterface == I2C_MODE )
	{
		I2CAddress = inputArg;
	}
	if( commInterface == SPI_MODE )
	{
		chipSelectPin = inputArg;
	}

}

status_t LSM6DS3Core::beginCore(void)
{
	status_t returnError = IMU_SUCCESS;
  uint32_t spiPortSpeed = 5000000;

	switch (commInterface) {

	case I2C_MODE:
		Wire.begin();
		break;

	case SPI_MODE:
		// start the SPI library:
		SPI.begin();
    
#ifdef __AVR__ 
    mySpiSettings = SPISettings(spiPortSpeed, MSBFIRST, SPI_MODE1);
#endif
#if defined(ESP32) || defined(ESP8266)
    mySpiSettings = SPISettings(spiPortSpeed, MSBFIRST, SPI_MODE1);
#endif
#ifdef __MK20DX256__
    mySpiSettings = SPISettings(spiPortSpeed, MSB_FIRST, SPI_MODE0);
#endif
#ifdef ARDUINO_NANO33BLE
    mySpiSettings = SPISettings(spiPortSpeed, MSB_FIRST, SPI_MODE0);
#endif

		pinMode(chipSelectPin, OUTPUT);
		digitalWrite(chipSelectPin, HIGH);
		break;
	default:
		break;
	}

	//Spin for a few ms
	volatile uint8_t temp = 0;
	for( uint16_t i = 0; i < 10000; i++ )
	{
		temp++;
	}

	//Check the ID register to determine if the operation was a success.
	uint8_t readCheck;
	readRegister(&readCheck, LSM6DS3_ACC_GYRO_WHO_AM_I_REG);
	if( readCheck != 0x69 )
	{
		returnError = IMU_HW_ERROR;
	}

	return returnError;

}

//****************************************************************************//
//
//  ReadRegisterRegion
//
//  Parameters:
//    *outputPointer -- Pass &variable (base address of) to save read data to
//    offset -- register to read
//    length -- number of bytes to read
//
//  Note:  Does not know if the target memory space is an array or not, or
//    if there is the array is big enough.  if the variable passed is only
//    two bytes long and 3 bytes are requested, this will over-write some
//    other memory!
//
//****************************************************************************//
status_t LSM6DS3Core::readRegisterRegion(uint8_t *outputPointer , uint8_t offset, uint8_t length)
{
	status_t returnError = IMU_SUCCESS;

	//define pointer that will point to the external space
	uint8_t i = 0;
	uint8_t c = 0;
	uint8_t tempFFCounter = 0;

	switch (commInterface) {

	case I2C_MODE:
		Wire.beginTransmission(I2CAddress);
		Wire.write(offset);
		if( Wire.endTransmission() != 0 )
		{
			returnError = IMU_HW_ERROR;
		}
		else  //OK, all worked, keep going
		{
			// request 6 bytes from slave device
			Wire.requestFrom(I2CAddress, length);
			while ( (Wire.available()) && (i < length))  // slave may send less than requested
			{
				c = Wire.read(); // receive a byte as character
				*outputPointer = c;
				outputPointer++;
				i++;
			}
		}
		break;

	case SPI_MODE:
		// take the chip select low to select the device:
		digitalWrite(chipSelectPin, LOW);
		// send the device the register you want to read:
		SPI.transfer(offset | 0x80);  //Ored with "read request" bit
		while ( i < length ) // slave may send less than requested
		{
			c = SPI.transfer(0x00); // receive a byte as character
			if( c == 0xFF )
			{
				//May have problem
				tempFFCounter++;
			}
			*outputPointer = c;
			outputPointer++;
			i++;
		}
		if( tempFFCounter == i )
		{
			//Ok, we've recieved all ones, report
			returnError = IMU_ALL_ONES_WARNING;
		}
		// take the chip select high to de-select:
		digitalWrite(chipSelectPin, HIGH);
		break;

	default:
		break;
	}

	return returnError;
}

//****************************************************************************//
//
//  ReadRegister
//
//  Parameters:
//    *outputPointer -- Pass &variable (address of) to save read data to
//    offset -- register to read
//
//****************************************************************************//
status_t LSM6DS3Core::readRegister(uint8_t* outputPointer, uint8_t offset) {
	//Return value
	uint8_t result = 0;
	uint8_t numBytes = 1;
	status_t returnError = IMU_SUCCESS;

	switch (commInterface) {

	case I2C_MODE:
		Wire.beginTransmission(I2CAddress);
		Wire.write(offset);
		if( Wire.endTransmission() != 0 )
		{
			returnError = IMU_HW_ERROR;
		}
		Wire.requestFrom(I2CAddress, numBytes);
		while ( Wire.available() ) // slave may send less than requested
		{
			result = Wire.read(); // receive a byte as a proper uint8_t
		}
		break;

	case SPI_MODE:
		// take the chip select low to select the device:
		digitalWrite(chipSelectPin, LOW);
		// send the device the register you want to read:
		SPI.transfer(offset | 0x80);  //Ored with "read request" bit
		// send a value of 0 to read the first byte returned:
		result = SPI.transfer(0x00);
		// take the chip select high to de-select:
		digitalWrite(chipSelectPin, HIGH);
		
		if( result == 0xFF )
		{
			//we've recieved all ones, report
			returnError = IMU_ALL_ONES_WARNING;
		}
		break;

	default:
		break;
	}

	*outputPointer = result;
	return returnError;
}

//****************************************************************************//
//
//  readRegisterInt16
//
//  Parameters:
//    *outputPointer -- Pass &variable (base address of) to save read data to
//    offset -- register to read
//
//****************************************************************************//
status_t LSM6DS3Core::readRegisterInt16( int16_t* outputPointer, uint8_t offset )
{
	uint8_t myBuffer[2];
	status_t returnError = readRegisterRegion(myBuffer, offset, 2);  //Does memory transfer
	int16_t output = (int16_t)myBuffer[0] | int16_t(myBuffer[1] << 8);
	
	*outputPointer = output;
	return returnError;
}

//****************************************************************************//
//
//  writeRegister
//
//  Parameters:
//    offset -- register to write
//    dataToWrite -- 8 bit data to write to register
//
//****************************************************************************//
status_t LSM6DS3Core::writeRegister(uint8_t offset, uint8_t dataToWrite) {
	status_t returnError = IMU_SUCCESS;
	switch (commInterface) {
	case I2C_MODE:
		//Write the byte
		Wire.beginTransmission(I2CAddress);
		Wire.write(offset);
		Wire.write(dataToWrite);
		if( Wire.endTransmission() != 0 )
		{
			returnError = IMU_HW_ERROR;
		}
		break;

	case SPI_MODE:
		// take the chip select low to select the device:
    SPI.beginTransaction(mySpiSettings);
		digitalWrite(chipSelectPin, LOW);
		// send the device the register you want to read:
		SPI.transfer(offset);
		// send a value of 0 to read the first byte returned:
		SPI.transfer(dataToWrite);
		// decrement the number of bytes left to read:
		// take the chip select high to de-select:
		digitalWrite(chipSelectPin, HIGH);
		break;
		
		//No way to check error on this write (Except to read back but that's not reliable)

	default:
		break;
	}

	return returnError;
}

status_t LSM6DS3Core::embeddedPage( void )
{
	status_t returnError = writeRegister( LSM6DS3_ACC_GYRO_RAM_ACCESS, 0x80 );
	
	return returnError;
}

status_t LSM6DS3Core::basePage( void )
{
	status_t returnError = writeRegister( LSM6DS3_ACC_GYRO_RAM_ACCESS, 0x00 );
	
	return returnError;
}


//****************************************************************************//
//
//  Main user class -- wrapper for the core class + maths
//
//  Construct with same rules as the core ( uint8_t busType, uint8_t inputArg )
//
//****************************************************************************//
LSM6DS3::LSM6DS3( uint8_t busType, uint8_t inputArg ) : LSM6DS3Core( busType, inputArg )
{
	//Construct with these default settings

	settings.gyroEnabled = 1;  //Can be 0 or 1
	settings.gyroRange = 2000;   //Max deg/s.  Can be: 125, 245, 500, 1000, 2000
	settings.gyroSampleRate = 416;   //Hz.  Can be: 13, 26, 52, 104, 208, 416, 833, 1666
	settings.gyroBandWidth = 400;  //Hz.  Can be: 50, 100, 200, 400;
	settings.gyroFifoEnabled = 1;  //Set to include gyro in FIFO
	settings.gyroFifoDecimation = 1;  //set 1 for on /1

	settings.accelEnabled = 1;
	settings.accelODROff = 1;
	settings.accelRange = 16;      //Max G force readable.  Can be: 2, 4, 8, 16
	settings.accelSampleRate = 416;  //Hz.  Can be: 13, 26, 52, 104, 208, 416, 833, 1666, 3332, 6664, 13330
	settings.accelBandWidth = 100;  //Hz.  Can be: 50, 100, 200, 400;
	settings.accelFifoEnabled = 1;  //Set to include accelerometer in the FIFO
	settings.accelFifoDecimation = 1;  //set 1 for on /1

	settings.tempEnabled = 1;

	//Select interface mode
	settings.commMode = 1;  //Can be modes 1, 2 or 3

	//FIFO control data
	settings.fifoThreshold = 3000;  //Can be 0 to 4095 (16 bit bytes)
	settings.fifoSampleRate = 10;  //default 10Hz
	settings.fifoModeWord = 0;  //Default off

	allOnesCounter = 0;
	nonSuccessCounter = 0;

}

//****************************************************************************//
//
//  Configuration section
//
//  This uses the stored SensorSettings to start the IMU
//  Use statements such as "myIMU.settings.commInterface = SPI_MODE;" or
//  "myIMU.settings.accelEnabled = 1;" to configure before calling .begin();
//
//****************************************************************************//
status_t LSM6DS3::begin(SensorSettings* pSettingsYouWanted)
{
	//Check the settings structure values to determine how to setup the device
	uint8_t dataToWrite = 0;  //Temporary variable

	//Begin the inherited core.  This gets the physical wires connected
	status_t returnError = beginCore();
	
	// Copy the values from the user's settings into the output 'pSettingsYouWanted'
	// compare settings with 'pSettingsYouWanted' after 'begin' to see if anything changed
	if(pSettingsYouWanted != NULL){ 
		pSettingsYouWanted->gyroEnabled = settings.gyroEnabled;
		pSettingsYouWanted->gyroRange = settings.gyroRange;
		pSettingsYouWanted->gyroSampleRate = settings.gyroSampleRate;
		pSettingsYouWanted->gyroBandWidth = settings.gyroBandWidth;
		pSettingsYouWanted->gyroFifoEnabled = settings.gyroFifoEnabled;
		pSettingsYouWanted->gyroFifoDecimation = settings.gyroFifoDecimation;
		pSettingsYouWanted->accelEnabled = settings.accelEnabled;
		pSettingsYouWanted->accelODROff = settings.accelODROff;
		pSettingsYouWanted->accelRange = settings.accelRange;
		pSettingsYouWanted->accelSampleRate = settings.accelSampleRate;
		pSettingsYouWanted->accelBandWidth = settings.accelBandWidth;
		pSettingsYouWanted->accelFifoEnabled = settings.accelFifoEnabled;
		pSettingsYouWanted->accelFifoDecimation = settings.accelFifoDecimation;
		pSettingsYouWanted->tempEnabled = settings.tempEnabled;
		pSettingsYouWanted->commMode = settings.commMode;
		pSettingsYouWanted->fifoThreshold = settings.fifoThreshold;
		pSettingsYouWanted->fifoSampleRate = settings.fifoSampleRate;
		pSettingsYouWanted->fifoModeWord = settings.fifoModeWord;
	}

	//Setup the accelerometer******************************
	dataToWrite = 0; //Start Fresh!
	if ( settings.accelEnabled == 1) {
		//Build config reg
		//First patch in filter bandwidth
		switch (settings.accelBandWidth) {
		case 50:
			dataToWrite |= LSM6DS3_ACC_GYRO_BW_XL_50Hz;
			break;
		case 100:
			dataToWrite |= LSM6DS3_ACC_GYRO_BW_XL_100Hz;
			break;
		case 200:
			dataToWrite |= LSM6DS3_ACC_GYRO_BW_XL_200Hz;
			break;
		default:  //set default case to max passthrough
			settings.accelBandWidth = 400;
		case 400:
			dataToWrite |= LSM6DS3_ACC_GYRO_BW_XL_400Hz;
			break;
		}
		//Next, patch in full scale
		switch (settings.accelRange) {
		case 2:
			dataToWrite |= LSM6DS3_ACC_GYRO_FS_XL_2g;
			break;
		case 4:
			dataToWrite |= LSM6DS3_ACC_GYRO_FS_XL_4g;
			break;
		case 8:
			dataToWrite |= LSM6DS3_ACC_GYRO_FS_XL_8g;
			break;
		default:  //set default case to 16(max)
			settings.accelRange = 16;
		case 16:
			dataToWrite |= LSM6DS3_ACC_GYRO_FS_XL_16g;
			break;
		}
		//Lastly, patch in accelerometer ODR
		switch (settings.accelSampleRate) {
		case 13:
			dataToWrite |= LSM6DS3_ACC_GYRO_ODR_XL_13Hz;
			break;
		case 26:
			dataToWrite |= LSM6DS3_ACC_GYRO_ODR_XL_26Hz;
			break;
		case 52:
			dataToWrite |= LSM6DS3_ACC_GYRO_ODR_XL_52Hz;
			break;
		default:  //Set default to 104
			settings.accelSampleRate = 104;
		case 104:
			dataToWrite |= LSM6DS3_ACC_GYRO_ODR_XL_104Hz;
			break;
		case 208:
			dataToWrite |= LSM6DS3_ACC_GYRO_ODR_XL_208Hz;
			break;
		case 416:
			dataToWrite |= LSM6DS3_ACC_GYRO_ODR_XL_416Hz;
			break;
		case 833:
			dataToWrite |= LSM6DS3_ACC_GYRO_ODR_XL_833Hz;
			break;
		case 1660:
			dataToWrite |= LSM6DS3_ACC_GYRO_ODR_XL_1660Hz;
			break;
		case 3330:
			dataToWrite |= LSM6DS3_ACC_GYRO_ODR_XL_3330Hz;
			break;
		case 6660:
			dataToWrite |= LSM6DS3_ACC_GYRO_ODR_XL_6660Hz;
			break;
		case 13330:
			dataToWrite |= LSM6DS3_ACC_GYRO_ODR_XL_13330Hz;
			break;
		}
	}
	else
	{
		//dataToWrite already = 0 (powerdown);
	}

	//Now, write the patched together data
	writeRegister(LSM6DS3_ACC_GYRO_CTRL1_XL, dataToWrite);

	//Set the ODR bit
	readRegister(&dataToWrite, LSM6DS3_ACC_GYRO_CTRL4_C);
	dataToWrite &= ~((uint8_t)LSM6DS3_ACC_GYRO_BW_SCAL_ODR_ENABLED);
	if ( settings.accelODROff == 1) {
		dataToWrite |= LSM6DS3_ACC_GYRO_BW_SCAL_ODR_ENABLED;
	}
	writeRegister(LSM6DS3_ACC_GYRO_CTRL4_C, dataToWrite);

	//Setup the gyroscope**********************************************
	dataToWrite = 0; //Start Fresh!
	if ( settings.gyroEnabled == 1) {
		//Build config reg
		//First, patch in full scale
		switch (settings.gyroRange) {
		case 125:
			dataToWrite |= LSM6DS3_ACC_GYRO_FS_125_ENABLED;
			break;
		case 245:
			dataToWrite |= LSM6DS3_ACC_GYRO_FS_G_245dps;
			break;
		case 500:
			dataToWrite |= LSM6DS3_ACC_GYRO_FS_G_500dps;
			break;
		case 1000:
			dataToWrite |= LSM6DS3_ACC_GYRO_FS_G_1000dps;
			break;
		default:  //Default to full 2000DPS range
			settings.gyroRange = 2000;
		case 2000:
			dataToWrite |= LSM6DS3_ACC_GYRO_FS_G_2000dps;
			break;
		}
		//Lastly, patch in gyro ODR
		switch (settings.gyroSampleRate) {
		case 13:
			dataToWrite |= LSM6DS3_ACC_GYRO_ODR_G_13Hz;
			break;
		case 26:
			dataToWrite |= LSM6DS3_ACC_GYRO_ODR_G_26Hz;
			break;
		case 52:
			dataToWrite |= LSM6DS3_ACC_GYRO_ODR_G_52Hz;
			break;
		default:  //Set default to 104
			settings.gyroSampleRate = 104;
		case 104:
			dataToWrite |= LSM6DS3_ACC_GYRO_ODR_G_104Hz;
			break;
		case 208:
			dataToWrite |= LSM6DS3_ACC_GYRO_ODR_G_208Hz;
			break;
		case 416:
			dataToWrite |= LSM6DS3_ACC_GYRO_ODR_G_416Hz;
			break;
		case 833:
			dataToWrite |= LSM6DS3_ACC_GYRO_ODR_G_833Hz;
			break;
		case 1660:
			dataToWrite |= LSM6DS3_ACC_GYRO_ODR_G_1660Hz;
			break;
		}
	}
	else
	{
		//dataToWrite already = 0 (powerdown);
	}
	//Write the byte
	writeRegister(LSM6DS3_ACC_GYRO_CTRL2_G, dataToWrite);

	//Setup the internal temperature sensor
	if ( settings.tempEnabled == 1) {
	}

	//Return WHO AM I reg  //Not no mo!
	uint8_t result;
	readRegister(&result, LSM6DS3_ACC_GYRO_WHO_AM_I_REG);

	return returnError;
}

//****************************************************************************//
//
//  Accelerometer section
//
//****************************************************************************//
int16_t LSM6DS3::readRawAccelX( void )
{
	int16_t output;
	status_t errorLevel = readRegisterInt16( &output, LSM6DS3_ACC_GYRO_OUTX_L_XL );
	if( errorLevel != IMU_SUCCESS )
	{
		if( errorLevel == IMU_ALL_ONES_WARNING )
		{
			allOnesCounter++;
		}
		else
		{
			nonSuccessCounter++;
		}
	}
	return output;
}
float LSM6DS3::readFloatAccelX( void )
{
	float output = calcAccel(readRawAccelX());
	return output;
}

int16_t LSM6DS3::readRawAccelY( void )
{
	int16_t output;
	status_t errorLevel = readRegisterInt16( &output, LSM6DS3_ACC_GYRO_OUTY_L_XL );
	if( errorLevel != IMU_SUCCESS )
	{
		if( errorLevel == IMU_ALL_ONES_WARNING )
		{
			allOnesCounter++;
		}
		else
		{
			nonSuccessCounter++;
		}
	}
	return output;
}
float LSM6DS3::readFloatAccelY( void )
{
	float output = calcAccel(readRawAccelY());
	return output;
}

int16_t LSM6DS3::readRawAccelZ( void )
{
	int16_t output;
	status_t errorLevel = readRegisterInt16( &output, LSM6DS3_ACC_GYRO_OUTZ_L_XL );
	if( errorLevel != IMU_SUCCESS )
	{
		if( errorLevel == IMU_ALL_ONES_WARNING )
		{
			allOnesCounter++;
		}
		else
		{
			nonSuccessCounter++;
		}
	}
	return output;
}
float LSM6DS3::readFloatAccelZ( void )
{
	float output = calcAccel(readRawAccelZ());
	return output;
}

float LSM6DS3::calcAccel( int16_t input )
{
	float output = (float)input * 0.061 * (settings.accelRange >> 1) / 1000;
	return output;
}

//****************************************************************************//
//
//  Gyroscope section
//
//****************************************************************************//
int16_t LSM6DS3::readRawGyroX( void )
{
	int16_t output;
	status_t errorLevel = readRegisterInt16( &output, LSM6DS3_ACC_GYRO_OUTX_L_G );
	if( errorLevel != IMU_SUCCESS )
	{
		if( errorLevel == IMU_ALL_ONES_WARNING )
		{
			allOnesCounter++;
		}
		else
		{
			nonSuccessCounter++;
		}
	}
	return output;
}
float LSM6DS3::readFloatGyroX( void )
{
	float output = calcGyro(readRawGyroX());
	return output;
}

int16_t LSM6DS3::readRawGyroY( void )
{
	int16_t output;
	status_t errorLevel = readRegisterInt16( &output, LSM6DS3_ACC_GYRO_OUTY_L_G );
	if( errorLevel != IMU_SUCCESS )
	{
		if( errorLevel == IMU_ALL_ONES_WARNING )
		{
			allOnesCounter++;
		}
		else
		{
			nonSuccessCounter++;
		}
	}
	return output;
}
float LSM6DS3::readFloatGyroY( void )
{
	float output = calcGyro(readRawGyroY());
	return output;
}

int16_t LSM6DS3::readRawGyroZ( void )
{
	int16_t output;
	status_t errorLevel = readRegisterInt16( &output, LSM6DS3_ACC_GYRO_OUTZ_L_G );
	if( errorLevel != IMU_SUCCESS )
	{
		if( errorLevel == IMU_ALL_ONES_WARNING )
		{
			allOnesCounter++;
		}
		else
		{
			nonSuccessCounter++;
		}
	}
	return output;
}
float LSM6DS3::readFloatGyroZ( void )
{
	float output = calcGyro(readRawGyroZ());
	return output;
}

float LSM6DS3::calcGyro( int16_t input )
{
	uint8_t gyroRangeDivisor = settings.gyroRange / 125;
	if ( settings.gyroRange == 245 ) {
		gyroRangeDivisor = 2;
	}

	float output = (float)input * 4.375 * (gyroRangeDivisor) / 1000;
	return output;
}

//****************************************************************************//
//
//  Temperature section
//
//****************************************************************************//
int16_t LSM6DS3::readRawTemp( void )
{
	int16_t output;
	readRegisterInt16( &output, LSM6DS3_ACC_GYRO_OUT_TEMP_L );
	return output;
}  

float LSM6DS3::readTempC( void )
{
	float output = (float)readRawTemp() / 16; //divide by 16 to scale
	output += 25; //Add 25 degrees to remove offset

	return output;

}

float LSM6DS3::readTempF( void )
{
	float output = (float)readRawTemp() / 16; //divide by 16 to scale
	output += 25; //Add 25 degrees to remove offset
	output = (output * 9) / 5 + 32;

	return output;

}

//****************************************************************************//
//
//  FIFO section
//
//****************************************************************************//
void LSM6DS3::fifoBegin( void ) {
	//CONFIGURE THE VARIOUS FIFO SETTINGS
	//
	//
	//This section first builds a bunch of config words, then goes through
	//and writes them all.

	//Split and mask the threshold
	uint8_t thresholdLByte = settings.fifoThreshold & 0x00FF;
	uint8_t thresholdHByte = (settings.fifoThreshold & 0x0F00) >> 8;
	//Pedo bits not configured (ctl2)

	//CONFIGURE FIFO_CTRL3
	uint8_t tempFIFO_CTRL3 = 0;
	if (settings.gyroFifoEnabled == 1)
	{
		//Set up gyro stuff
		//Build on FIFO_CTRL3
		//Set decimation
		tempFIFO_CTRL3 |= (settings.gyroFifoDecimation & 0x07) << 3;

	}
	if (settings.accelFifoEnabled == 1)
	{
		//Set up accelerometer stuff
		//Build on FIFO_CTRL3
		//Set decimation
		tempFIFO_CTRL3 |= (settings.accelFifoDecimation & 0x07);
	}

	//CONFIGURE FIFO_CTRL4  (nothing for now-- sets data sets 3 and 4
	uint8_t tempFIFO_CTRL4 = 0;


	//CONFIGURE FIFO_CTRL5
	uint8_t tempFIFO_CTRL5 = 0;
	switch (settings.fifoSampleRate) {
	default:  //set default case to 10Hz(slowest)
	case 10:
		tempFIFO_CTRL5 |= LSM6DS3_ACC_GYRO_ODR_FIFO_10Hz;
		break;
	case 25:
		tempFIFO_CTRL5 |= LSM6DS3_ACC_GYRO_ODR_FIFO_25Hz;
		break;
	case 50:
		tempFIFO_CTRL5 |= LSM6DS3_ACC_GYRO_ODR_FIFO_50Hz;
		break;
	case 100:
		tempFIFO_CTRL5 |= LSM6DS3_ACC_GYRO_ODR_FIFO_100Hz;
		break;
	case 200:
		tempFIFO_CTRL5 |= LSM6DS3_ACC_GYRO_ODR_FIFO_200Hz;
		break;
	case 400:
		tempFIFO_CTRL5 |= LSM6DS3_ACC_GYRO_ODR_FIFO_400Hz;
		break;
	case 800:
		tempFIFO_CTRL5 |= LSM6DS3_ACC_GYRO_ODR_FIFO_800Hz;
		break;
	case 1600:
		tempFIFO_CTRL5 |= LSM6DS3_ACC_GYRO_ODR_FIFO_1600Hz;
		break;
	case 3300:
		tempFIFO_CTRL5 |= LSM6DS3_ACC_GYRO_ODR_FIFO_3300Hz;
		break;
	case 6600:
		tempFIFO_CTRL5 |= LSM6DS3_ACC_GYRO_ODR_FIFO_6600Hz;
		break;
	}
	//Hard code the fifo mode here:
	tempFIFO_CTRL5 |= settings.fifoModeWord = 6;  //set mode:

	//Write the data
	writeRegister(LSM6DS3_ACC_GYRO_FIFO_CTRL1, thresholdLByte);
	//Serial.println(thresholdLByte, HEX);
	writeRegister(LSM6DS3_ACC_GYRO_FIFO_CTRL2, thresholdHByte);
	//Serial.println(thresholdHByte, HEX);
	writeRegister(LSM6DS3_ACC_GYRO_FIFO_CTRL3, tempFIFO_CTRL3);
	writeRegister(LSM6DS3_ACC_GYRO_FIFO_CTRL4, tempFIFO_CTRL4);
	writeRegister(LSM6DS3_ACC_GYRO_FIFO_CTRL5, tempFIFO_CTRL5);

}
void LSM6DS3::fifoClear( void ) {
	//Drain the fifo data and dump it
	while( (fifoGetStatus() & 0x1000 ) == 0 ) {
		fifoRead();
	}

}
int16_t LSM6DS3::fifoRead( void ) {
	//Pull the last data from the fifo
	uint8_t tempReadByte = 0;
	uint16_t tempAccumulator = 0;
	readRegister(&tempReadByte, LSM6DS3_ACC_GYRO_FIFO_DATA_OUT_L);
	tempAccumulator = tempReadByte;
	readRegister(&tempReadByte, LSM6DS3_ACC_GYRO_FIFO_DATA_OUT_H);
	tempAccumulator |= ((uint16_t)tempReadByte << 8);

	return tempAccumulator;
}

uint16_t LSM6DS3::fifoGetStatus( void ) {
	//Return some data on the state of the fifo
	uint8_t tempReadByte = 0;
	uint16_t tempAccumulator = 0;
	readRegister(&tempReadByte, LSM6DS3_ACC_GYRO_FIFO_STATUS1);
	tempAccumulator = tempReadByte;
	readRegister(&tempReadByte, LSM6DS3_ACC_GYRO_FIFO_STATUS2);
	tempAccumulator |= (tempReadByte << 8);

	return tempAccumulator;  

}
void LSM6DS3::fifoEnd( void ) {
	// turn off the fifo
	writeRegister(LSM6DS3_ACC_GYRO_FIFO_CTRL5, 0x00);  //Disable
}


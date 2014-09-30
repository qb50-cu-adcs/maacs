/* ***********************************************************************
 * file: 	   ACD7991.c
 * author: 	   Kyle Litzer
 * created on: 9/20/2014
 *
 * This file contains routines to interface with the AD7991 Analog
 * Devices analog to digital converter (ADC) used with the OSRAM (SFH2430) 
 * sun sensors. 
 * The i2c driver is utilized here to perform many of the low level i2c
 * operations required to communicate and draw data from the sensors. Each
 * of the functions contained within this driver are documented and
 * described below as they are declared.
 * ***********************************************************************
 */
 
/* Include Files */
#include "types.h" // not sure what this is for yet
#include "i2c.h" // I2C driver for low level operations

/* Include Files for Debugging */
#include "dbg.h" // For debug routines
#include "io.h" /* Standard C library */ 
#include "AD7991.h"

/* Variable Definitions & Initializations */
//char bitsNumber      = 0;
char i2cSlaveAddress = 0;

/* Initialize I2C peripheral
 *
 * 		@param deviceVersion should be one of two addresses in header
 *			0 - refers to AD7991-0
 *			1 - refers to AD7991-1 
 *
 * 		@param sclFreq should be one of two capable frequencies in header
 *			0 - refers to 100kHz
 *			1 - refers to 400kHz
 */
void AD7991_Init(char deviceVersion, char sclFreq, struct i2c *ad7991_i2c) 
{
        unsigned int scl_freq = 0;

	/* Ternary statement to determine which of the two frequencies to use */
	scl_freq = (sclFreq) ? 400000 : 100000;
	
	/* Initialize passed I2C port to the frequency specified */
	i2c_init(ad7991_i2c,scl_freq);
	
	/* Ternary statement to determine which built in address */
	i2cSlaveAddress = (deviceVersion) ? AD7991_1_ADDRESS : AD7991_0_ADDRESS;
	
	/* Set AD7991 Slave Address */
	i2c_slave(ad7991_i2c,i2cSlaveAddress,ADDR_7BIT);
}

/* Write Data into the Configuration Register
 *
 *		@param registerValue - Configuration Register Structure is defined 
 *								under "Internal Register Structure" on page
 *								20 of the AD7991 data sheet
 *
 * http://www.analog.com/static/imported-files/data_sheets/AD7991_7995_7999.pdf
 */
void AD7991_SetConfigReg(char registerValue,char deviceVersion,struct i2c *ad7991_i2c)
{
        //char *data = &registerValue;
        
	/* Ternary statement to determine which built in address */
	i2cSlaveAddress = (deviceVersion) ? AD7991_1_ADDRESS : AD7991_0_ADDRESS;
	i2c_slave(ad7991_i2c,i2cSlaveAddress,ADDR_7BIT);
	i2c_tx(ad7991_i2c,&registerValue,AD7991_REGISTERLENGTH); 
	i2c_stop(ad7991_i2c);
}

/* Reads the High byte and the Low byte of the conversion
 *
 *		@param convValue - It is used to store the conversion value. 
 *		@param channel - Stores the channel number for the current conversion.
 */
void AD7991_Read(short* convValue, char* channel,char deviceVersion,struct i2c *ad7991_i2c)
{
	/* Variable Definitions & Initializations */
	//char rxData[2] = {0, 0};
        char rxData = 0;
	//short		  convWord = 0;

	/* Ternary statement to determine which built in address */
	i2cSlaveAddress = (deviceVersion) ? AD7991_1_ADDRESS : AD7991_0_ADDRESS;
	
	/* Read data from AD7991 register and write it to rxData */
	i2c_slave(ad7991_i2c,i2cSlaveAddress,ADDR_7BIT); // need to reslave before rx or tx
	i2c_rx(ad7991_i2c,&rxData,2);
	//convWord = (rxData[0] << 8) + rxData[1];
	//*channel = (convWord & 0x3000) >> 12;
	//*conValue = (convWord & 0x0FFF) >> (12 - AD7991_BITSNUMBER);
}

/* Convert Raw Data sample (rxData?) to volts
 *
 * 		@param rawSample - The data sample.
 * 		@param vRef - The value of the voltage reference used by the device.
 *
 * 		@return voltage - The result of the conversion expressed as volts.
 */
float AD7991_ConvToVolts(short rawSample, float vRef)
{
	/* Variable Definitions & Initializations */
	float voltage = 0;
	
	voltage = vRef * (float)rawSample / (1 << AD7991_BITSNUMBER);
	
	return voltage;
}

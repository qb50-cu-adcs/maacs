/* file: 		magnetometer.c
 * author: 	Vishnuu Mallik and Jacob Cook
 * created on: 09/21/2014
 
 USE THIS !!!
 */

#include "dbg.h"
#include "types.h"
#include "io.h"
#include "i2c.h"
#include "magnetometer.h"

#define MAG_ADDRESS1 (0x01) /* SOME NUMBER !!! */
#define X_LSB_ADDRESS (0x28) /* Address of the Least significant bit */

/* -- mag_init : INITIALIZATION ROUTINE FOR INSIDE MAGNETOMETER --------
 * 
 * ---------------------------------------------------------------------*/
void mag_init(struct magInfo mag)   
{
	char cmd[2];

	/* Set mangetometer slave address */
	i2c_slave(mag.i2c_port, mag.slave_addr, ADDR_7BIT);
    
	/* Set control register 1 */
	cmd[0] = 0x20;
	cmd[1] = 0xF0;
 	i2c_tx(mag.i2c_port,cmd,2);
	i2c_stop(mag.i2c_port);
    
	/* Set control register 2 */
  cmd[0] = 0x21;
  cmd[1] = 0x00; /* Full scale selection of _+4 guass */
  i2c_tx(mag.i2c_port,cmd,2);
  i2c_stop(mag.i2c_port);
    
	/* Set control register 3 */
	cmd[0] = 0x22;
	cmd[1] = 0x00; /* Continous conversion mode */
	i2c_tx(mag.i2c_port,cmd,2);
	i2c_stop(mag.i2c_port);
	
	/* Set control register 4 */
	cmd[0] = 0x23;
	cmd[1] = 0x0C; 
	i2c_tx(mag.i2c_port,cmd,2);
	i2c_stop(mag.i2c_port);
}

/* -- READING DATA -------------------------------------------------------
 * 
 * ---------------------------------------------------------------------*/
void mag_read(struct magInfo mag)
{
    char mag_data[6];
    char start_address = X_LSB_ADDRESS;
    
    i2c_slave(mag.i2c_port, mag.slave_addr, ADDR_7BIT);
    
    /*Start data transmission */
    i2c_tx(mag.i2c_port, &start_address, 1);  /* selects X LSB register */
    
    /* one time fill array */
    i2c_rx(mag.i2c_port, mag_data, 6);
    i2c_stop(mag.i2c_port);
    
    mag.X = (mag_data[1] << 8) | mag_data[0];
    mag.Y = (mag_data[3] << 8) | mag_data[2];
    mag.Z = (mag_data[5] << 8) | mag_data[4];

    /* Apply scale factor here */
    mag.X = mag.X*0.01462;
    mag.Y = mag.Y*0.01462;
    mag.Z = mag.Z*0.1462;
    
    PRINT("X = %u\n\r",mag.X);
    PRINT("Y = %u\n\r",mag.Y);
    PRINT("Z = %u\n\r",mag.Z);
}

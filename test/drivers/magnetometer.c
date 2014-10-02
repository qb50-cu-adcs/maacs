/* file: 		magnetometer.c
 * author: 	jacob cook
 * created on: 7/11/2014
 */

#include "dbg.h"
#include "types.h"
#include "io.h"
#include "i2c.h"

#define MAG_ADDRESS1 (0x01) /* SOME NUMBER !!! */

struct i2c *mag_i2c =(struct i2c *)I2C1_BASE;

/* -- mag_init -----------------------------------------------------------
 * 
 * ---------------------------------------------------------------------*/
void mag_init(unsigned int address)
{
	char cmd[2];

	/* Set mangetometer slave address */
	i2c_slave(mag_i2c,address,ADDR_7BIT);
	
	/* Set control register 1 */
	cmd[0] = 0x20;
	cmd[1] = 0xF0;	
	i2c_tx(mag_i2c,cmd,2);
	i2c_stop(mag_i2c);

	/* Set control register 2 */


}


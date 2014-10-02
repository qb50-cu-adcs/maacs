/* file: test.c
 * author: Jacob Cook
 * created on: 5/21/2014
 *
 */

#include "dbg.h"
#include "io.h"
#include "types.h"
#include "clocks.h"
#include "uart.h"
#include "control_module.h"
#include "i2c.h"
#include "sd_card.h"
#include "mmcsd.h"
#include "timer.h"
#include "pwm.h"
#include "util.h"
#include "test.h"

/*************************************************************************
**             ADCS MAIN LOOP 
*************************************************************************/

struct i2c *i2c0 = (struct i2c *)I2C0_BASE;
struct i2c *i2c1 = (struct i2c *)I2C1_BASE;
struct i2c *i2c2 = (struct i2c *)I2C2_BASE;

int main()
{
	/* Configuring the system clocks */
	INIT_DBG_PORT();
	pll_init();
	util_setup();

	/* Perform necessary pin muxing */
	enable_mmc0_pin_mux();
	enable_i2c0_pin_mux();	
	enable_i2c1_pin_mux();
	enable_i2c2_pin_mux();
	enable_tmr4_pin_mux();
	enable_tmr5_pin_mux();
	enable_tmr7_pin_mux();

	/* Run Testing Menu */
	test_main_menu();
	return 1;
}

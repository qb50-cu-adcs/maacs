/* ***********************************************************************
 * file: 		i2c.c
 * author: 	jacob cook
 * created on: 6/3/2014
 *
 * This file contains lowlevel routines to interface with the I2C module. 
 * The i2c module uses a polling method for transmitting and recieving 
 * data to and from the slave device. The module assumes that all i2c
 * port will be operating in master mode.
 * ***********************************************************************
 */

#include "io.h"
#include "i2c.h"
#include "types.h"

/* For Debugging */
#include "dbg.h" 

/* CLOCK CONFIGURATION PARAMETER
 * Prescale Sampling Clock Divider Value 
 * here we want a 12 Mhz clock for the I2C clock.  We get a 48 Mhz clock
 * from the Periphereal PLL output (see figure 8-11 in TRM) so it needs
 * to be divided down according to SYS_CLK/(PSC_DIV+1) = 12MHz 
 */
#define SCLK (48000000) /* hz (PPL OUTPUT CLOCK? */
#define ICLK (12000000) /* hz (I2C CLOCK?) */
#define PSC_DIV (SCLK/ICLK - 1) /* GIVES US A VALUE OF 3? */

/* Configuration Bits */
#define I2C_EN (0x1 << 15)
#define MST		 (0x1 << 10)
#define TRX		 (0x1 << 9)
#define XSA 	 (0x1 << 8)
#define STP		 (0x1 << 1)
#define STT		 (0x1)

/* SYSC bits */
#define SYSC_AUTOIDLE (0x1)
#define SYSC_SRST			(0x1 << 1)
#define SYSC_IDLEMODE	(0x3 << 3)
#define SYSC_NOIDLE		(0x1 << 3)

/* Raw Interrupt Status Bits */
#define BB 		(0x1 << 12)
#define XRDY 	(0x1 << 4)
#define RRDY	(0x1 << 3)
#define ARDY	(0x1 << 2)
#define NACK	(0x1 << 1)


/* -- i2c_init -----------------------------------------------------------
 * This function initializes the passed i2c port to the frequency 
 * specified by the scl_freq parameter. The port can be configured to 
 * to operate at frequencies of either 100khz or 400khz. All ports are 
 * configured to operate as the i2c master. At the end of the routine the 
 * port is enabled.
 * ---------------------------------------------------------------------*/
void i2c_init(struct i2c *i2c_port, unsigned int scl_freq)
{
	int divider;

	/* Disable port to set up clocks */
	i2c_disable(i2c_port);
	i2c_reset(i2c_port);

	/* Disable Auto Idle Setting */
	and_regl(~SYSC_AUTOIDLE, &i2c_port->sysc);
	
	/* Set PSC value to obtain 12Mhz clock */
	writel(PSC_DIV, &i2c_port->psc);
		
	/* set the SCLL and SCLH registers to obtain the desired output freq */
	divider = ICLK/scl_freq/2;
	writel(divider-7, &i2c_port->scll);
	writel(divider-5, &i2c_port->sclh);
		
	/* enable the port */
	i2c_enable(i2c_port);

	/* verify that reset is complete and clocks are running */
	while(!readl(&i2c_port->syss));
	
	/* set port to master mode */
	or_regl(MST, &i2c_port->con);	
	
	/* manually set ARDY after reset */
	or_regl(ARDY, i2c_port->irqstatus_raw);
}

/* -- i2c_reset ----------------------------------------------------------
 *
 * ---------------------------------------------------------------------*/
void i2c_reset(struct i2c *i2c_port)
{
	or_regl(SYSC_SRST, &i2c_port->sysc);
}


/* -- i2c_enable ---------------------------------------------------------
 * This function enables the passed i2c port by setting the enable bit 
 * in the configuration register.
 * ---------------------------------------------------------------------*/
void i2c_enable(struct i2c *i2c_port)
{
	or_regl(I2C_EN, &i2c_port->con);
}

/* -- i2c_diSable ---------------------------------------------------------
 * This function disables the passed i2c port by clearing the enable bit
 * in the configuration register.
 * ---------------------------------------------------------------------*/
void i2c_disable(struct i2c *i2c_port)
{
	and_regl(~I2C_EN, &i2c_port->con);
}	

/* -- i2c_slave ----------------------------------------------------------
 * This function sets the slave address length and address.  The length
 * can be configured to be either 10 or 7 bits in length. This is 
 * specified through the extend slave address (xsa) parameter.
 * ---------------------------------------------------------------------*/
void i2c_slave(struct i2c *i2c_port, int slave_addr, int xsa)
{
	unsigned int i2c_con;
	i2c_con = readl(&i2c_port->con);

	if(xsa)
		i2c_con |= XSA;
	else 
		i2c_con &= ~XSA;

	writel(i2c_con, &i2c_port->con);
	writel(slave_addr, &i2c_port->sa);
}

/* -- i2c_start ----------------------------------------------------------
 * This function sets the start bit in the configuration register in 
 * order to initiate communication between the Master and slave.
 * ---------------------------------------------------------------------*/
void i2c_start(struct i2c *i2c_port)
{
	or_regl(STT, &i2c_port->con);
}

/* -- i2c_stop -----------------------------------------------------------
 * This function set the stop bit in the configuration register of the 
 * passed i2c port. This will impart a stop condition when the cnt 
 * register reads 0.
 * ---------------------------------------------------------------------*/
void i2c_stop(struct i2c *i2c_port)
{
	or_regl(STP, &i2c_port->con);
}

/* -- i2c_busy -----------------------------------------------------------
 * This function checks to see if the passed i2c port is busy.
 * ---------------------------------------------------------------------*/
int i2c_busy(struct i2c *i2c_port)
{
	return (int)(readl(&i2c_port->irqstatus_raw) & BB);
}	

/* -- i2c_clear_flags ----------------------------------------------------
 * This function clears all the interrupt flags associated with the i2c
 * port.
 * ---------------------------------------------------------------------*/
void i2c_clear_flags(struct i2c *i2c_port)
{
	writel(0x7FFF, &i2c_port->irqenable_set);
	writel(0x7FFF, &i2c_port->irqstatus);
	writel(0x7FFF, &i2c_port->irqenable_clr);
}

/* -- i2c_tx -------------------------------------------------------------
 * This function transmits the passed data on the specified i2c port.
 * ---------------------------------------------------------------------*/
void i2c_tx(struct i2c *i2c_port, char *data, int len)
{
	int i;	


	/* wait for i2c registers to be ready to access */
	/*while(!(readl(&i2c_port->irqstatus_raw) & ARDY))*/
		/*;	*/
	
	/* clear out all status flags */
	i2c_clear_flags(i2c_port);
	
	/* set transmit bit in configuration register */
	or_regl(TRX, &i2c_port->con);

	/* load transfer count */
	writel(len, &i2c_port->cnt);
	
	/* generate start condition */
	i2c_start(i2c_port);
	
	/* wait for bus to fire up TODO: check for ACK*/
	while(!i2c_busy(i2c_port))
		;

	/* use the polling method to transfer a byte at a time */
	for(i=len; i!=0; i--)
	{
		/* wait until previous transfer is complete */
		while(!(readl(&i2c_port->irqstatus_raw) & XRDY)); 

		/* load next byte into data register */
		writel(*data++, &i2c_port->data);
		
		/* clear transmit flag */
		writel(XRDY, &i2c_port->irqstatus);
	}
	
}

/* -- i2c_rx -------------------------------------------------------------
 * This function reads the specified amount of data on the specified 
 * i2c port.
 * ---------------------------------------------------------------------*/
void i2c_rx(struct i2c *i2c_port, char *data, int len)
{
	int i;	

	/* wait for i2c registers to be ready to access */
	/*while(!(readl(&i2c_port->irqstatus_raw) & ARDY))*/
		/*;	*/
	
	/* load transfer count */
	writel(len, &i2c_port->cnt);
	
	/* clear out all status flags */
	i2c_clear_flags(i2c_port);
	
	/* clear transmit bit in configuration register */
	and_regl(~TRX, &i2c_port->con);

	/* generate start condition or restart if already started */
	i2c_start(i2c_port);

	/* wait for bus to fire up TODO: check for ACK*/
	while(!i2c_busy(i2c_port))
		;	

	/* use the polling method to retrieve a byte at a time */
	for(i=len; i!=0; i--)
	{
		/* wait until reception is complete */
		while(!(readl(&i2c_port->irqstatus_raw) & RRDY));
		
		/* load next byte into data register */
		*data++ = (char)readl(&i2c_port->data);

		/* clear flag */
		writel(RRDY, &i2c_port->irqstatus);
	}
}

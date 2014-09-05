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

/*************************************************************************
**              FUNCTION DEFINITIONS
*************************************************************************/

static char test_str[] = {0x00, 0x0};
char test_data[50];
struct i2c *i2c0 = (struct i2c *)I2C0_BASE;

struct mmcsdInfo mysdcard;
struct mmcsdCtlr *mmcsd0 = (struct mmcsdCtlr *)MMC0_BASE;
struct cardInfo sdCard;
static char sdBlk[1024];

struct tmrCtlr *tmr4 = (struct tmrCtlr *)TIMER4_BASE;
struct pwmInfo pwm4;
struct tmrCtlr *tmr5 = (struct tmrCtlr *)TIMER5_BASE;
struct pwmInfo pwm5;
struct tmrCtlr *tmr7 = (struct tmrCtlr *)TIMER7_BASE;
struct pwmInfo pwm7;

int main()
{
		char c;
		char str[50];	
		int i, j, retval;

    /* Configuring the system clocks */
		pll_init();
		util_setup();

    /* Perform necessary pin muxing */
		enable_mmc0_pin_mux();
		enable_i2c0_pin_mux();	
		enable_tmr4_pin_mux();
		enable_tmr5_pin_mux();
		enable_tmr7_pin_mux();

		/* Initialize debug port */
		INIT_DBG_PORT();

		PRINT("this is %u\n\r", 5);

		pwm4.period = 1;
		pwm4.duty_cycle = 50;
		pwm4.polarity = PWM_POL_POS;
		pwm4.ctlr = tmr4;

		pwm5.period = 1;
		pwm5.duty_cycle = 0;
		pwm5.polarity = PWM_POL_POS;
		pwm5.ctlr = tmr5;

		pwm7.period = 1;
		pwm7.duty_cycle = 100;
		pwm7.polarity = PWM_POL_POS;
		pwm7.ctlr = tmr7;

		pwm_setup(&pwm4);
		pwm_setup(&pwm5);
		pwm_setup(&pwm7);

		pwm_start(&pwm4);
		pwm_start(&pwm5);
		pwm_start(&pwm7);

		/*pwm_set_duty_cycle(&pwm4, 50);*/
		/*PRINT("PWM init complete\n\r");*/

		while(1)
		{
			stopwatch_start();
			retval = stopwatch_stop();	
			/*PRINT("time = %u ms\n\r", retval);*/
		}

		/*test_main_menu();*/
	
		mysdcard.ctlr = mmcsd0;
		mysdcard.card = &sdCard;

		/* *************************sd card testing **************************/
		if(!mmcsd_init(mysdcard.ctlr))
		{
			
			PRINT("MMC0 software init successful\n\r");
		}	

		mysdcard.buswidth = SD_BUSWIDTH_4BIT;
		if(sdcard_init(&mysdcard))
		{
			PRINT("SD Card init successful\n\r");
		}

		sdcard_print_info(&sdCard);
		PRINT("\n\rSD Card working conditions\n\r");
		PRINT("  Bus Width = %u bit\n\r", mysdcard.buswidth);
		PRINT("  Bus Freq  = %u hz\n\r", mysdcard.busfreq);

		PRINT("\n\r");

		while(1)
		{
			/*uart0_getc();*/
			PRINT("reading from SD card\n\r");
			if(sdcard_read(&mysdcard, sdBlk, 5000000, 1) == 0)
			{
				PRINT("SD card read was unsuccessful\n\r");
			}

			PRINT("RD = ");
			for(i=0;i<10;i++)
			{
				PRINT("0x%X, ",sdBlk[i]);
			}
			PRINT("\n\r");

			for(j=0;j<10;j++)
			{	
				if(sdBlk[j] == 0xAA)
					sdBlk[j] = 0x55;
				else 
					sdBlk[j] = 0xAA;
			}
			c = sdBlk[0];
			

			PRINT("WR = ");
			for(i=0;i<10;i++)
			{
				PRINT("0x%X, ",sdBlk[i]);
			}
			PRINT("\n\r");

			if(sdcard_write(&mysdcard, sdBlk, 5000000, 1) == 0)
			{
				PRINT("SD card write was unsuccessful\n\r");
			}

			if(sdcard_read(&mysdcard, sdBlk, 5000000, 1) == 0)
			{
				PRINT("SD card read was unsuccessful\n\r");
			}

			PRINT("RD = ");
			for(i=0;i<10;i++)
			{
				PRINT("0x%X, ",sdBlk[i]);
			}
			PRINT("\n\r");
			if(c != sdBlk[0])
			{
				PRINT("Re-Init SD card\n\r");
				mmcsd_init(mysdcard.ctlr);
				sdcard_init(&mysdcard);
			}	
		}		



/*************************** I2C Testing ********************************/
/*
		i2c_init(i2c0,1000000);
		i2c_slave(i2c0,0x50,ADDR_7BIT);

		i2c_tx(i2c0,test_str,2);
		i2c_rx(i2c0,test_data,50);	
		i2c_stop(i2c0);

		for(i=0;i<50;i++)
		{
			sprintf(str,"%X,", test_data[i]);
			uart0_puts(str);
		}
*/
		while(1)
		{
		}
}


/* -- uart_soft_reset ---------------------------------------------------
 *
 * Reset uart0 and disable smart idle. This function is located here 
 * instead of a uarti.c file because it is used specifically in the 
 * bootloader. 
 *
 * reference file:
 * 	u-boot/arch/arm/cpu/armv7/am33xx/board.c
 * --------------------------------------------------------------------*/
/*#define UART_RESET		(0x1 << 1)
#define UART_CLK_RUNNING_MASK	0x1
#define UART_SMART_IDLE_EN	(0x1 << 3)

static void uart_soft_reset(void)
{
	unsigned int regval;

	regval = readl(UART0_SYSC);
	regval |= UART_RESET;
	writel(regval, UART0_SYSC);
	while (
	(readl(UART0_SYSS) & UART_CLK_RUNNING_MASK) != UART_CLK_RUNNING_MASK)
		;

	regval = readl(UART0_SYSC);
	regval |= UART_SMART_IDLE_EN;
	writel(regval, UART0_SYSC);
}
*/
/***************************** End of file ******************************/

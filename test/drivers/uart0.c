/* ***********************************************************************
 * file: 		uart0.c
 * author: 	jacob cook
 * created on: 3/24/2014
 *
 * This file contains lowlevel routines to interface with uart0. 
 * ***********************************************************************
 */

#include "io.h"
#include "uart0.h"
#include "types.h"

/* define UART system clock */
#define UART_SYS_CLK (48000000) /* 48 Mhz */

/* baudrate */
#define MODE_X_DIV	16
#define UART0_BAUDRATE	115200
#define UART0_BAUD_DIV 	26 
	
/* default for Line Contro Register (LCR) */
#define UART0_LCR_DEF	0x03	/* 8 data, 1 stop, no parity */

/* default for Modern Control Register (MCR) */
#define UART0_MCR_DEF (UART0_MCR_DTR | UART0_MCR_RTS) /* DTR/RTS active */

/* default for FIFO Control Register (FCR) */
#define UART0_FCR_DEF (UART0_FCR_FIFO_EN | \
												UART0_FCR_CLEAR_RCVR | \
												UART0_FCR_CLEAR_XMIT)

/* default for Interrupt Enable Register (IER) */
#define UART0_IER_DEF 0x00	/* disable all interrrupts */


/* -- uart0_init --------------------------------------------------------
 * 
 * Initialize UART0 to 115200, 8 bit, 1 stop, no parity.
 * 
 * reference file:
 * 	u-boot/driers/serial/ns16550.c
 * reference function:
 *	NS16550_init 
 * --------------------------------------------------------------------*/

void uart0_init(void)
{
	/* make sure transmitter buffer is empty */
	while(!(readb(UART0_LSR) & UART0_LSR_TXSRE))
		;

	/* disable uart interrupts */
	writeb(UART0_IER_DEF, UART0_IER);
	
	/* reset uart mode to default state */
	writeb(0x7, UART0_MDR1);
	
	/* Set LCR register and enable access to DLL and DLH */
	writeb(UART0_LCR_DIV_EN | UART0_LCR_DEF, UART0_LCR);

	/* Zero out Divisor latches (DLL & DLH) */
	writeb(0, UART0_DLL);
	writeb(0, UART0_DLH);

	/* disable access to DLL and DLH */
	writeb(UART0_LCR_DEF, UART0_LCR);
	
	/* set default modem control register (MCR) */
	writeb(UART0_MCR_DEF, UART0_MCR);

	/* set defualt FIFO control register (FCR) */
	writeb(UART0_FCR_DEF, UART0_FCR);

	/* Re-enable access to DLL and DLH registers */
	writeb(UART0_LCR_DIV_EN | UART0_LCR_DEF, UART0_LCR);
	
	/* Set Buad rate divisors */
	writeb(UART0_BAUD_DIV & 0xFF, UART0_DLL);
	writeb((UART0_BAUD_DIV >> 8) & 0xFF, UART0_DLH);

	/* disable access to DLL and DLH */
	writeb(UART0_LCR_DEF, UART0_LCR);
		
	/* Put in UART 16x mode */
	writeb(0, UART0_MDR1);
}

/* -- uart0_putc --------------------------------------------------------
 * 
 * reference file:
 * 	u-boot/drivers/serial/ns16550.c
 * reference function:
 * 	ns16550_putc 
 * --------------------------------------------------------------------*/

void uart0_putc(char c)
{
	/* wait for a spot in the transmitter FIFO */
	while((readb(UART0_LSR) & UART0_LSR_TXFIFOE) == 0)
		;

	/* Load character in to FIFO */
	writeb(c, UART0_THR);
}

void uart0_puts(char *s)
{
	while(*s != 0)
	{
		uart0_putc(*s++);
	}
}

/* -- uart0_getc --------------------------------------------------------
 * 
 * reference file:
 * 	u-boot/drivers/serial/ns16550.c
 * reference function:
 *	ns16550_getc
 * --------------------------------------------------------------------*/

unsigned char uart0_getc(void)
{
	/* wait for something to be loaded into the RX register */
	while((readb(UART0_LSR) & UART0_LSR_DR) == 0)
		;
	
	return readb(UART0_RHR);
}

/* -- uart0_tstc --------------------------------------------------------
 *
 * reference file:
 * 	u-boot/drivers/serial/ns16550.c
 * reference function:
 * 	NS16550_tstc 
 * --------------------------------------------------------------------*/

unsigned int uart0_tstc(void)
{
	return (readb(UART0_LSR) & UART0_LSR_DR) != 0;
}

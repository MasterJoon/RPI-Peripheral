#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include "uart.h"

static volatile unsigned *g_pvuiUART;
static volatile unsigned *g_pvuiClk;
static void *uart_map;
static void *clk_map;

void uart_init(int baud)
{
	int mem_fd;

	/* open /dev/mem */
	if((mem_fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0) {
		printf("can't open /dev/mem \n");
		exit(-1);
	}

	/* mmap GPIO */
	uart_map = mmap(
		NULL,					// Any adddress in our space will do
		getpagesize(),			// Map length
		PROT_READ | PROT_WRITE,	// Enable reading & writting to mapped memory
		MAP_SHARED,				// Shared with other processes
		mem_fd,					// File to map
		BCM_UART_BASE			// Offset to UART peripheral
	);

	close(mem_fd); // No need to keep mem_fd open after mmap

	if(uart_map == MAP_FAILED) {
		printf("uart_init: mmap error %d\n", (int)uart_map); // errno also set!
		exit(-1);
	}

	// Always use volatile pointer!
	g_pvuiUART = (volatile unsigned *)uart_map;

	/* open /dev/mem */
	if((mem_fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0) {
		printf("can't open /dev/mem \n");
		exit(-1);
	}

	/* mmap GPIO */
	clk_map = mmap(
		NULL,					// Any adddress in our space will do
		getpagesize(),			// Map length
		PROT_READ | PROT_WRITE,	// Enable reading & writting to mapped memory
		MAP_SHARED,				// Shared with other processes
		mem_fd,					// File to map
		BCM_CLK_BASE			// Offset to GPIOCLK peripheral
	);

	close(mem_fd); // No need to keep mem_fd open after mmap

	if(clk_map == MAP_FAILED) {
		printf("uart_init: clk mmap error %d\n", (int)clk_map); // errno also set!
		exit(-1);
	}

	// Always use volatile pointer!
	g_pvuiClk = (volatile unsigned *)clk_map;

	// Diable UART
	*(g_pvuiUART + BCM_UART_CR) = 0;

	// Stop UART Clock
	*(g_pvuiClk + BCM_UART_CLK_CTL) = BCM_CLK_PASSWORD | 0x01;
	usleep(110);
	while((*(g_pvuiClk + BCM_UART_CLK_CTL) & 0x80) != 0) {
		usleep(1);
	}

#if (BCM_PERI_ADRS_BASE == BCM_2837_PERI_ADRS_BASE)
	*(g_pvuiClk + BCM_UART_CLK_DIV) = BCM_CLK_PASSWORD | (1 << 12);
	// Start UART clock
    *(g_pvuiClk + BCM_UART_CLK_CTL) = BCM_CLK_PASSWORD | 0x10 | CLK_PLLD;

	double bauddiv = 500000000.0 / (16.0 * (double)baud);
	uint32_t ibaud = (uint32_t)bauddiv;
	uint32_t fbaud = (uint32_t)(63.0 * (bauddiv - (double)ibaud));
	printf("BAUDDIV: %llf\r\n", bauddiv);
	bauddiv = (double)ibaud + ((double)fbaud / 63.0);
	printf("IBRD: %d FBRD: %d\r\n", ibaud, fbaud);
	printf("Actual Baudrade: %d\r\n", (int)(500000000.0 / (16.0 * bauddiv)));
#elif (BCM_PERI_ADRS_BASE == BCM_2711_PERI_ADRS_BASE)
	*(g_pvuiClk + BCM_UART_CLK_DIV) = BCM_CLK_PASSWORD | (2 << 12) | (0 << 9);
	// Start UART clock
	*(g_pvuiClk + BCM_UART_CLK_CTL) = BCM_CLK_PASSWORD | 0x10 | CLK_PLLD;

	double bauddiv = 375000000.0 / (16.0 * (double)baud);
	uint32_t ibaud = (uint32_t)bauddiv;
	uint32_t fbaud = (uint32_t)(64.0 * (bauddiv - (double)ibaud));
	printf("BAUDDIV: %llf\r\n", bauddiv);
	bauddiv = (double)ibaud + ((double)fbaud / 64.0);
	printf("IBRD: %d FBRD: %d\r\n", ibaud, fbaud);
	printf("Actual Baudrade: %d\r\n", (int)(375000000.0 / (16.0 * bauddiv)));
#endif
	*(g_pvuiUART + BCM_UART_IBRD) = ibaud;
	*(g_pvuiUART + BCM_UART_FBRD) = fbaud;
	*(g_pvuiUART + BCM_UART_LCRH) = BCM_UART_LCRH_WLEN | BCM_UART_LCRH_FEN;
	*(g_pvuiUART + BCM_UART_DMACR) = BCM_UART_DMACR_TXDMAE | BCM_UART_DMACR_RXDMAE;
	*(g_pvuiUART + BCM_UART_CR) = BCM_UART_CR_RXE | BCM_UART_CR_TXE | BCM_UART_CR_UARTEN;
}

void uart_wait_tx(void)
{
	while(BCM_UART_FR_TXFE != (*(g_pvuiUART + BCM_UART_FR) & (BCM_UART_FR_TXFE | BCM_UART_FR_BUSY)));
}

int uart_tx_byte(uint8_t ucByte)
{
	while(BCM_UART_FR_TXFE != (*(g_pvuiUART + BCM_UART_FR) & (BCM_UART_FR_TXFE | BCM_UART_FR_BUSY)));
	*(g_pvuiUART + BCM_UART_DR) = ucByte;

	return 1;
}

int uart_tx(uint8_t *pucBuff, int len)
{
//	while(BCM_UART_FR_TXFE != (*(g_pvuiUART + BCM_UART_FR) & (BCM_UART_FR_TXFE | BCM_UART_FR_BUSY)));
	for(int i = 0; i < len; i++) {
		while(BCM_UART_FR_TXFE != (*(g_pvuiUART + BCM_UART_FR) & (BCM_UART_FR_TXFE | BCM_UART_FR_BUSY)));
		*(g_pvuiUART + BCM_UART_DR) = pucBuff[i];
	}
	return len;
}

int uart_rx_byte(uint8_t *pucByte)
{
	if(*(g_pvuiUART + BCM_UART_FR) & BCM_UART_FR_RXFE) {
		return 0;
	}
	else {
		*pucByte = *(g_pvuiUART + BCM_UART_DR);
		return 1;
	}
}

int uart_rx(uint8_t *pucBuff, int len)
{
	int cnt = 0;
	while(1) {
		if(*(g_pvuiUART + BCM_UART_FR) & BCM_UART_FR_RXFE) {
			break;
		}
		pucBuff[cnt++] = *(g_pvuiUART + BCM_UART_DR);
	}

	return cnt;
}

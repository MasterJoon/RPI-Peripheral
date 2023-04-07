#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include "spi.h"

static volatile unsigned *g_pvuiSPI;
static void *spi_map;

void spi_init(int mode, int hz)
{
	int mem_fd;

	/* open /dev/mem */
	if((mem_fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0) {
		printf("can't open /dev/mem \n");
		exit(-1);
	}

	/* mmap GPIO */
	spi_map = mmap(
		NULL,					// Any adddress in our space will do
		getpagesize(),			// Map length
		PROT_READ | PROT_WRITE,	// Enable reading & writting to mapped memory
		MAP_SHARED,				// Shared with other processes
		mem_fd,					// File to map
		BCM_SPI_BASE			// Offset to SPI peripheral
	);

	close(mem_fd); // No need to keep mem_fd open after mmap

	if(spi_map == MAP_FAILED) {
		printf("spi_init: mmap error %d\n", (int)spi_map); // errno also set!
		exit(-1);
	}

	// Always use volatile pointer!
	g_pvuiSPI = (volatile unsigned *)spi_map;

	uint32_t cs = BCM_SPI_CS_CLEAR_RX | BCM_SPI_CS_CLEAR_TX;
	if(mode & SPI_CPOL) {
		cs |= BCM_SPI_CS_CPOL;
	}
	if(mode * SPI_CPHA) {
		cs |= BCM_SPI_CS_CPHA;
	}
	*(g_pvuiSPI + BCM_SPI_CS) = cs;

	uint32_t cdiv;
	if(BCM_SPI_CLK_MAX < hz) {
		cdiv = 2;
	}
	else if(hz) {
		cdiv = (int)(((double)BCM_APB_CLK / (double)hz) + 0.5);
		cdiv += (cdiv % 2);
	}
//	printf("cdiv: %d\r\n", cdiv);
	*(g_pvuiSPI + BCM_SPI_CLK) = cdiv;
}

uint8_t spi_txrx_byte(uint8_t tx)
{
	uint8_t rx = 0;
	uint32_t cs = *(g_pvuiSPI + BCM_SPI_CS);
	cs |= BCM_SPI_CS_TA;
	*(g_pvuiSPI + BCM_SPI_CS) = cs;

	if(*(g_pvuiSPI + BCM_SPI_CS) & BCM_SPI_CS_TXD) {
		*(g_pvuiSPI + BCM_SPI_FIFO) = tx;
//		printf("tx %d\r\n", tx);
	}
	while(0 == (*(g_pvuiSPI + BCM_SPI_CS) & BCM_SPI_CS_DONE)) {
//		printf("waitng DONE...\r\n");
	}
	if(*(g_pvuiSPI + BCM_SPI_CS) & BCM_SPI_CS_RXD) {
		rx = *(g_pvuiSPI + BCM_SPI_FIFO);
//		printf("rx: %d\r\n", rx);
	}
	cs &= ~BCM_SPI_CS_TA;
	*(g_pvuiSPI + BCM_SPI_CS) = cs;

	return rx;
}

int spi_txrx(uint8_t *tx, uint8_t *rx, int len)
{
	int cnt;
	uint32_t cs = *(g_pvuiSPI + BCM_SPI_CS);

	cs |= BCM_SPI_CS_TA;
	*(g_pvuiSPI + BCM_SPI_CS) = cs;

	for(cnt = 0; cnt < len; cnt++) {
		if(*(g_pvuiSPI + BCM_SPI_CS) & BCM_SPI_CS_TXD) {
			*(g_pvuiSPI + BCM_SPI_FIFO) = tx[cnt];
//			printf("tx %d\r\n", tx);
		}
		while(0 == (*(g_pvuiSPI + BCM_SPI_CS) & BCM_SPI_CS_DONE)) {
//			printf("waitng DONE...\r\n");
		}
		if(*(g_pvuiSPI + BCM_SPI_CS) & BCM_SPI_CS_RXD) {
			rx[cnt] = *(g_pvuiSPI + BCM_SPI_FIFO);
//			printf("rx: %d\r\n", rx);
		}
	}

	cs &= ~BCM_SPI_CS_TA;
	*(g_pvuiSPI + BCM_SPI_CS) = cs;

	return cnt;
}

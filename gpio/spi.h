#ifndef __SPI_H__
#define __SPI_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <linux/spi/spidev.h>
#include "gpio.h"

#define BCM_SPI_BASE		(BCM_PERI_ADRS_BASE + 0x00204000)

#define BCM_SPI_CS			(0x00 >> 2)
#define BCM_SPI_FIFO		(0x04 >> 2)
#define BCM_SPI_CLK			(0x08 >> 2)
#define BCM_SPI_DLEN		(0x0C >> 2)
#define BCM_SPI_LTOH		(0x10 >> 2)
#define BCM_SPI_DC			(0x14 >> 2)

#define BCM_SPI_CS_LEN_LONG	0x02000000
#define BCM_SPI_CS_DMA_LEN	0x01000000
#define BCM_SPI_CS_CSPOL2	0x00800000
#define BCM_SPI_CS_CSPOL1	0x00400000
#define BCM_SPI_CS_CSPOL0	0x00200000
#define BCM_SPI_CS_RXF		0x00100000
#define BCM_SPI_CS_RXR		0x00080000
#define BCM_SPI_CS_TXD		0x00040000
#define BCM_SPI_CS_RXD		0x00020000
#define BCM_SPI_CS_DONE		0x00010000
#define BCM_SPI_CS_LEN		0x00002000
#define BCM_SPI_CS_REN		0x00001000
#define BCM_SPI_CS_ADCS		0x00000800
#define BCM_SPI_CS_INTR		0x00000400
#define BCM_SPI_CS_INTD		0x00000200
#define BCM_SPI_CS_DMAEN	0x00000100
#define BCM_SPI_CS_TA		0x00000080
#define BCM_SPI_CS_CSPOL	0x00000040
#define BCM_SPI_CS_CLEAR_RX	0x00000020
#define BCM_SPI_CS_CLEAR_TX	0x00000010
#define BCM_SPI_CS_CPOL		0x00000008
#define BCM_SPI_CS_CPHA		0x00000004
#define BCM_SPI_CS_CS_10	0x00000002
#define BCM_SPI_CS_CS_01	0x00000001

#if (BCM_PERI_ADRS_BASE == BCM_2837_PERI_ADRS_BASE)
	#define BCM_APB_CLK		(250000000)
#elif (BCM_PERI_ADRS_BASE == BCM_2711_PERI_ADRS_BASE)
	#define BCM_APB_CLK		(200000000)
#endif
#define BCM_SPI_CLK_MAX		(BCM_APB_CLK >> 1)

extern void spi_init(int mode, int hz);
extern uint8_t spi_txrx_byte(uint8_t tx);
extern int spi_txrx(uint8_t *tx, uint8_t *rx, int len);

#ifdef __cplusplus
}
#endif

#endif /* __SPI_H__ */

#ifndef __UART_H__
#define __UART_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "gpio.h"

#define BCM_UART_BASE			(BCM_PERI_ADRS_BASE + 0x00201000)

#if (BCM_PERI_ADRS_BASE == BCM_2711_PERI_ADRS_BASE)
	#define BCM_UARTx			(UART2)
		#define UART0			(0x000)
		#define UART2			(0x400)
		#define UART3			(0x600)
		#define UART4			(0x800)
		#define UART5			(0xA00)
#else
	#define BCM_UARTx			(0)
#endif

#define BCM_UART_DR				((BCM_UARTx + 0x00) >> 2)
#define BCM_UART_FR				((BCM_UARTx + 0x18) >> 2)
#define BCM_UART_IBRD			((BCM_UARTx + 0x24) >> 2)
#define BCM_UART_FBRD			((BCM_UARTx + 0x28) >> 2)
#define BCM_UART_LCRH			((BCM_UARTx + 0x2C) >> 2)
#define BCM_UART_CR				((BCM_UARTx + 0x30) >> 2)
#define BCM_UART_DMACR			((BCM_UARTx + 0x48) >> 2)

#define BCM_UART_FR_TXFE		(0x00000080)
#define BCM_UART_FR_RXFF		(0x00000040)
#define BCM_UART_FR_TXFF		(0x00000020)
#define BCM_UART_FR_RXFE		(0x00000010)
#define BCM_UART_FR_BUSY		(0x00000008)
#define BCM_UART_FR_CTS			(0x00000001)

#define BCM_UART_LCRH_WLEN		(0x00000060)
#define BCM_UART_LCRH_FEN		(0x00000010)

#define BCM_UART_CR_RTSEN		(0x00004000)
#define BCM_UART_CR_RTS			(0x00000800)
#define BCM_UART_CR_RXE			(0x00000200)
#define BCM_UART_CR_TXE			(0x00000100)
#define BCM_UART_CR_UARTEN		(0x00000001)

#define BCM_UART_DMACR_TXDMAE	(0x00000002)
#define BCM_UART_DMACR_RXDMAE	(0x00000001)

extern void uart_init(int baud);
extern void uart_wait_tx(void);

extern int uart_tx_byte(uint8_t ucByte);
extern int uart_tx(uint8_t *pucBuff, int len);
extern int uart_rx_byte(uint8_t *pucByte);
extern int uart_rx(uint8_t *pucBuff, int len);

#ifdef __cplusplus
}
#endif

#endif /* __UART_H__ */

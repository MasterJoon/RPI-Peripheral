#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

#define BCM_PERI_ADRS_BASE				BCM_2711_PERI_ADRS_BASE
	#define BCM_2837_PERI_ADRS_BASE		0x3F000000		// RPI 3
	#define BCM_2711_PERI_ADRS_BASE		0xFE000000		// RPI 4

#if (BCM_PERI_ADRS_BASE == BCM_2837_PERI_ADRS_BASE)
	#define GPIO_PIN_MAX				(53)
#elif (BCM_PERI_ADRS_BASE == BCM_2711_PERI_ADRS_BASE)
	#define GPIO_PIN_MAX				(57)
#endif

#define BCM_CLK_BASE					(BCM_PERI_ADRS_BASE + 0x00101000)
#define	BCM_CLK_PASSWORD				(0x5A000000)

#define	BCM_PWM_CLK_CTL					(0xA0 >> 2)
#define	BCM_PWM_CLK_DIV					(0xA4 >> 2)
#define BCM_UART_CLK_CTL				(0xF0 >> 2)
#define	BCM_UART_CLK_DIV				(0xF4 >> 2)


#define BCM_GPIO_BASE					(BCM_PERI_ADRS_BASE + 0x00200000)

//#define BCM_GPIO_GPFSEL0				(0x00 >> 2)
//#define BCM_GPIO_GPFSEL1				(0x04 >> 2)
//#define BCM_GPIO_GPFSEL2				(0x08 >> 2)
//#define BCM_GPIO_GPFSEL3				(0x0C >> 2)
//#define BCM_GPIO_GPFSEL4				(0x10 >> 2)
//#define BCM_GPIO_GPFSEL5				(0x14 >> 2)
#define BCM_GPIO_GPFSEL(n)				(n)
#define BCM_GPIO_GPSET0					(0x1C >> 2)
#define BCM_GPIO_GPSET1					(0x20 >> 2)
#define BCM_GPIO_GPCLR0					(0x28 >> 2)
#define BCM_GPIO_GPCLR1					(0x2C >> 2)
#define BCM_GPIO_GPLEV0					(0x34 >> 2)
#define BCM_GPIO_GPLEV1					(0x38 >> 2)
//#define BCM_GPIO_PUP_PDN_CNTRL_REG0	(0xE4 >> 2)
//#define BCM_GPIO_PUP_PDN_CNTRL_REG1	(0xE8 >> 2)
//#define BCM_GPIO_PUP_PDN_CNTRL_REG2	(0xEC >> 2)
//#define BCM_GPIO_PUP_PDN_CNTRL_REG3	(0xF0 >> 2)

#define BCM_GPIO_GPFSELx_FSELx(n)		(0x00000003 << (3 * (n)))

typedef enum {
	INPUT	= 0,
	OUTPUT	= 1,
	ALT0	= 4,
	ALT1	= 5,
	ALT2	= 6,
	ALT3	= 7,
	ALT4	= 3,
	ALT5	= 2,
} E_GPIO_MODE;
						//   BCM2837	|   BCM2711
typedef enum {			//-----------------------------
	CLK_OSC		= 1,	//  192 [MHz]	|  54 [MHz]
	CLK_PLLA	= 4,	// Not working	| Not working
	CLK_PLLC	= 5,	// 1000 [MHz]	| 325 [MHz]
	CLK_PLLD	= 6,	//  500 [MHz]	| 375 [MHz]
} E_CLK_SRC;

extern void gpio_init(void);
extern void gpio_setMode(int pin, E_GPIO_MODE mode);
extern void gpio_write(int pin, int val);
extern int gpio_read(int pin);

#ifdef __cplusplus
}
#endif

#endif /* __GPIO_H__ */

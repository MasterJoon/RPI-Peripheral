#ifndef __PWM_H__
#define __PWM_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "gpio.h"

#define BCM_PWM_BASE			(BCM_PERI_ADRS_BASE + 0x0020C000)

#define BCM_PWM_CTL				(0x00 >> 2)
#define BCM_PWM_STA				(0x04 >> 2)
#define BCM_PWM_DMAC			(0x08 >> 2)
#define BCM_PWM_FIF1			(0x18 >> 2)
#define BCM_PWM_RNG(ch)			((0x10 + (ch << 4)) >> 2)
#define BCM_PWM_DAT(ch)			((0x14 + (ch << 4)) >> 2)

#define BCM_PWM_CTL_CLRF		(0x00000040)
#define BCM_PWM_CTL_MSEN(ch)	(0x00000080 << (ch << 3))
#define BCM_PWM_CTL_USEF(ch)	(0x00000020 << (ch << 3))
#define BCM_PWM_CTL_POLA(ch)	(0x00000010 << (ch << 3))
#define BCM_PWM_CTL_SBIT(ch)	(0x00000008 << (ch << 3))
#define BCM_PWM_CTL_RPTL(ch)	(0x00000004 << (ch << 3))
#define BCM_PWM_CTL_MODE(ch)	(0x00000002 << (ch << 3))
#define BCM_PWM_CTL_PWEN(ch)	(0x00000001 << (ch << 3))

typedef enum {
	PWM0 = 0,
	PWM1 = 1,
} E_PWM_CH;

typedef enum {
	PWM_POLA_NORM = 0,
	PWM_POLA_REVS = 1,
} E_PWM_POLA;

/*
	                Source_clock
	PWM Frequency = -------------
					div + range
	
*/
extern void pwm_init(E_PWM_CH eCh, E_PWM_POLA ePola, E_CLK_SRC eClkSrc, uint32_t div, uint32_t range);
extern void pwm_set(E_PWM_CH eCh, uint32_t duty);

#ifdef __cplusplus
}
#endif

#endif /* __PWM_H__ */
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include "pwm.h"

static volatile unsigned *g_pvuiPWM;
static volatile unsigned *g_pvuiClk;
static void *pwm_map;
static void *clk_map;

void pwm_init(E_PWM_CH eCh, E_PWM_POLA ePola, E_CLK_SRC eClkSrc, uint32_t div, uint32_t range)
{
	int mem_fd;

#if (BCM_PERI_ADRS_BASE == BCM_2711_PERI_ADRS_BASE)
	div = 540 * div / 192;
#endif
	div &= 4095;
	if(1 == div) {
		div = 2;
	}

	/* open /dev/mem */
	if((mem_fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0) {
		printf("can't open /dev/mem \n");
		exit(-1);
	}

	/* mmap GPIO */
	pwm_map = mmap(
		NULL,					// Any adddress in our space will do
		getpagesize(),			// Map length
		PROT_READ | PROT_WRITE,	// Enable reading & writting to mapped memory
		MAP_SHARED,				// Shared with other processes
		mem_fd,					// File to map
		BCM_PWM_BASE			// Offset to PWM peripheral
	);

	close(mem_fd); // No need to keep mem_fd open after mmap

	if(pwm_map == MAP_FAILED) {
		printf("pwm_init: mmap error %d\n", (int)pwm_map); // errno also set!
		exit(-1);
	}

	// Always use volatile pointer!
	g_pvuiPWM = (volatile unsigned *)pwm_map;

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
		printf("pwm_init: clk mmap error %d\n", (int)clk_map); // errno also set!
		exit(-1);
	}

	// Always use volatile pointer!
	g_pvuiClk = (volatile unsigned *)clk_map;

	// Stop PWM
	*(g_pvuiPWM + BCM_PWM_CTL) = 0;
	// Stop PWM Clock
	*(g_pvuiClk + BCM_PWM_CLK_CTL) = BCM_CLK_PASSWORD | 0x01;
	usleep(110);
	while((*(g_pvuiClk + BCM_PWM_CLK_CTL) & 0x80) != 0) {
		usleep(1);
	}

	*(g_pvuiClk + BCM_PWM_CLK_DIV) = BCM_CLK_PASSWORD | (div << 12) ;
	// Start PWM clock
    *(g_pvuiClk + BCM_PWM_CLK_CTL) = BCM_CLK_PASSWORD | 0x10 | eClkSrc;

	// PWM Setting
	uint32_t pwm_ctl;
	pwm_ctl = BCM_PWM_CTL_PWEN(eCh);
	pwm_ctl |= BCM_PWM_CTL_MSEN(eCh);

	if(PWM_POLA_REVS == ePola) {
		pwm_ctl |= BCM_PWM_CTL_POLA(eCh);
	}
	*(g_pvuiPWM + BCM_PWM_CTL) = pwm_ctl;

	*(g_pvuiPWM + BCM_PWM_RNG(eCh)) = range;
	usleep(10);
	*(g_pvuiPWM + BCM_PWM_DAT(eCh)) = 0;
}


void pwm_set(E_PWM_CH eCh, uint32_t duty)
{
	*(g_pvuiPWM + BCM_PWM_DAT(eCh)) = duty;
}

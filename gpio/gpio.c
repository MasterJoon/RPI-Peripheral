#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include "gpio.h"

static volatile unsigned *g_pvuiGPIO;
static void *gpio_map;

void gpio_init(void)
{
	int mem_fd;

	/* open /dev/mem */
	if((mem_fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0) {
		printf("can't open /dev/mem \n");
		exit(-1);
	}

	/* mmap GPIO */
	gpio_map = mmap(
		NULL,						// Any adddress in our space will do
		getpagesize(),			// Map length
		PROT_READ | PROT_WRITE,	// Enable reading & writting to mapped memory
		MAP_SHARED,				// Shared with other processes
		mem_fd,					// File to map
		BCM_GPIO_BASE				// Offset to GPIO peripheral
	);

	close(mem_fd); // No need to keep mem_fd open after mmap

	if(gpio_map == MAP_FAILED) {
		printf("gpio_init: mmap error %d\n", (int)gpio_map); // errno also set!
		exit(-1);
	}

	// Always use volatile pointer!
	g_pvuiGPIO = (volatile unsigned *)gpio_map;
}

void gpio_setMode(int pin, E_GPIO_MODE mode)
{
	int gpfsel_ofs;
	int pin_nx = pin / 10;
	int pin_xn = pin % 10;
	uint32_t gpfsel;

	if(GPIO_PIN_MAX < pin) return;

	gpfsel_ofs = BCM_GPIO_GPFSEL(pin_nx);
	gpfsel = *(g_pvuiGPIO + gpfsel_ofs);
	gpfsel &= ~BCM_GPIO_GPFSELx_FSELx(pin_xn);
	gpfsel |= ((uint32_t)mode << (3 * pin_xn));
	*(g_pvuiGPIO + gpfsel_ofs) = gpfsel;
}

void gpio_write(int pin, int val)
{
	int pin_val;
	uint32_t reg_ofs;

	if(GPIO_PIN_MAX < pin) return;

	if(0 < val)	reg_ofs = BCM_GPIO_GPSET0;
	else			reg_ofs = BCM_GPIO_GPCLR0;

	if(pin < 32) {
		pin_val = 1 << pin;
	}
	else {
		pin_val = 1 << (pin - 32);
		reg_ofs++;
	}

	*(g_pvuiGPIO + reg_ofs) = pin_val;
}

int gpio_read(int pin)
{
	int pin_msk;
	uint32_t reg_ofs = BCM_GPIO_GPLEV0;

	if(GPIO_PIN_MAX < pin) return 0;

	if(pin < 32) {
		pin_msk = 1 << pin;
	}
	else {
		pin_msk = 1 << (pin - 32);
		reg_ofs++;
	}

	return !(0 == (*(g_pvuiGPIO + reg_ofs) & pin_msk));
}

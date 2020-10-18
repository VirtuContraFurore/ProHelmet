/*
 * utils.h
 *
 *  Created on: Sep 7, 2020
 *      Author: luca
 */

#ifndef UTILS_H_
#define UTILS_H_

#ifndef STM32F429xx
#define STM32F429xx
#endif

#include <stdlib.h>
#include "stm32f4xx.h"

#include "stm32f429xx.h"
#include "stm32f4xx_hal.h"

#include "driver/oled.h"

extern OLED_data_t * screen;

#define ON 	1
#define OFF 0

/*
 * Enable semihosting console
 */
#define DEBUG_SEMIHOSTING OFF


#define REG_CLEAR(reg, field) reg &= ~(field)
#define REG_SET(reg, field) reg |= (field)
#define REG_SETV(reg, field, value) reg &= ~(field##_Msk); reg |= (value) << (field##_Pos)

/*
 * GPIO mask missing from ST's libs
 */
#define GPIO_MODER_INPUT		0b00
#define GPIO_MODER_OUTPUT		0b01
#define GPIO_MODER_ALTERNATE	0b10
#define GPIO_MODER_ANALOG		0b11
#define GPIO_OTYPER_PUSHPULL	 0b0
#define GPIO_OTYPER_OPENDRAIN	 0b1
#define GPIO_PUPDR_NOPULL		0b00
#define GPIO_PUPDR_PULLUP		0b01
#define GPIO_PUPDR_PULLDOWN		0b10
#define GPIO_OSPEEDR_5MHZ		0b00
#define GPIO_OSPEEDR_25Mhz		0b01
#define GPIO_OSPEEDR_50Mhz		0b10
#define GPIO_OSPEEDR_100Mhz		0b11

#if DEBUG_SEMIHOSTING == ON
#define __PRINT_DEBUG_STR(string)	__debug_str((string), sizeof((string))/sizeof(unsigned char))
#else
#define __PRINT_DEBUG_STR(string)	/* No operation */
#endif

/**
 * Delay with 180Mhz sys clock
 */
static inline void __delay_ms(uint32_t ms){
	while(ms--){
		uint32_t val = 180 * 1000 / 3 - 3;
		while(--val);
	}
}

/*
 * Send semihosting cmd
 * Note: without the debugger attached the program will HALT!
 */
static inline void __send_command(int command, void *message)
{
   asm(
       "mov r0, %[cmd];"
       "mov r1, %[msg];"
       "bkpt #0xAB"
		:
        : [cmd] "r" (command), [msg] "r" (message)
        : "r0", "r1", "memory"
	);
}

/*
 * Print debug str
 * arguments are the string and the size of the string in bytes (last terminal zero included)
 */
static inline void __debug_str(unsigned char * str, unsigned int size){
	uint32_t m[] = { 1/*stdout*/, (uint32_t)str, size - 1 };
	__send_command(0x05/* some interrupt ID */, m);
}


#endif /* UTILS_H_ */

/*
 * main.c
 *
 *  Created on: Aug 30, 2020
 *      Author: luca
 */

#include <sys/types.h>
#include <math.h>

#ifndef ERIKA
#define STM32F429xx
#define USE_HAL_DRIVER
#endif

#include "ee.h"

#include "utils.h"
#include "system.h"
#include "sensors.h"

#include "driver/oled.h"
#include "driver/graphics.h"
#include "driver/bluetooth.h"

OLED_data_t screen_data;
OLED_data_t * screen;

static void init();

int main(){
	init();
	ActivateTask(ViewBoot);
	StartOS(OSDEFAULTAPPMODE);
	return 0;
}

/*
 * Idle hook
 */
void idle(){

}

static void init(){
	screen = &screen_data;

	SYS_InitSys();
	SYS_InitGreenLed();
	SYS_InitSysTimer();
	SYS_InitSideButtons();
	SYS_InitPhotores();

	BT_Init(BT_BAUDRATE, BT_PARITY, BT_STOPBITS);

	OLED_InitScreen(screen, OLED_SOUTH);

	Sensor_init();
}

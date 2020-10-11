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

/*
 * Struttura del programma:
 *
 * un task per "view" sullo schermo
 * background task: ricevere notifiche
 * background task: mandare dati
 * background task: ricevere dati
 * background task: controllare che non siamo caduti e nel caso lanciare un SoS
 * background task: check accelerometro
 * background task: check giroscopio
 * background task: check buttons
 * background task: check pressure & temp
 * interrupt degli user buttons
 *
 * Usare le ALARMSs per i background, aggioranandole con l'ISR every ms
 *
 * la photores viene letta esplicitamente
 * gli altri device pure dire...
 *
 * all'avvio fare un check di tutto l'hardware....
 * fare uno splash screen carino...! :-)
 *
 * cosa mettere nel main...?
 *
 * Testare acceleremotro + giroscopio
 *
 * c'è anche il sensore di pressione.... può essere utile...? ---> previsioni meteo...!!!
 */

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

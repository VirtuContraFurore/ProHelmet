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

#include "driver/oled.h"
#include "driver/graphics.h"

/*
 * Struttura del programma:
 *
 * un task per "view" sullo schermo
 * background task: ricevere notifiche
 * background task: mandare dati
 * background task: ricevere dati
 * background task: controllare la luminosità esterna
 * background task: controllare la pressione esterna
 * background task: controllare che non siamo caduti e nel caso lanciare un SoS
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

int main(){
	screen = &screen_data;
	SYS_InitSys();

	ActivateTask(ViewBoot);
	StartOS(OSDEFAULTAPPMODE);
	return 0;
}

/*
 * Idle hook
 */
void idle(){

}



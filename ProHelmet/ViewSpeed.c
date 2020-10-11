/*
 * ViewSpeed.c
 *
 *  Created on: Oct 11, 2020
 *      Author: luca
 */

#include "ViewSpeed.h"

TASK(ViewSpeed){
	while(1){

		ViewSpeed_drawingLoop();
	}
}

void ViewSpeed_drawingLoop(){
	GR_ClearColor(screen, Color_BLACK);


	GR_SwapBuffers(screen);
}


/*
 * ViewNav.c
 *
 *  Created on: Oct 11, 2020
 *      Author: luca
 */

#include "ViewNav.h"

TASK(ViewNav){
	int down = 0;
	while(1){
		if(SYS_getButton2Status() == 1){
			if(down)
				ChainTask(VIEWNAV_NEXT);
		} else
			down = 1;

		ViewNav_drawingLoop();
	}
}

void ViewNav_drawingLoop(){
	GR_ClearColor(screen, Color_BLACK);

	GR_DrawString(screen, (Point_t) {0,0}, "Navigator", &font_ArialNarrow22px,Color_PURPLE);

	Notifications_Draw();

	GR_SwapBuffers(screen);
}

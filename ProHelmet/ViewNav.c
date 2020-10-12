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


static uint8_t symbol = 0;
static int16_t distance;
static uint8_t msg[256];

void ViewNav_drawingLoop(){
	GR_ClearColor(screen, Color_BLACK);

	if(EPCTL_PayloadReceived(Endpoint_SatNav)){
		symbol = Endpoint_SatNav->data[0];

		if(symbol != 0){
			distance = Endpoint_SatNav->data[1] | (Endpoint_SatNav->data[2] << 8);

			for(int i = 0; i < Endpoint_SatNav->size - 3; i++){
				msg[i] = Endpoint_SatNav->data[i+3];
				if(msg[i] == 0)
					break;
			}
		}

		while(GetResource(Res_Bluetooth) != E_OK);
		EPCTL_RequestData(Endpoint_SatNav);
		ReleaseResource(Res_Bluetooth);
	}

	if(symbol == 0){
		GR_DrawString(screen, (Point_t) {0, 10}, "Awaiting route", &font_ArialNarrow22px, Color_WHITE);
		GR_DrawString(screen, (Point_t) {0, 40}, "from App", &font_ArialNarrow22px, Color_WHITE);
	} else {
		ViewNav_drawNav();
	}

	Notifications_Draw();
	GR_SwapBuffers(screen);
}

void ViewNav_drawNav(){
	Color_t color = Color_RED;

	GR_DrawBitmapColorized(screen, (Point_t) {0,0}, &bitmap_roundabout_2l, Color_ORANGE);

	GR_DrawLine(screen, (Point_t) { 61, 0}, (Point_t) { 61, screen->height}, color);
	GR_DrawLine(screen, (Point_t) { 62, 0}, (Point_t) { 62, screen->height}, color);
	GR_DrawLine(screen, (Point_t) { 63, 0}, (Point_t) { 63, screen->height}, color);

	GR_DrawLine(screen, (Point_t) {0, 61}, (Point_t) { 60, 61}, color);
	GR_DrawLine(screen, (Point_t) {0, 62}, (Point_t) { 60, 62}, color);
	GR_DrawLine(screen, (Point_t) {0, 63}, (Point_t) { 60, 63}, color);

	GR_DrawNumber(screen,(Point_t) {70, -5}, distance, 4, &font_MonoTypewriter22px, Color_LIGHT_BLUE);
	GR_DrawString(screen, (Point_t){126, -5}, "m", &font_MonoTypewriter22px, Color_LIGHT_BLUE);

	GR_DrawString(screen, (Point_t) {68, 25}, msg, &font_RobotoCondensed18px, Color_WHITE);
}

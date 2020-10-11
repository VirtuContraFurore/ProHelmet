/*
 * Notifications.c
 *
 *  Created on: Oct 11, 2020
 *      Author: luca
 */

#include "Notifications.h"

#include "string.h"

#define NFont font_MonoTypewriter22

static time_ms_t start = 0;
static time_ms_t duration;
static char str[256];

static enum {waiting, displaying} state;

void Notifications_Draw(){
	time_ms_t elapsed;
	switch(state){
	case waiting:
		if(!EPCTL_PayloadReceived(Endpoint_Notification))
			return;

		state = displaying;
		start = SYS_getTimeMillis();

		int i;
		for(i = 0; i < Endpoint_Notification->size; i++){
			if(Endpoint_Notification->data[i])
				str[i] = Endpoint_Notification->data[i];
			else {
				str[i+1] = 0;
				break;
			}
		}

		duration = i * 50;

		break;
	case displaying:
		elapsed = SYS_getTimeMillis() - start;

		if(elapsed < Not_scrollTime_ms){
			Notifications_DrawRect(-screen->height + (screen->height * elapsed / Not_scrollTime_ms), Color_WHITE);
		} else if(elapsed < (Not_scrollTime_ms + duration)) {
			Notifications_DrawRect(0, Color_WHITE);
		} else if(elapsed < (Not_scrollTime_ms + Not_scrollTime_ms + duration)){
			Notifications_DrawRect(-(screen->height * (elapsed - Not_scrollTime_ms - duration) / Not_scrollTime_ms), Color_WHITE);
		} else {
			state = waiting;
			EPCTL_RequestData(Endpoint_Notification);
		}

	}
}

void Notifications_DrawRect(int16_t y, Color_t color){
	GR_FillRect(screen, (Rect_t) { (Point_t) {0, y}, screen->width, screen->height}, Color_DARK_GREY);
	GR_FillRect(screen, (Rect_t) { (Point_t) {0, y+screen->height-8}, screen->width, 8}, Color_BLACK);
	GR_FillRect(screen, (Rect_t) { (Point_t) {80-20, y+screen->height-5}, 40, 2}, Color_DARK_GREY);
	GR_DrawString(screen, (Point_t) {0, y}, str, &font_ArialNarrow22px, color);
}

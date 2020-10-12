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
static char from[50];

static enum {waiting, displaying} state;

void Notifications_Draw(){
	Notifications_DrawConn();

	time_ms_t elapsed;
	switch(state){
	case waiting:
		if(!EPCTL_PayloadReceived(Endpoint_Notification))
			return;

		while(GetResource(Res_Bluetooth) != E_OK);

		state = displaying;
		start = SYS_getTimeMillis();

		uint8_t * prefix = "from ";

		unsigned int k;
		for(k = 0; k < sizeof(from); k++){
			if(prefix[k] == 0)
				break;
			from[k] = prefix[k];
		}

		int i;
		for(i = 0;i < Endpoint_Notification->size; i++){
			if(Endpoint_Notification->data[i] == ':'){
				from[i+k] = 0;
				break;
			}
			from[i+k] = Endpoint_Notification->data[i];
		}

		i++;

		if(Endpoint_Notification->data[i] == ' ')
			i++;

		int f;
		for(f = 0; f + i < Endpoint_Notification->size; f++){
			str[f] = Endpoint_Notification->data[i+f];
			if(Endpoint_Notification->data[i+f] == 0)
				break;
		}

		ReleaseResource(Res_Bluetooth);

		duration = f * 110;

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
			while(GetResource(Res_Bluetooth) != E_OK);
			EPCTL_RequestData(Endpoint_Notification);
			ReleaseResource(Res_Bluetooth);
		}

	}
}

void Notifications_DrawRect(int16_t y, Color_t color){
	GR_FillRect(screen, (Rect_t) { (Point_t) {0, y}, screen->width, screen->height}, Color_DARK_GREY);
	GR_FillRect(screen, (Rect_t) { (Point_t) {0, y+screen->height-8}, screen->width, 8}, Color_BLACK);
	GR_FillRect(screen, (Rect_t) { (Point_t) {80-20, y+screen->height-5}, 40, 2}, Color_DARK_GREY);

	GR_DrawBitmap(screen, (Point_t) {8, y+2}, &bitmap_TelegramLogo);
	GR_DrawString_noWrap(screen, (Point_t) {35, y - 1}, from, &font_RobotoBoldCondensed18px, Color_LIGHT_GREY);
	GR_DrawString(screen, (Point_t) {0, y + 18}, str, &font_RobotoLight18px, color);
}


void Notifications_DrawConn(){
	if(Ping_isConnected()){
		GR_DrawBitmap(screen, (Point_t) {screen->width-16 , 0}, &bitmap_Bluetooth);
	} else {
		GR_DrawBitmap(screen, (Point_t) {screen->width-16 , 0}, &bitmap_Redcross);
	}
}

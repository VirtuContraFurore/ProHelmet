/*
 * brightness.c
 *
 *  Created on: Oct 11, 2020
 *      Author: luca
 */

#include "ViewBrightness.h"

int16_t Brightness_adjust = BRG_MEAN;
int16_t brg_orig;

TASK(ViewBrightness){
	int down = 0;

	int button = 0;
	time_ms_t start;

	while(1){

		if(SYS_getButton2Status() == 1){
			if(down)
				ChainTask(VIEWBRIGHTNESS_NEXT);
		} else
			down = 1;

		if(button == 0){
			if(SYS_getButton3Status()){
				button = 3;
				start = SYS_getTimeMillis();
				brg_orig = Brightness_adjust;
			} else if (SYS_getButton4Status()){
				button = 4;
				start = SYS_getTimeMillis();
				brg_orig = Brightness_adjust;
			} else {
				button = 0;
			}
		} else {
			if(!SYS_getButton3Status() && !SYS_getButton4Status()){
				button = 0;
			}
		}

		if(button){
			time_ms_t elapsed = SYS_getTimeMillis() - start;
			elapsed /= 50;
			if(button == 3){
				Brightness_adjust = brg_orig + elapsed;
				if(Brightness_adjust > BRG_MAX)
					Brightness_adjust = BRG_MAX;
			} else {
				Brightness_adjust = brg_orig - elapsed;
				if(Brightness_adjust < BRG_MIN)
					Brightness_adjust = BRG_MIN;
			}
		}

		ViewBrightness_drawingLoop();
	}
}

void ViewBrightness_drawingLoop(){
	GR_ClearColor(screen, Color_BLACK);

	GR_DrawBitmapColorized(screen, (Point_t) {(160-54), 3}, &bitmap_Brightness_ctl, Color_GREY);

	GR_DrawString(screen, (Point_t) {5, 5}, "Brightness", &font_ArialNarrow22px, Color_WHITE);
	GR_DrawString(screen, (Point_t) {5,35}, "adjust +/-", &font_ArialNarrow22px, Color_WHITE);

	GR_DrawRect(screen, (Rect_t) {  5, 60, 150, 15}, Color_WHITE);
	GR_FillRect(screen, (Rect_t) {  5+2, 60+2, (150-4)*(Brightness_adjust-BRG_MIN)/(BRG_MAX-BRG_MIN), 15-4}, Color_GOLD);

	GR_SwapBuffers(screen);
}

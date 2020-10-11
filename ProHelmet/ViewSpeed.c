/*
 * ViewSpeed.c
 *
 *  Created on: Oct 11, 2020
 *      Author: luca
 */

#include "ViewSpeed.h"

static Sensor_accel_data_t accel;
//static Sensor_gyro_data_t gyro;

TASK(ViewSpeed){
	int down = 0;
	while(1){
		if(SYS_getButton2Status() == 1){
			if(down)
				ChainTask(VIEWSPEED_NEXT);
		} else
			down = 1;

		accel = Sensor_getAccel();
//		gyro = Sensor_getGyro();

		ViewSpeed_drawingLoop();
	}
}

void ViewSpeed_drawingLoop(){
	GR_ClearColor(screen, Color_BLACK);
	ViewSpeed_drawbackground();

	ViewSpeed_drawspeed((Point_t) {160-98,16}, 157, Color_WHITE);

	GR_DrawBitmapColorized(screen, (Point_t) {0, +3}, &bitmap_Smallclock, Color_WHITE);
	ViewSpeed_drawTime((Point_t) {16, -6}, Color_WHITE);

	GR_SwapBuffers(screen);
}

void ViewSpeed_drawTime(Point_t pos, Color_t color){
	uint8_t min = Time_getMinutes();
	uint8_t hrs = Time_getHours();

	int16_t min_l = min % 10;
	int16_t min_h = (min / 10) % 10;

	int16_t hrs_l = hrs % 10;
	int16_t hrs_h = (hrs / 10) % 10;


	if(hrs_h)
		GR_DrawNumber(screen, (Point_t) {pos.x, pos.y}, hrs_h, 1, &font_MonoTypewriter22px, color);
	else
		pos.x -= 10;
	GR_DrawNumber(screen, (Point_t) {pos.x+=10, pos.y}, hrs_l, 1, &font_MonoTypewriter22px, color);
	if((SYS_getTimeMillis() % 1000) > 500)
		GR_DrawString(screen, (Point_t) {pos.x+=10, pos.y}, ":", &font_MonoTypewriter22px, color);
	else
		pos.x += 10;
	GR_DrawNumber(screen, (Point_t) {pos.x+=10, pos.y}, min_h, 1, &font_MonoTypewriter22px, color);
	GR_DrawNumber(screen, (Point_t) {pos.x+=10, pos.y}, min_l, 1, &font_MonoTypewriter22px, color);

}

void ViewSpeed_drawbackground(){
	Color_t bar_color;
	int16_t val = accel.value.x * 2;
	if(val > 0){
		uint8_t blend = (val > 0xFF) ? 0xFF : val;
		bar_color = GR_Blend(Color_RED, Color_BLUE, blend);
	} else {
		uint8_t blend = (-val > 0xFF) ? 0xFF : -val;
		bar_color = GR_Blend(Color_GREEN, Color_BLUE, blend);
	}
	GR_DrawBitmapColorized(screen, (Point_t) {0, 0}, &bitmap_Speedback, bar_color);
}

void ViewSpeed_drawspeed(Point_t pos, uint16_t speed, Color_t color){
	uint8_t digits[3];

	for(uint32_t i = 0; i < sizeof(digits); i++){
		digits[i] = speed % 10;
		speed /= 10;
	}
	if(digits[2])
		GR_DrawNumber(screen, (Point_t) {pos.x   , pos.y}, digits[2], 1, &font_MonoTypewriter60px, color);
	if(digits[1])
		GR_DrawNumber(screen, (Point_t) {pos.x+23   , pos.y}, digits[1], 1, &font_MonoTypewriter60px, color);
	if(digits[0])
		GR_DrawNumber(screen, (Point_t) {pos.x+53   , pos.y}, digits[0], 1, &font_MonoTypewriter60px, color);
}

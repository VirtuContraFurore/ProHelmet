/*
 * ViewBoot.c
 *
 *  Created on: Sep 28, 2020
 *      Author: luca
 */

#include "ViewBoot.h"

#define CALIB_TICKS 100

static int16_t y;

TASK(ViewBoot){
	__PRINT_DEBUG_STR("Booting!\n");

	/* Ping Bluetooth */
	Ping_start();

	/**
	 * TODO: Prima mostriamo uno splash screen!!!
	 * Qualcosa di figo...
	 */

	/* Draw booting diagnostics */
	draw_boot_diagnostics(VIEWBOOT_ms);

	/* Accelerometer calibration */
	draw_sensor_calib(CALIB_ms);

	draw_done(DONE_ms);

	ChainTask(ViewSpeed);
}

static void draw_done(time_ms_t duration_ms){
	time_ms_t start = SYS_getTimeMillis();
	while(1){
		time_ms_t elapsed = SYS_getTimeMillis()-start;
		float f = screen->width - screen->width*elapsed/duration_ms;

		OLED_Clear(screen, Color_BLACK);
		GR_DrawString_noWrap(screen, (Point_t) {4 + f, 5}, "Sensors", &font_ArialNarrow36px, Color_LIGHT_BLUE);
		GR_DrawString_noWrap(screen, (Point_t) {4 - f, 40}, "Calibrated!", &font_ArialNarrow36px, Color_LIGHT_BLUE);
		GR_SwapBuffers(screen);

		if(elapsed >= duration_ms)
			break;
	}

	__delay_ms(100);

	OLED_Clear(screen, Color_BLACK);
	GR_DrawString_noWrap(screen, (Point_t) {4, 5}, "Sensors", &font_ArialNarrow36px, Color_WHITE);
	GR_DrawString_noWrap(screen, (Point_t) {4, 40}, "Calibrated!", &font_ArialNarrow36px, Color_WHITE);
	GR_SwapBuffers(screen);

	__delay_ms(200);

	OLED_Clear(screen, Color_BLACK);
	GR_DrawString_noWrap(screen, (Point_t) {4, 5}, "Sensors", &font_ArialNarrow36px, Color_LIGHT_BLUE);
	GR_DrawString_noWrap(screen, (Point_t) {4, 40}, "Calibrated!", &font_ArialNarrow36px, Color_LIGHT_BLUE);
	GR_SwapBuffers(screen);

	__delay_ms(300);
}

static void draw_str(uint8_t * str){
	GR_DrawString(screen, (Point_t) {0, y}, str, &font_MonoTypewriter12px, Color_WHITE);
	y += font_MonoTypewriter12px.size;
}

static void draw_sensor_calib(time_ms_t duration_ms){
	time_ms_t start = SYS_getTimeMillis();

	int32_t press, temp;
	int16_t x1,y1,z1;
	int16_t x2,y2,z2;
	int32_t ticks=0;

	Sensor_startTasks();

	while(1){

		Sensor_accel_data_t accel = Sensor_getAccel();
		x1 = accel.value.x;
		y1 = accel.value.y;
		z1 = accel.value.z;

		Sensor_gyro_data_t gyro = Sensor_getGyro();
		x2 = gyro.value.x;
		y2 = gyro.value.y;
		z2 = gyro.value.z;

		Sensor_press_data_t pres = Sensor_getPress();
		press = pres.press;
		temp = pres.temp;

		time_ms_t elapsed = SYS_getTimeMillis()-start;
		y=0;
		OLED_Clear(screen, Color_BLACK);

		draw_str("Calibrating sensors...");

		if(elapsed > 400){
			GR_DrawNumber(screen, (Point_t){75, y}, temp/10, 2, &font_MonoTypewriter12px, Color_GREEN);
			GR_DrawString(screen, (Point_t){75+15, y}, ".", &font_MonoTypewriter12px, Color_GREEN);
			GR_DrawNumber(screen, (Point_t){75+22, y}, temp%10, 1, &font_MonoTypewriter12px, Color_GREEN);
			GR_DrawString(screen, (Point_t){75+30, y}, "C", &font_MonoTypewriter12px, Color_GREEN);
			draw_str("Ext temp:");
		}

		if(elapsed > 800){
			GR_DrawNumber(screen, (Point_t){75, y}, press/100, 4, &font_MonoTypewriter12px, Color_LIGHT_BLUE);
			GR_DrawString(screen, (Point_t){75+40, y}, "hPa", &font_MonoTypewriter12px, Color_LIGHT_BLUE);
			draw_str("Ext press:");
		}

		if(elapsed > 1200){

			GR_DrawString(screen, (Point_t){85, y}, "g", &font_MonoTypewriter22px, Color_WHITE);
			GR_DrawNumber(screen, (Point_t){75, y}, x1, 8, &font_MonoTypewriter12px, Color_ORANGE);
			GR_DrawNumber(screen, (Point_t){75, y+10}, y1, 8, &font_MonoTypewriter12px, Color_ORANGE);
			GR_DrawNumber(screen, (Point_t){75, y+20}, z1, 8, &font_MonoTypewriter12px, Color_ORANGE);


			GR_DrawString(screen, (Point_t){15, y}, "w", &font_MonoTypewriter22px, Color_WHITE);
			GR_DrawNumber(screen, (Point_t){5, y}, x2, 8, &font_MonoTypewriter12px, Color_ORANGE);
			GR_DrawNumber(screen, (Point_t){5, y+10}, y2, 8, &font_MonoTypewriter12px, Color_ORANGE);
			GR_DrawNumber(screen, (Point_t){5, y+20}, z2, 8, &font_MonoTypewriter12px, Color_ORANGE);
		}

		GR_DrawRect(screen, (Rect_t) {  5, 69, 150, 6}, Color_WHITE);
		GR_FillRect(screen, (Rect_t) {  5+2, 69+2, ((150-4)*(SYS_time_millis - start)/duration_ms), 6-4}, Color_PURPLE);

		GR_SwapBuffers(screen);

		if(elapsed > duration_ms && accel.calibration_ongoing == 0)
			break;
	}
}

static void draw_boot_diagnostics(time_ms_t duration_ms){
	time_ms_t start = SYS_getTimeMillis();

	while(1){
		time_ms_t elapsed = SYS_getTimeMillis()-start;
		y=0;
		OLED_Clear(screen, Color_BLACK);

		if(elapsed > 400){
			GR_DrawBitmap(screen, (Point_t) {160-26,0}, &bitmap_BootGear);
			draw_str("SYS Diagnostics");
		}

		if(elapsed > 800)
			draw_str("Core clock: 180Mhz");

		if(elapsed > 1200){
			uint8_t btn_str[] = "Button test: Up Up Up";
			if(SYS_getButton2Status() == 1){
				btn_str[13] = 'D';
				btn_str[14] = 'n';
			}
			if(SYS_getButton3Status() == 1){
				btn_str[16] = 'D';
				btn_str[17] = 'n';
			}
			if(SYS_getButton4Status() == 1){
				btn_str[19] = 'D';
				btn_str[20] = 'n';
			}
			draw_str(btn_str);
		}

		if(elapsed > 1600){
			uint8_t pt_str [] = "Photores test:";
			GR_DrawNumber(screen, (Point_t) {sizeof(pt_str)*8, y}, SYS_PhotoresGetVal(), 2, &font_MonoTypewriter12px, Color_WHITE);
			draw_str(pt_str);
		}


		if(elapsed > 2000){
			if(EPCTL_PayloadReceived(Endpoint_Ping))
				draw_str("BT ping received!");
			else {
				switch((elapsed / 100) % 4){
				case 0:
					draw_str("BT ping: waiting");
					break;
				case 1:
					draw_str("BT ping: waiting.");
					break;
				case 2:
					draw_str("BT ping: waiting..");
					break;
				case 3:
					draw_str("BT ping: waiting...");
				}
			}
		}


		GR_DrawRect(screen, (Rect_t) {  5, 65, 150, 10}, Color_WHITE);
		GR_FillRect(screen, (Rect_t) {  5+2, 65+2, ((150-4)*(SYS_time_millis - start)/duration_ms), 10-4}, Color_ORANGE);

		GR_SwapBuffers(screen);

		if(elapsed > duration_ms)
			break;
	};
}

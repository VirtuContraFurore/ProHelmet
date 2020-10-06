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

	SYS_InitGreenLed();
	SYS_InitSysTimer();
	SYS_InitSideButtons();
	SYS_InitPhotores();
	BT_Init(BT_BAUDRATE, BT_PARITY, BT_STOPBITS);
	GY80_Init_i2c(400);
	BMP085_Init();
	ADXL345_Init();
	L3G4200D_Init();

	OLED_InitScreen(screen, OLED_SOUTH);

	/* Ping Bluetooth */
	EPCTL_RequestData(Endpoint_Ping);

	/**
	 * TODO: Prima mostriamo uno splash screen!!!
	 * Qualcosa di figo...
	 */

	/* Draw booting diagnostics */
	draw_boot_diagnostics(VIEWBOOT_ms);

	/* Accelerometer calibration */
	draw_sensor_calib();
}

static void draw_str(uint8_t * str){
	GR_DrawString(screen, (Point_t) {0, y}, str, &font_MonoTypewriter12px, Color_WHITE);
	y += font_MonoTypewriter12px.size;
}

static void draw_sensor_calib(){
	time_ms_t start = SYS_getTimeMillis();

	int32_t press, temp;
	BMP085_Test(&temp, &press);
	int16_t x1,y1,z1;
	int16_t x2,y2,z2;
	int64_t gcx=0, gcy=0, gcz=0;
	int64_t acx=0, acy=0, acz=0;
	int32_t ticks=0;

	while(1){
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
		GR_FillRect(screen, (Rect_t) {  5+2, 69+2, ((150-4)*(ticks)/(CALIB_TICKS)), 6-4}, Color_PURPLE);

		OLED_SwapBuffers(screen);

		__delay_ms(15);
		ADXL345_ReadAccel(&x1, &y1, &z1);
		L3G4200D_ReadOmega(&x2, &y2, &z2);

		if(ticks < CALIB_TICKS){
			gcx+=x2;
			gcy+=y2;
			gcz+=z2;
			acx+=x1;
			acy+=y1;
			acz+=z1;
			ticks++;
		} else {
			x2 -= gcx/ticks;
			y2 -= gcy/ticks;
			z2 -= gcz/ticks;
			x1 -= acx/ticks;
			y1 -= acy/ticks;
			z1 -= acz/ticks;
		}

	}
}

static void draw_boot_diagnostics(uint32_t duration_ms){
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
		GR_FillRect(screen, (Rect_t) {  5+2, 65+2, ((150-4)*(SYS_time_millis - start)/VIEWBOOT_ms), 10-4}, Color_ORANGE);

		OLED_SwapBuffers(screen);

		if(elapsed > duration_ms)
			break;
	};
}

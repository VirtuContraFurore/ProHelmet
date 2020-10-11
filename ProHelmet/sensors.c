/*
 * sensors.c
 *
 *  Created on: Oct 10, 2020
 *      Author: luca
 */

#include "sensors.h"
#include "ee.h"
#include "driver/oled.h"

static int init = 0;

static Sensor_accel_data_t Sensor_accel;
static Sensor_gyro_data_t Sensor_gyro;
static Sensor_press_data_t Sensor_press;
static uint16_t	light_data;

void Sensor_init(){
	GY80_Init_i2c(400);
	BMP085_Init();
	ADXL345_Init();
	L3G4200D_Init();
	init = 1;
}

void Sensor_startTasks(){
	if(init != 1)
		Sensor_init();

	Sensor_accel.calibration_ongoing = SENSOR_CALIBRATION_TICKS(ACCEL);

	SetRelAlarm(AccelAlarm, 	 5, 	SENSOR_ACCEL_PERIOD_ms);
	SetRelAlarm(GyroAlarm, 		25, 	SENSOR_GYRO_PERIOD_ms);
	SetRelAlarm(LightAlarm,		45,		SENSOR_LIGHT_PERIOD_ms);
	SetRelAlarm(PressAlarm,     65,     SENSOR_PRESS_PERIOD_ms);
}

Sensor_accel_data_t Sensor_getAccel(){
	Sensor_accel_data_t ret;
	GetResource(Res_AccelData);
	ret = Sensor_accel;
	ReleaseResource(Res_AccelData);
	return ret;
}

Sensor_gyro_data_t Sensor_getGyro(){
	Sensor_gyro_data_t ret;
	GetResource(Res_GyroData);
	ret = Sensor_gyro;
	ReleaseResource(Res_GyroData);
	return ret;
}

Sensor_press_data_t Sensor_getPress(){
	Sensor_press_data_t ret;
	GetResource(Res_PressData);
	ret = Sensor_press;
	ReleaseResource(Res_PressData);
	return ret;
}

uint16_t Sensor_getLight(){
	return light_data;
}

/*
 * Calibration accumulator for accelerometer
 */
static int32_t cal_x = 0, cal_y = 0, cal_z = 0;

TASK(AccelSensor){
	int16_t x,y,z;

	GetResource(Res_I2c2);
	ADXL345_ReadAccel(&x, &y, &z);
	ReleaseResource(Res_I2c2);

	GetResource(Res_AccelData);
	Sensor_accel.value.x = x;
	Sensor_accel.value.y = y;
	Sensor_accel.value.z = z;
	if(Sensor_accel.calibration_ongoing > 0){
		Sensor_accel.calibration_ongoing--;
		cal_x += x;
		cal_y += y;
		cal_z += z;
		if(Sensor_accel.calibration_ongoing == 0){
			Sensor_accel.calib.x = cal_x / SENSOR_CALIBRATION_TICKS(ACCEL);
			Sensor_accel.calib.y = cal_y / SENSOR_CALIBRATION_TICKS(ACCEL);
			Sensor_accel.calib.z = cal_z / SENSOR_CALIBRATION_TICKS(ACCEL);
		}
	}
	ReleaseResource(Res_AccelData);
}

TASK(GyroSensor){
	int16_t x,y,z;

	GetResource(Res_I2c2);
	L3G4200D_ReadOmega(&x, &y, &z);
	ReleaseResource(Res_I2c2);

	GetResource(Res_GyroData);
	Sensor_gyro.value.x = x;
	Sensor_gyro.value.y = y;
	Sensor_gyro.value.z = z;
	ReleaseResource(Res_GyroData);
}

TASK(PressSensor){
	int32_t p, t;

	GetResource(Res_I2c2);
	BMP085_Test(&t, &p);
	ReleaseResource(Res_I2c2);

	GetResource(Res_PressData);
	Sensor_press.press = p;
	Sensor_press.temp = t;
	ReleaseResource(Res_PressData);
}

TASK(LightSensor){
	light_data = SYS_PhotoresGetVal();

	float f = light_data / 64;

#define off_thres	20

	int16_t backlight = off_thres + (OLED_BACKLIGHT_MAX-off_thres) * light_data / 64;

	OLED_SetBacklight(screen, backlight);
}

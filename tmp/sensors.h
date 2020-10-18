/*
 * sensors.h
 *
 *  Created on: Oct 10, 2020
 *      Author: luca
 */

#ifndef SENSORS_H_
#define SENSORS_H_

#include "utils.h"
#include "system.h"

#include "driver/adxl345.h"
#include "driver/l3g4200d.h"
#include "driver/bmp085.h"

#define SENSOR_ACCEL_PERIOD_ms		 100
#define SENSOR_GYRO_PERIOD_ms		 100
#define SENSOR_PRESS_PERIOD_ms		1000
#define SENSOR_LIGHT_PERIOD_ms		 300

#define SENSOR_ACCEL_CALIBRATION_TIME_ms 1500

#define SENSOR_CALIBRATION_TICKS(Sensor) (SENSOR_##Sensor##_CALIBRATION_TIME_ms/SENSOR_##Sensor##_PERIOD_ms)

typedef struct Vec3_16 {
	int16_t x;
	int16_t y;
	int16_t z;
} Vec3_16_t;

typedef struct Vec3_32 {
	int32_t x;
	int32_t y;
	int32_t z;
} Vec3_32_t;

typedef struct Sensor_accel_data {
	Vec3_16_t value;
	Vec3_16_t calib;
	float calib_xy_mod;
	float a;
	int16_t calibration_ongoing;
} Sensor_accel_data_t;

typedef struct Sensor_gyro_data {
	Vec3_16_t value;
} Sensor_gyro_data_t;

typedef struct Sensor_press_data {
	int32_t temp;
	int32_t press;
} Sensor_press_data_t;


/*
 * Initialize sensors
 */
void Sensor_init();

/*
 * Enable polling tasks
 */
void Sensor_startTasks();

Sensor_gyro_data_t Sensor_getGyro();

Sensor_accel_data_t Sensor_getAccel();

Sensor_press_data_t Sensor_getPress();

uint16_t Sensor_getLight();

#endif /* SENSORS_H_ */

/*
 * Speed.c
 *
 *  Created on: Oct 13, 2020
 *      Author: luca
 */

#include "Speed.h"

#include "system.h"
#include "sensors.h"
#include "driver/bluetooth_endpoints.h"

/*
 * Calculated speed value, MSB is 0.1 m/s
 */
static float speed = 0;
static float max_breaking = 7.0f; /* breaking force */
static float acceleration;
static float gps_speed = 0;
static uint8_t speed_gps_read;
static time_ms_t start;

/*
 * Use to calculate acceleration along the head direction point "to the front"
 */
static float getSpeedInc(Sensor_accel_data_t accel, time_ms_t elapsed);

TASK(SpeedMeasurment){
	/*
	 * Accelerometer reading
	 */
	Sensor_accel_data_t accel = Sensor_getAccel();

	/*
	 * Obtain data from GPS
	 */
	if(EPCTL_PayloadReceived(Endpoint_Speed)){
		union { float val; uint8_t data[4]; } gps_data;

		for(int i = 0; i < 4; i++)
			gps_data.data[i] = Endpoint_Speed->data[i];

		gps_speed = gps_data.val;
		speed_gps_read = 1;
	} else {
		speed_gps_read = 0;
	}

	/*
	 * Request fresh GPS data for the next time
	 */
	if(GetResource(Res_Bluetooth) == E_OK){
		EPCTL_RequestData(Endpoint_Speed);
		ReleaseResource(Res_Bluetooth);
	}

	time_ms_t elapsed = SYS_getTimeMillis() - start;

	float incr = getSpeedInc(accel, elapsed);

	/*
	 * Ignore gps singla until the braking / accelerating force ends
	 */
	if(speed_gps_read && (acceleration < 3.0f))
		speed = gps_speed;

	speed += incr;
	if(speed < 0)
		speed = 0;

	start = SYS_getTimeMillis();
}

void Speed_start(){
	start = SYS_getTimeMillis();
	speed = 0;
	SetRelAlarm(SpeedAlarm, SPEED_PERIOD_ms, SPEED_PERIOD_ms);
}

int32_t Speed_get_ms(){
	return (int32_t) (speed);
}

int32_t Speed_get_kmh(){
	return (int32_t) (speed * 3.6);
}

static float getSpeedInc(Sensor_accel_data_t accel, time_ms_t elapsed){
	/*
	 * Current scale: 255 = 1g = 9.81 m/s^2
	 */
	float a = ((accel.value.x-accel.calib.x) *( + accel.calib.y) + (accel.value.y-accel.calib.y) * ( - accel.calib.x));
	a /= accel.calib_xy_mod;
	a = a * 9.81f / 255;

	if(a < 0 && (-a) > max_breaking)
		max_breaking = a;

	acceleration = a;

	float delta_speed = a * elapsed / 1000;

	if(fabs(delta_speed) < 0.5f)
		return 0;


	return delta_speed;
}

int32_t Speed_getBrakingDistance(){
	float distance = 0;

	distance += SPEED_REACTION_TIME_ms / 1000 * speed;

	distance += speed * speed / (2 * max_breaking);

	return (int32_t) (distance);
}

float Speed_getAcceleration(){
	return acceleration;
}

/*
 * time.c
 *
 *  Created on: Oct 11, 2020
 *      Author: luca
 */

#ifndef TIME_C_
#define TIME_C_

#include "time.h"

#include "ee.h"

#include "driver/bluetooth_endpoints.h"

static uint8_t init = 0;

static int8_t hours, minutes;

void Time_init(){
	while(GetResource(Res_Bluetooth) != E_OK);
	EPCTL_RequestData(Endpoint_Time);
	uint32_t ret = TIME_WAIT_TIMEOUT_wait_state;
	while(!EPCTL_PayloadReceived(Endpoint_Time)){
		ret--;
		if(ret == 0) {
			ReleaseResource(Res_Bluetooth);
			return;
		}
	}
	minutes = Endpoint_Time->data[0];
	hours = Endpoint_Time->data[1];
	ReleaseResource(Res_Bluetooth);

	init = 1;
	SetRelAlarm(TimeAlarm, 60000, 60000);
}

int8_t Time_getHours(){
	if(!init)
		Time_init();

	while(GetResource(Res_TimeData) != E_OK);
	uint8_t hours2 = hours;
	ReleaseResource(Res_TimeData);

	return hours2;
}

int8_t Time_getMinutes(){
	if(!init)
		Time_init();

	while(GetResource(Res_TimeData) != E_OK);
	uint8_t minutes2 = minutes;
	ReleaseResource(Res_TimeData);

	return minutes2;
}

static int reset = TIME_RESYNCH_mins;

TASK(IncrTime){
	while(GetResource(Res_TimeData) != E_OK);

	if(reset-- <= 0){
		init = 0;
		reset = TIME_RESYNCH_mins;
		// force a resynch
	}

	minutes++;
	if(minutes > 59){
		hours++;
		minutes = 0;
		if(hours > 23)
			hours = 0;
	}

	ReleaseResource(Res_TimeData);
}

#endif /* TIME_C_ */

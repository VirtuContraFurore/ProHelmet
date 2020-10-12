/*
 * PingBluetooth.c
 *
 *  Created on: Oct 12, 2020
 *      Author: luca
 */

#include "PingBluetooth.h"

static volatile uint8_t connected = 0;

TASK(PingBluetooth){
	if(GetResource(Res_Bluetooth) == E_OK){
		if(EPCTL_PayloadReceived(Endpoint_Ping))
			connected = 1;
		else
			connected = 0;

		EPCTL_RequestData(Endpoint_Ping);

		ReleaseResource(Res_Bluetooth);
	} else {
		CancelAlarm(PingAlarm);
		SetRelAlarm(PingAlarm, 500, PING_PERIOD_ms);
	}
}

void Ping_start(){
	SetRelAlarm(PingAlarm, 500, PING_PERIOD_ms);
}

uint8_t Ping_isConnected(){
	return connected;
}

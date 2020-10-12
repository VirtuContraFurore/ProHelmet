/*
 * PingBluetooth.h
 *
 *  Created on: Oct 12, 2020
 *      Author: luca
 */

#ifndef PINGBLUETOOTH_H_
#define PINGBLUETOOTH_H_

#include "ee.h"
#include "utils.h"
#include "driver/bluetooth_endpoints.h"

#define PING_PERIOD_ms 2500

uint8_t Ping_isConnected();

void Ping_start();

#endif /* PINGBLUETOOTH_H_ */

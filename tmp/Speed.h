/*
 * Speed.h
 *
 *  Created on: Oct 13, 2020
 *      Author: luca
 */

#ifndef SPEED_H_
#define SPEED_H_

#define SPEED_PERIOD_ms 250


#define SPEED_1G_ms 	9.81
#define SPEED_1g		981   // 0.1 ms^2

#define SPEED_REACTION_TIME_ms 1200

#include "ee.h"
#include "utils.h"

/*
 * Start speed calculation in background
 */
void Speed_start();

/*
 * returns computed value
 */
int32_t Speed_get_ms();

int32_t Speed_get_kmh();

/*
 * Get distance to travel to a complete stop
 */
int32_t Speed_getBrakingDistance();

float Speed_getAcceleration();

#endif /* SPEED_H_ */

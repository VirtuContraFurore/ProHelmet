/*
 * time.h
 *
 *  Created on: Oct 11, 2020
 *      Author: luca
 */

#ifndef TIME_H_
#define TIME_H_

#include "utils.h"

#define TIME_WAIT_TIMEOUT_wait_state	10000

void Time_init();

int8_t Time_getHours();

int8_t Time_getMinutes();

#endif /* TIME_H_ */

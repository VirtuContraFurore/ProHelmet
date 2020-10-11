/*
 * Notifications.h
 *
 *  Created on: Oct 11, 2020
 *      Author: luca
 */

#ifndef NOTIFICATIONS_H_
#define NOTIFICATIONS_H_

#include "utils.h"
#include "system.h"

#include "driver/graphics.h"
#include "driver/bluetooth_endpoints.h"

#include "assets/fonts/MonoTypewriter22.h"
#include "assets/fonts/ArialNarrow22.h"

#define Not_scrollTime_ms 800

/**
 * Draw over-screen notifications
 */
void Notifications_Draw();

void Notifications_DrawRect(int16_t y, Color_t color);

#endif /* NOTIFICATIONS_H_ */

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
#include "PingBluetooth.h"

#include "driver/graphics.h"
#include "driver/bluetooth_endpoints.h"

#include "assets/fonts/MonoTypewriter22.h"
#include "assets/fonts/ArialNarrow22.h"
#include "assets/fonts/ArialNarrow18.h"
#include "assets/fonts/RobotoThin18.h"
#include "assets/fonts/RobotoLight18.h"
#include "assets/fonts/RobotoCondensed18.h"
#include "assets/fonts/RobotoBoldCondensed18.h"

#include "assets/images/TelegramLogo.h"
#include "assets/images/Greentick.h"
#include "assets/images/Redcross.h"
#include "assets/images/Bluetooth.h"
#include "assets/images/PhoneCall.h"
#include "assets/images/Info.h"


#define Not_scrollTime_ms 800

/**
 * Draw over-screen notifications
 */
void Notifications_Draw();

void Notifications_DrawConn();

void Notifications_DrawRect(int16_t y, Color_t color);

#endif /* NOTIFICATIONS_H_ */

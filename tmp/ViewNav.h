/*
 * ViewNav.h
 *
 *  Created on: Oct 11, 2020
 *      Author: luca
 */

#ifndef VIEWNAV_H_
#define VIEWNAV_H_

#include "ee.h"
#include "utils.h"
#include "system.h"
#include "Notifications.h"
#include "sensors.h"

#include "driver/graphics.h"
#include "driver/bluetooth_endpoints.h"

#include "assets/fonts/ArialNarrow22.h"
#include "assets/fonts/ArialNarrow36.h"
#include "assets/fonts/RobotoCondensed18.h"
#include "assets/fonts/MonoTypewriter22.h"
#include "assets/fonts/MonoTypewriter12.h"

#include "assets/images/roundabout_2l.h"
#include "assets/images/ExitRight.h"

#define VIEWNAV_NEXT ViewBrightness

static struct Bitmap * const signals[]= {&bitmap_roundabout_2l, &bitmap_ExitRight};

void ViewNav_drawingLoop();
void ViewNav_drawNav();
void ViewNav_drawTemp();

#endif /* VIEWNAV_H_ */

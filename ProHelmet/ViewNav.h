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

#include "driver/graphics.h"
#include "driver/bluetooth_endpoints.h"

#include "assets/fonts/ArialNarrow22.h"
#include "assets/fonts/ArialNarrow36.h"
#include "assets/fonts/RobotoCondensed18.h"
#include "assets/fonts/MonoTypewriter22.h"

#include "assets/images/roundabout_2l.h"

#define VIEWNAV_NEXT ViewBrightness

static struct Bitmap * const signals[]= {&bitmap_roundabout_2l};

void ViewNav_drawingLoop();
void ViewNav_drawNav();

#endif /* VIEWNAV_H_ */

/*
 * ViewSpeed.h
 *
 *  Created on: Oct 11, 2020
 *      Author: luca
 */

#ifndef VIEWSPEED_H_
#define VIEWSPEED_H_

#include "ee.h"

#include "utils.h"
#include "system.h"
#include "sensors.h"
#include "time.h"
#include "Speed.h"

#include "driver/graphics.h"

#include "assets/fonts/ArialNarrow22.h"
#include "assets/fonts/MonoTypewriter22.h"
#include "assets/fonts/MonoTypewriter60.h"

#include "assets/images/Speedback.h"
#include "assets/images/Smallclock.h"

#define VIEWSPEED_NEXT 		ViewNav

void ViewSpeed_drawingLoop();
void ViewSpeed_drawbackground();
void ViewSpeed_drawspeed(Point_t pos, uint16_t speed, Color_t color);
void ViewSpeed_drawTime(Point_t pos, Color_t color);
void ViewSpeed_drawStopDistance(Point_t pos, uint32_t distance, Color_t color);


#endif /* VIEWSPEED_H_ */

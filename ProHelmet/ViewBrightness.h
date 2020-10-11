/*
 * ViewBrightness.h
 *
 *  Created on: Oct 11, 2020
 *      Author: luca
 */

#ifndef WIDGETS_VIEWBRIGHTNESS_H_
#define WIDGETS_VIEWBRIGHTNESS_H_

#include "ee.h"
#include "utils.h"
#include "system.h"

#include "driver/graphics.h"

#include "assets/fonts/ArialNarrow22.h"
#include "assets/images/Brightness_ctl.h"

#define BRG_MAX 	50
#define BRG_MIN   	-50
#define BRG_MEAN ((BRG_MAX+BRG_MIN)/2)

#define VIEWBRIGHTNESS_NEXT ViewSpeed

void ViewBrightness_drawingLoop();

#endif /* WIDGETS_VIEWBRIGHTNESS_H_ */

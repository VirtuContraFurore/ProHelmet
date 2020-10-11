/*
 * ViewBoot.h
 *
 *  Created on: Sep 28, 2020
 *      Author: luca
 */

#ifndef VIEWBOOT_H_
#define VIEWBOOT_H_

#include "ee.h"
#include "utils.h"
#include "system.h"

#include "driver/graphics.h"
#include "driver/bluetooth.h"
#include "driver/bluetooth_endpoints.h"
#include "driver/gy80.h"
#include "driver/bmp085.h"
#include "driver/adxl345.h"
#include "driver/l3g4200d.h"

#include "sensors.h"

#include "assets/fonts/MonoTypewriter12.h"
#include "assets/fonts/MonoTypewriter22.h"
#include "assets/fonts/ArialNarrow36.h"
#include "assets/images/BootGear.h"

#define VIEWBOOT_ms 	5000
#define CALIB_ms		2500
#define DONE_ms			800

static void draw_str(uint8_t * str);
static void draw_boot_diagnostics(time_ms_t duration_ms);
static void draw_sensor_calib(time_ms_t duration_ms);
static void draw_done(time_ms_t duration_ms);

#endif /* VIEWBOOT_H_ */

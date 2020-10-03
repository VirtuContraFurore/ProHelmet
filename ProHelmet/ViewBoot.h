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

#include "assets/fonts/MonoTypewriter12.h"
#include "assets/fonts/MonoTypewriter22.h"
#include "assets/images/BootGear.h"

#define VIEWBOOT_ms 	5000

static void draw_str(uint8_t * str);
static void draw_boot_diagnostics(uint32_t duration_ms);
static void draw_sensor_calib();

#endif /* VIEWBOOT_H_ */

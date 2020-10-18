/*
 * bmp085.h
 *
 *  Created on: Sep 30, 2020
 *      Author: luca
 */

#ifndef DRIVER_BMP085_H_
#define DRIVER_BMP085_H_

#include "gy80.h"
#include <math.h>

#define BMP085_ADDR				0xEE

#define BMP085_CALIB_ADDR		0xAA

#define BMP085_MSB_REG			0xF6
#define BMP085_LSB_REG			0xF7
#define BMP085_XSB_REG			0xF8

#define BMP085_CTRL_REG		 	0xF4
#define BMP085_CTRL_TEMP			0x2E
#define BMP085_CTRL_PRESS			0x34
#define BMP085_CTRL_PRESS_OV(ov)		((BMP085_CTRL_PRESS | ((ov) << 6)) & 0xFF)
#define BMP085_CTRL_PRESS_LOWPWR		BMP085_CTRL_PRESS_OV(0)
#define BMP085_CTRL_PRESS_STANDARD		BMP085_CTRL_PRESS_OV(1)
#define BMP085_CTRL_PRESS_HIGH_RES		BMP085_CTRL_PRESS_OV(2)
#define BMP085_CTRL_PRESS_ULTRA_RES		BMP085_CTRL_PRESS_OV(3)
#define BMP085_CTRL_TEMP_ms			 		15
#define BMP085_CTRL_PRESS_OV_ms(ov)			(20*(ov)+6)
#define BMP085_CTRL_PRESS_LOWPWR_ms		 	BMP085_CTRL_PRESS_OV_ms(0)
#define BMP085_CTRL_PRESS_STANDARD_ms		BMP085_CTRL_PRESS_OV_ms(1)
#define BMP085_CTRL_PRESS_HIGH_RES_ms		BMP085_CTRL_PRESS_OV_ms(2)
#define BMP085_CTRL_PRESS_ULTRA_RES_ms		BMP085_CTRL_PRESS_OV_ms(3)

typedef struct __attribute__ ((packed)) BMP085_Cal {
	int16_t AC1;
	int16_t AC2;
	int16_t AC3;
	uint16_t AC4;
	uint16_t AC5;
	uint16_t AC6;
	int16_t B1;
	int16_t B2;
	int16_t MB;
	int16_t MC;
	int16_t MD;
} BMP085_Cal_t;


/*
 * Initialize temperature + pressure sensor
 */
void BMP085_Init();

/*
 * Busy wait reads a value
 * Only for testing!!!
 */
uint16_t BMP085_ReadUT();
uint32_t BMP085_ReadUP(uint8_t oss);
void BMP085_Test(int32_t * temp, int32_t * press);

/**
 * Convert temperature value
 */
int16_t BMP085_TempConversion(uint16_t read_val);

/*
 * Conver temp + press value
 */
void BMP085_Conversion(uint16_t UT, uint32_t UP, uint8_t oss, int32_t * temp, int32_t * press);

#endif /* DRIVER_BMP085_H_ */

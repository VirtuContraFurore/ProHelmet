/*
 * adxl345.h
 *
 *  Created on: Oct 1, 2020
 *      Author: luca
 */

#ifndef DRIVER_ADXL345_H_
#define DRIVER_ADXL345_H_

#include "gy80.h"

#define ADXL345_ADDR 	0xA6

#define ADXL345_REG_BWRATE			0x2C
#define ADXL345_REG_POWERCTL		0x2D
#define ADXL345_REG_DATAFORMAT		0x31
#define ADXL345_REG_DATA			0x32

/*
 * Initialize
 */
void ADXL345_Init();

/*
 * Write reg
 */
void ADXL345_WriteReg(uint8_t reg, uint8_t data);

/*
 * Read reg
 */
uint8_t ADXL345_ReadReg(uint8_t reg);

/*
 * Read acceleration data
 */
void ADXL345_ReadAccel(int16_t * x, int16_t * y, int16_t * z);

#endif /* DRIVER_ADXL345_H_ */

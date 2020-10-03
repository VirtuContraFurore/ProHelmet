/*
 * adxl345.c
 *
 *  Created on: Oct 1, 2020
 *      Author: luca
 */

#include "adxl345.h"

void ADXL345_Init(){
	/* Scale 4g, 10 bit mode, adjust right */
	ADXL345_WriteReg(ADXL345_REG_DATAFORMAT,	0b00000000);
	/* Rate 50Hz */
	ADXL345_WriteReg(ADXL345_REG_BWRATE, 		0b00001010);
	ADXL345_WriteReg(ADXL345_REG_POWERCTL, 		0b00001000);
}

void ADXL345_WriteReg(uint8_t reg, uint8_t data){
	uint8_t buf[2];
	buf[0] = reg;
	buf[1] = data;
	GY80_SendBytes(ADXL345_ADDR, buf, sizeof(buf));
}

uint8_t ADXL345_ReadReg(uint8_t reg){
	GY80_SendByte(ADXL345_ADDR, reg);
	return GY80_ReadByte(ADXL345_ADDR);
}

void ADXL345_ReadAccel(int16_t * x, int16_t * y, int16_t * z){
	uint8_t buf[6];
	GY80_SendByte(ADXL345_ADDR, ADXL345_REG_DATA);
	GY80_ReadBytes(ADXL345_ADDR, buf, sizeof(buf));
	*x = (int16_t) (buf[0] | buf[1]<<8);
	*y = (int16_t) (buf[2] | buf[3]<<8);
	*z = (int16_t) (buf[4] | buf[5]<<8);
}

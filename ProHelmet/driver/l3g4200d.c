/*
 * l3g4200d.c
 *
 *  Created on: Oct 1, 2020
 *      Author: luca
 */

#include "l3g4200d.h"

void L3G4200D_Init(){
	/* Output Data Rate = 100Hz * (bit7:bit6 + 1), Bandwidth: ??,Power mode = 1, All 3 axis enabled */
	__delay_ms(10);
	L3G4200D_WriteReg(L3G4200D_REG_CTRL1, 0b00001111);
	/* Resolution = bit5:bit4 */
	L3G4200D_WriteReg(L3G4200D_REG_CTRL4, 0b00010000);
	/* Filters */
	L3G4200D_WriteReg(L3G4200D_REG_CTRL5, 0b00000000);
}

void L3G4200D_WriteReg(uint8_t reg, uint8_t data){
	uint8_t buf[2];
	buf[0] = reg;
	buf[1] = data;
	GY80_SendBytes(L3G4200D_ADDR, buf, sizeof(buf));
}

uint8_t L3G4200D_ReadReg(uint8_t reg){
	GY80_SendByte(L3G4200D_ADDR, reg);
	return GY80_ReadByte(L3G4200D_ADDR);
}

void L3G4200D_ReadOmega(int16_t * x, int16_t * y, int16_t * z){
	uint8_t buf[6];
	GY80_SendByte(L3G4200D_ADDR, L3G4200D_REG_DATA | L3G4200D_AUTOINC);
	GY80_ReadBytes(L3G4200D_ADDR, buf, sizeof(buf));
	*x = (int16_t) (buf[0] | buf[1] << 8);
	*y = (int16_t) (buf[2] | buf[3] << 8);
	*z = (int16_t) (buf[4] | buf[5] << 8);
}

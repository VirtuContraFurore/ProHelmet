/*
 * bmp085.c
 *
 *  Created on: Sep 30, 2020
 *      Author: luca
 */

#include "bmp085.h"

static BMP085_Cal_t cal;

void BMP085_Init(){
	uint8_t * buf = (uint8_t * ) &cal;

	GY80_SendByte(BMP085_ADDR, BMP085_CALIB_ADDR);
	GY80_ReadBytes(BMP085_ADDR, buf, sizeof(BMP085_Cal_t));
	/* Swap little endian */
	for(uint8_t i = 0; i < sizeof(BMP085_Cal_t)/2; i++){
		uint8_t low = 2*i;
		uint8_t high = 2*i+1;
		buf[low] ^= buf[high];
		buf[high] ^= buf[low];
		buf[low] ^= buf[high];
	}

	/* Fix erroneus calibration value */
	cal.AC1 = 0x7a8;
}

uint16_t BMP085_ReadUT(){
	uint8_t data[2] = {BMP085_CTRL_REG, BMP085_CTRL_TEMP};
	GY80_SendBytes(BMP085_ADDR, data, 2);

	__delay_ms(BMP085_CTRL_TEMP_ms);

	GY80_SendByte(BMP085_ADDR, BMP085_MSB_REG);
	GY80_ReadBytes(BMP085_ADDR, data, 2);

	return (data[0] << 8) | data[1];
}

uint32_t BMP085_ReadUP(uint8_t oss){
	uint8_t data[3];
	data[0] = BMP085_CTRL_REG;
	data[1] = BMP085_CTRL_PRESS_OV(oss);

	GY80_SendBytes(BMP085_ADDR, data, 2);

	__delay_ms(BMP085_CTRL_PRESS_OV_ms(oss));

	GY80_SendByte(BMP085_ADDR, BMP085_MSB_REG);
	GY80_ReadBytes(BMP085_ADDR, data, 3);

	return (((data[0] << 16) | (data[1] << 8) | data[2]) >> (8 - oss));
}

int16_t BMP085_TempConversion(uint16_t UT){
	int32_t X1 = (UT-cal.AC6) * cal.AC5 / (1<<15);
	int32_t X2 = cal.MC * (1<<11) / (X1 + cal.MD);
	int32_t B5 = X1 + X2;
	int32_t T = (B5 + 8) / (1<<4);

	return (int16_t) T;
}

void BMP085_Conversion(uint16_t UT, uint32_t UP, uint8_t oss, int32_t * temp, int32_t * press){
	int32_t X1 = ((UT-cal.AC6) * cal.AC5) >> 15;
	int32_t X2 = (cal.MC << 11) / (X1 + cal.MD);
	int32_t B5 = X1 + X2;
	int32_t T = (B5 + 8) >> 4;

	int32_t B6 = B5 - 4000;
	X1 = (cal.B2 * ((B6 * B6) >> 12)) >> 11;
	X2 = cal.AC2 * B6 / (1<<11);
	int32_t X3 = X1 + X2;
	int32_t B3 = ((cal.AC1*4+X3)<<oss + 2)/4;
	X1 = (cal.AC3 * B6) >> 13;
	X2 = (cal.B1 * ( (B6 * B6) >> 12 )) >> 16;
	X3 = ((X1+X2)+2)>>2;
	uint32_t B4 = (cal.AC4 * (uint32_t)(X3 + 32768)) >> 15;
	uint32_t B7 = ((uint32_t) UP - B3) * (50000 >> oss);
	int32_t P;
	if (B7 < 0x80000000)
		P = (B7 << 1) / B4;
	else
		P = (B7 / B4) << 1;
	X1 = (P >> 8) * (P >> 8);
	X1 = (X1 * 3038) >> 16;
	X2 = (-7357 * P) >> 16;
	P = P + ((X1 + X2 + 3791) >> 4);

	*press = P;
	*temp = T;
}


void BMP085_Test(int32_t * temp, int32_t * press){
	BMP085_Conversion(BMP085_ReadUT(), BMP085_ReadUP(0), 0, temp, press);
}

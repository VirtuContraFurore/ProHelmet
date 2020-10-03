/*
 * gy80.h
 *
 *  Created on: Sep 29, 2020
 *      Author: luca
 */

#ifndef DRIVER_GY80_H_
#define DRIVER_GY80_H_

#include "../utils.h"
#include "../system.h"

#define GY80_I2C_SPEED_Khz	100

/*
 * GY80 Module over i2c
 * PB11 ( WHITE ) -> I2C2_SDA
 * PB10 ( RED   ) -> I2C2_SCL
 */

/*
 * Initialize all the hardware
 */
void GY80_Init_i2c(uint32_t freq_khz);

/*
 * Send a byte over i2c
 */
void GY80_SendByte(uint8_t addr, uint8_t data);

/*
 * Send bytes over i2c
 */
void GY80_SendBytes(uint8_t addr, uint8_t * buffer, int16_t count);

/*
 * Read a byte over i2c
 */
uint8_t GY80_ReadByte(uint8_t addr);

/*
 * Read bytes
 */
void GY80_ReadBytes(uint8_t addr, uint8_t * buffer, int16_t count);

#endif /* DRIVER_GY80_H_ */

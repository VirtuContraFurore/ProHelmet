/*
 * l3g4200d.h
 *
 *  Created on: Oct 1, 2020
 *      Author: luca
 */

#ifndef DRIVER_L3G4200D_H_
#define DRIVER_L3G4200D_H_

#include "gy80.h"

#define L3G4200D_ADDR	0b11010010

#define L3G4200D_REG_CTRL1	0x20
#define L3G4200D_REG_CTRL2	0x21
#define L3G4200D_REG_CTRL3	0x22
#define L3G4200D_REG_CTRL4  0x23
#define L3G4200D_REG_CTRL5  0x24
#define L3G4200D_REG_DATA	0x28
#define L3G4200D_AUTOINC	(1<<7)
/*
 * Initialize
 */
void L3G4200D_Init();

/*
 * Write reg
 */
void L3G4200D_WriteReg(uint8_t reg, uint8_t data);

/*
 * Read reg
 */
uint8_t L3G4200D_ReadReg(uint8_t reg);

/*
 * Read angular velocity
 */
void L3G4200D_ReadOmega(int16_t * x, int16_t * y, int16_t * z);

#endif /* DRIVER_L3G4200D_H_ */

/*
 * gy80.c
 *
 *  Created on: Sep 29, 2020
 *      Author: luca
 */

#include "gy80.h"

static inline void i2c_start();
static inline void i2c_stop();
static inline void i2c_enableACK();
static inline void i2c_disableACK();
static inline void i2c_enablePOS();
static inline void i2c_disablePOS();
static inline void i2c_wait_SB();
static inline void i2c_wait_ADDR();
static inline void i2c_wait_TxE();
static inline void i2c_wait_RxNE();
static inline void i2c_wait_BTF();
static inline void i2c_wait_nBUSY();
static inline void i2c_write(uint8_t data);
static inline uint8_t i2c_read();

void GY80_Init_i2c(uint32_t freq_khz){
	/* Setup I2C2 */
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOBEN;

	/* PB10 */
	REG_SETV(GPIOB->MODER, GPIO_MODER_MODER10, GPIO_MODER_ALTERNATE);
	REG_SETV(GPIOB->AFR[1], GPIO_AFRH_AFSEL10, 4);
	REG_SETV(GPIOB->OTYPER, GPIO_OTYPER_OT10, GPIO_OTYPER_OPENDRAIN);
	REG_SETV(GPIOB->OSPEEDR, GPIO_OSPEEDR_OSPEED10, 0b11);
	REG_SETV(GPIOB->PUPDR, GPIO_PUPDR_PUPD10, GPIO_PUPDR_PULLUP);
	/* PB11 */
	REG_SETV(GPIOB->MODER, GPIO_MODER_MODER11, GPIO_MODER_ALTERNATE);
	REG_SETV(GPIOB->AFR[1], GPIO_AFRH_AFSEL11, 4);
	REG_SETV(GPIOB->OTYPER, GPIO_OTYPER_OT11, GPIO_OTYPER_OPENDRAIN);
	REG_SETV(GPIOB->OSPEEDR, GPIO_OSPEEDR_OSPEED11, 0b11);
	REG_SETV(GPIOB->PUPDR, GPIO_PUPDR_PUPD11, GPIO_PUPDR_PULLUP);

	RCC->APB1ENR |= RCC_APB1ENR_I2C2EN;

	I2C2->CR1 = I2C_CR1_PE;
	I2C2->CR2 = 45 << I2C_CR2_FREQ_Pos;
	I2C2->CCR = 500 * SYS_CLK_APB1_Mhz / freq_khz;
}

void GY80_SendByte(uint8_t addr, uint8_t data){
	i2c_wait_nBUSY();
	i2c_start();
	i2c_wait_SB();
	i2c_write(addr & ~1);
	i2c_wait_ADDR();
	i2c_wait_TxE();
	i2c_write(data);
	i2c_wait_TxE();
	i2c_stop();
}

void GY80_SendBytes(uint8_t addr, uint8_t * buffer, int16_t count){
	i2c_wait_nBUSY();
	i2c_start();
	i2c_wait_SB();
	i2c_write(addr & ~1);
	i2c_wait_ADDR();

	while(count-- > 0){
		i2c_wait_TxE();
		i2c_write(*(buffer++));
	}

	i2c_wait_TxE();
	i2c_stop();
}

uint8_t GY80_ReadByte(uint8_t addr){
	i2c_wait_nBUSY();
	i2c_disableACK();
	i2c_start();
	i2c_wait_SB();
	i2c_write(addr | 1);
	i2c_wait_ADDR();
	i2c_wait_RxNE();
	uint8_t read = i2c_read();
	i2c_stop();
	return read;
}


void GY80_ReadBytes(uint8_t addr, uint8_t * buffer, int16_t count){
	i2c_wait_nBUSY();

	if(count > 2){

		i2c_enableACK();
		i2c_disablePOS();

		i2c_start();
		i2c_wait_SB();
		i2c_write(addr | 1);
		i2c_wait_ADDR();

		while(count-- > 2){
			i2c_wait_BTF();
			*(buffer++) = i2c_read();
		}

		i2c_disableACK();

		i2c_wait_BTF();
		i2c_stop();

		*(buffer++) = i2c_read();
		*(buffer++) = i2c_read();

	}else {

		i2c_disableACK();
		i2c_enablePOS();

		i2c_start();
		i2c_wait_SB();
		i2c_write(addr | 1);
		i2c_wait_ADDR();

		i2c_wait_BTF();
		i2c_stop();

		*(buffer++) = i2c_read();
		*(buffer++) = i2c_read();
	}
}

static inline void i2c_start(){
	I2C2->CR1 |= I2C_CR1_START;
}

static inline void i2c_stop(){
	I2C2->CR1 |= I2C_CR1_STOP;
}

static inline void i2c_enableACK(){
	REG_SET(I2C2->CR1,I2C_CR1_ACK);
}
static inline void i2c_disableACK(){
	REG_CLEAR(I2C2->CR1,I2C_CR1_ACK);
}

static inline void i2c_enablePOS(){
	REG_SET(I2C2->CR1,I2C_CR1_POS);
}
static inline void i2c_disablePOS(){
	REG_CLEAR(I2C2->CR1,I2C_CR1_POS);
}

static inline void i2c_wait_SB(){
	while((I2C2->SR1 & I2C_SR1_SB) == 0);
}

static inline void i2c_wait_ADDR(){
	while((I2C2->SR1 & I2C_SR1_ADDR) == 0);
	uint8_t dummy_read = I2C2->SR2;
}

static inline void i2c_wait_TxE()
{
	while((I2C2->SR1 & I2C_SR1_TXE) == 0);
}

static inline void i2c_wait_RxNE(){
	while((I2C2->SR1 & I2C_SR1_RXNE) == 0);
}

static inline void i2c_wait_BTF(){
	while((I2C2->SR1 & I2C_SR1_BTF) == 0);
}

static inline void i2c_wait_nBUSY(){
	while((I2C2->SR2 & I2C_SR2_BUSY) != 0);
}

static inline void i2c_write(uint8_t data){
	I2C2->DR = data;
}
static inline uint8_t i2c_read(){
	return I2C2->DR;
}

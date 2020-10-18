/*
 * bluetooth.c
 *
 *  Created on: Sep 28, 2020
 *      Author: luca
 */

#include "bluetooth.h"
#include "bluetooth_endpoints.h"
#include "../system.h"
#include "string.h"

volatile BT_TxBuffer_t	BT_TxBuffer_objs[BT_TX_BUFFER_OBJECTS];
volatile BT_TxBuffer_t * BT_TxBuffer_head;
volatile BT_TxBuffer_t * BT_TxBuffer_tail;

inline static void keyHigh();
inline static void keyLow();
static volatile BT_TxBuffer_t * findFreeTxBuf();

/*
 * HC-05 Module. AtCommands compliant.
 *
 * Connections: PD7 -> BT KEY (Pink) - set to HIGH to use AT commands
 * 				PD5 -> BT RX  (Yellow) - USART2_TX
 * 				PD6 -> BT TX  (Green) -  USART2_RX
 *
 * 	USART2 is clocked by APB1 @ 45Mhz
 */

void BT_Init(uint32_t baudrate, BT_Parity parity, uint8_t stop_bits){
	BT_TxBuffer_head = 0;
	BT_TxBuffer_tail = 0;

	RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN;

	/* KEY pin */
	REG_SETV(GPIOD->MODER, GPIO_MODER_MODE7, GPIO_MODER_OUTPUT);
	REG_SETV(GPIOD->OTYPER, GPIO_OTYPER_OT7, GPIO_OTYPER_PUSHPULL);
	REG_SETV(GPIOD->OSPEEDR, GPIO_OSPEEDR_OSPEED7, 0b00);
	REG_SETV(GPIOD->PUPDR, GPIO_PUPDR_PUPD7, GPIO_PUPDR_NOPULL);
	keyLow();

	/* Usart's TX */
	REG_SETV(GPIOD->MODER, GPIO_MODER_MODE5, GPIO_MODER_ALTERNATE);
	REG_SETV(GPIOD->OTYPER, GPIO_OTYPER_OT5, GPIO_OTYPER_PUSHPULL);
	REG_SETV(GPIOD->OSPEEDR, GPIO_OSPEEDR_OSPEED5, 0b11);
	REG_SETV(GPIOD->PUPDR, GPIO_PUPDR_PUPD5, GPIO_PUPDR_NOPULL);
	REG_SETV(GPIOD->AFR[0], GPIO_AFRL_AFSEL5, 7);

	/* Usart's RX */
	REG_SETV(GPIOD->MODER, GPIO_MODER_MODE6, GPIO_MODER_ALTERNATE);
	REG_SETV(GPIOD->PUPDR, GPIO_PUPDR_PUPD6, GPIO_PUPDR_NOPULL);
	REG_SETV(GPIOD->AFR[0], GPIO_AFRL_AFSEL6, 7);

	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

//	BT_setAtMode();
	BT_setSerialMode(baudrate, parity, stop_bits);
}

void BT_setAtMode(){
	keyHigh();
	BT_disableInt();

	__delay_ms(5);
	USART2->BRR = SYS_CLK_APB1_Mhz * 1000 * 1000 / BT_ATMODE_BAUDRATE;
	USART2->CR3 = 0;
	USART2->CR2 = 0;
	USART2->CR1 = USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;

	BT_disableInt();
}

void BT_setSerialMode(uint32_t baudrate, BT_Parity parity, uint8_t stop_bits){
	keyLow();
	BT_disableInt();

	USART2->BRR = SYS_CLK_APB1_Mhz * 1000 * 1000 / baudrate;
	USART2->CR3 = 0;
	USART2->CR2 = (stop_bits == 2) ? USART_STOPBITS_2 : USART_STOPBITS_1;
	USART2->CR1 = USART_CR1_RE | USART_CR1_TE | USART_CR1_UE
			| ((parity == BT_NOPARITY) ? 0 : (USART_CR1_PCE | USART_CR1_M | ((parity == BT_PARITY_ODD) ? USART_CR1_PS : 0)))
			| USART_CR1_RXNEIE;

	BT_clearInt();
	BT_enableInt();
}

void BT_sendByte(uint8_t data){
	while((USART2->SR & USART_SR_TXE) == 0);
	USART2->DR = data;
}

void BT_sendBytes(uint8_t * data, int16_t count){
	while(count-- > 0){
		while((USART2->SR & USART_SR_TXE) == 0);
		USART2->DR = *(data++);
	}
}

inline static void keyHigh(){
	GPIOD->BSRR = GPIO_BSRR_BS7;
}

inline static void keyLow(){
	GPIOD->BSRR = GPIO_BSRR_BR7;
}

/*
 * Return first buf obj
 */
static volatile BT_TxBuffer_t * findFreeTxBuf(){
	for(int i = 0; i < BT_TX_BUFFER_OBJECTS; i++){
		if(BT_TxBuffer_objs[i].status == BT_Tx_Empty)
			return &(BT_TxBuffer_objs[i]);
	}
	return 0;
}


volatile BT_TxBuffer_t * BT_sendData(uint8_t * data, int16_t size){
	volatile BT_TxBuffer_t * buf = findFreeTxBuf();
	if(buf == 0)
		return 0;

	size = (size > BT_TX_BUFFER_SIZE) ? BT_TX_BUFFER_SIZE : size;
	memcpy((void *) buf->data,(void *) data, size);
	buf->size = size;
	buf->transmitted = 0;
	buf->status = BT_Tx_Ready;
	buf->next = 0;

	__set_PRIMASK(1);

	if(BT_TxBuffer_head) {
		BT_TxBuffer_tail->next = buf;
	} else {
		BT_TxBuffer_head = buf;
		/* Enable interruption */
		USART2->CR1 |= USART_CR1_TXEIE;
	}

	__set_PRIMASK(0);

	BT_TxBuffer_tail = buf;


	return buf;
}


ISR1(Usart2ISR){
	BT_clearInt();

	/* Transmit Data Register Empty */
	if((USART2->SR & USART_SR_TXE) && BT_TxBuffer_head){
		switch(BT_TxBuffer_head->status){
		case BT_Tx_Ready:
			BT_TxBuffer_head->status = BT_Tx_Busy;
			USART2->DR = BT_TxBuffer_head->data[0];
			BT_TxBuffer_head->transmitted = 1;
			break;
		case BT_Tx_Busy:
			if(BT_TxBuffer_head->transmitted < BT_TxBuffer_head->size){
				USART2->DR = BT_TxBuffer_head->data[BT_TxBuffer_head->transmitted];
				BT_TxBuffer_head->transmitted++;
			} else {
				BT_TxBuffer_head->status = BT_Tx_Empty; // Free buffer
				BT_TxBuffer_head = BT_TxBuffer_head->next;
				if(BT_TxBuffer_head == 0)
					USART2->CR1 &= ~USART_CR1_TXEIE;
			}
		}
	}

	/* Received Data Ready to be Read */
	if(USART2->SR & USART_SR_RXNE)
		EP_dataReceivedCallback(USART2->DR);
}

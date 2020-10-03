/*
 * bluetooth.h
 *
 *  Created on: Sep 28, 2020
 *      Author: luca
 */

#ifndef DRIVER_BLUETOOTH_H_
#define DRIVER_BLUETOOTH_H_

#include "../utils.h"

/* TX linked list */
#define BT_TX_BUFFER_OBJECTS	8
#define BT_TX_BUFFER_SIZE		16

/*
 * HC-05 Module. AtCommands compliant.
 *
 * Connections: PD7 -> BT KEY (Pink) - set to HIGH to use AT commands
 * 				PD5 -> BT RX  (Yellow) - USART2_TX
 * 				PD6 -> BT TX  (Green) -  USART2_RX
 */

#define BT_BAUDRATE 			115200
#define BT_STOPBITS					 1
#define BT_PARITY				BT_PARITY_EVEN
#define BT_ATMODE_BAUDRATE 		38400

typedef enum BT_Parity_enum {BT_NOPARITY=0, BT_PARITY_EVEN, BT_PARITY_ODD} BT_Parity;

/*
 * Rx buffer object
 */
typedef struct BT_RxBuffer {
	int16_t size;
	int16_t received;
	uint8_t cmd_id[2]; /* byte 0 is cmd, byte 1 is id */
	enum {BT_Rx_Empty = 0, BT_Rx_Full, BT_Rx_Busy} status;
	volatile uint8_t * data;
} BT_RxBuffer_t;

/*
 * Tx buffer object
 */
typedef struct BT_TxBuffer {
	int16_t size;
	int16_t transmitted;
	enum {BT_Tx_Empty = 0, BT_Tx_Ready, BT_Tx_Busy, BT_Tx_Transmitted} status;
	uint8_t data[BT_TX_BUFFER_SIZE];
	volatile struct BT_TxBuffer * next;
} BT_TxBuffer_t;

extern volatile BT_TxBuffer_t	BT_TxBuffer_objs[BT_TX_BUFFER_OBJECTS];
extern volatile BT_TxBuffer_t * BT_TxBuffer_head;
extern volatile BT_TxBuffer_t * BT_TxBuffer_tail;

/*
 * Configures usart hardware and key gpio
 * returns 1 if everything was okay
 * parity: NO, EVEN or ODD
 * stop_bits: 1 or 2
 */
void BT_Init(uint32_t baudrate, BT_Parity parity, uint8_t stop_bits);

/*
 * Configure usart to send AT commands
 */
void BT_setAtMode();

/*
 * Configure usart to receive/transmit over bluetooth
 */
void BT_setSerialMode(uint32_t baudrate, BT_Parity parity, uint8_t stop_bits);

/*
 * Send byte over UART, blocking!
 */
void BT_sendByte(uint8_t data);

/*
 * Send bytes over UART, blocking!
 */
void BT_sendBytes(uint8_t * data, int16_t count);

/*
 * Send data using a background buffer
 * if returns zero it wasn't possible to allocate a buffer
 */
volatile BT_TxBuffer_t * BT_sendData(uint8_t * data, int16_t size);


static inline void BT_enableInt(){
	__NVIC_EnableIRQ(USART2_IRQn);
}

static inline void BT_disableInt(){
	__NVIC_DisableIRQ(USART2_IRQn);
}

static inline void BT_clearInt(){
	__NVIC_ClearPendingIRQ(USART2_IRQn);
}



#endif /* DRIVER_BLUETOOTH_H_ */

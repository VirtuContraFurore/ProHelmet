/*
 * bluetooth_endpoints.h
 *
 *  Created on: Sep 28, 2020
 *      Author: luca
 */

#ifndef DRIVER_BLUETOOTH_ENDPOINTS_H_
#define DRIVER_BLUETOOTH_ENDPOINTS_H_

#include "bluetooth.h"

#define EP_CTL_REQ_DATA			'A' // TODO: change this to 0x05
#define EP_CTL_ACK_DATA			'B'
#define EP_CTL_EP_NOTREADY		'C'
#define EP_CTL_EP_NOTEXISTS		'D'

#define EP_CTL_IDLE_RESYNCH		0xFF

#define EP_RX_ENDPOINTS_COUNT		sizeof(EP_RxEndpoints)

typedef volatile BT_RxBuffer_t *EP_ptr_t;
typedef volatile BT_RxBuffer_t * const EP_const_ptr_t;

#define ALLOCATE_ENDPOINT(EndPointName, EndPointSize, EndPointId) 		\
	volatile uint8_t Endpoint_##EndPointName##_data[EndPointSize]; 		\
	volatile BT_RxBuffer_t Endpoint_##EndPointName##_obj = { 			\
		.size = EndPointSize, 											\
		.received = 0,													\
		.cmd_id = {0, EndPointId},										\
		.data = Endpoint_##EndPointName##_data,							\
		.status = BT_Rx_Empty											\
	};																	\
	EP_const_ptr_t Endpoint_##EndPointName = &Endpoint_##EndPointName##_obj

/*** Endpoint extern declaration, must match *.c file ***********************/

extern EP_const_ptr_t Endpoint_Ping, Endpoint_Speed, Endpoint_Time, Endpoint_Notification, Endpoint_SatNav;
extern EP_const_ptr_t const EP_RxEndpoints[];

/*** EndPoint CTL functions *************************************************/

/*
 * Request more data from endpoint and enable the endpoint to receive another payload
 * This is a sort of "ACK" signal if
 */
void EPCTL_RequestData(EP_ptr_t endpoint);

/*
 * Send a CMD related to and endpoint
 */
void EPCTL_Cmd(EP_ptr_t endpoint, uint8_t cmd);

/*
 * Check if payload has been received
 */
static inline int EPCTL_PayloadReceived(EP_ptr_t endpoint){
	return endpoint->status == BT_Rx_Full;
}

/*
 * Callback called from USART's ISR
 */
void EP_dataReceivedCallback(uint8_t data);

#endif /* DRIVER_BLUETOOTH_ENDPOINTS_H_ */

/*
 * bluetooth_endpoints.c
 *
 *  Created on: Sep 29, 2020
 *      Author: luca
 */

#include "bluetooth_endpoints.h"

/***** RX ENDPOINTS DEFINITIONS *************************************************/
/*																				*/
/* ALLOCATE_ENDPOINT(EP_Name, EP_Size_in_bytes, EP_Id_number)  					*/

/*
 * Ping: returns 'O' + 'K'
 * TODO: change 'C' to '0x01'
 */
ALLOCATE_ENDPOINT(Ping, 2, 'a');

/*
 * Current speed measured from GPS
 */
ALLOCATE_ENDPOINT(Speed, 4, 'b');

/*
 * Global list with all the endpoints configured
 */
EP_const_ptr_t const EP_RxEndpoints[] = {Endpoint_Ping, Endpoint_Speed};

/*** Function definitions *******************************************************/

void EPCTL_RequestData(EP_ptr_t endpoint){
	endpoint->status = BT_Rx_Empty;
	EPCTL_Cmd(endpoint, EP_CTL_REQ_DATA);
}

void EPCTL_Cmd(EP_ptr_t endpoint, uint8_t cmd){
	endpoint->cmd_id[0] = cmd;
	BT_sendData((uint8_t*)endpoint->cmd_id, 2);
}

static EP_ptr_t active = 0;
static int16_t ignore = 0;

void EP_dataReceivedCallback(uint8_t data){
	if(ignore-- > 0) /* Endpoint not ready to receive data*/
		return;

	/* Still has to pick the destination */
	if(active == 0){

		/*
		 * Sending a bunch of RESYNCH data will reset ongoing reception
		 * */
		if(data == EP_CTL_IDLE_RESYNCH)
			return;

		for(uint32_t i = 0; i < EP_RX_ENDPOINTS_COUNT; i++)
			if(EP_RxEndpoints[i]->cmd_id[1] == data){
				active = EP_RxEndpoints[i];
				break;
			}

		if(active == 0){
			uint8_t cmd[2] = {EP_CTL_EP_NOTEXISTS, data};
			BT_sendData(cmd, 2);
			return;
		}

		switch(active->status){
		case BT_Rx_Empty:
			active->received = 0;
			active->status = BT_Rx_Busy;
			break;
		case BT_Rx_Full:
			ignore = active->size;
			active = 0;
			EPCTL_Cmd(active, EP_CTL_EP_NOTREADY);
		}
	} else { /* Destination identified */
		active->data[active->received++] = data;
		if(active->received >= active->size){
			active->status = BT_Rx_Full;
			EPCTL_Cmd(active, EP_CTL_ACK_DATA);
			active = 0;
		}
	}
}


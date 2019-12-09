/*
 * app_sim3g.c
 *
 *  Created on: Nov 22, 2019
 *      Author: VAIO
 */
#include "main.h"
#include "app_uart.h"
#include "app_sim3g.h"
#include "app_scheduler.h"

#define DEBUG_SIM3G(x)    				x
#define DATA_TO_SEND_LENGTH				20
#define TIMER_TO_POWER_ON_SIM3G			50
#define TIMER_TO_POWER_OFF_SIM3G		500

const uint8_t OK[] = "OK";
const uint8_t CONNECT_OK[] = "CONNECT OK";
const uint8_t ERROR_1[] = "ERROR";
const uint8_t GREATER_THAN_SYMBOL[] = ">";
const uint8_t NETWORK_OPENED[] = "Network opened";


const uint8_t AT[] = "AT\r\n";
//Define PDP context
const uint8_t AT_CGSOCKCONT[] = "AT+CGSOCKCONT,1,\"100.100.100.100\",\"cmet\"\r\n";
//Set authentication parameters
const uint8_t AT_CGSOCKAUTH[] = "AT+CGSOCKAUTH=1,1,\"teST\",\"1234\"\r\n";
//Set the number of active PDP context
const uint8_t AT_CGSOCKSETPN[] = "AT+CGSOCKSETPN=1\r\n";
//PDP context activation
const uint8_t AT_NETOPEN[] = "AT+NETOPEN=,,1\r\n";
//PDP context deactivatioin
const uint8_t AT_NETCLOSE[] = "AT+NETCLOSE\r\n";
//Option of receiving data's length
const uint8_t AT_CIPHEAD[] = "AT+CIPHEAD=1\r\n";
//Option of receiving data's length
const uint8_t AT_CIPSRIP[] = "AT+CIPSRIP=1\r\n";
//when we use tcp/ip we need to adjust number of retransmission according to network status or
//you want to know your data is received by remote end
const uint8_t AT_CIPCCFG [] = "AT+CIPCCFG=3,500,1,1\r\n";
//Establish a connection
const uint8_t AT_CIPOPEN[] = "AT+CIPOEN=0,\"TCP\",100.100.100.100\",100\r\n";
//Send data
const uint8_t LENGTH_POSITION = 13;
uint8_t AT_CIPSEND[16] = "AT+CIPSEND=0,4\r\n";

uint8_t DataToSend[DATA_TO_SEND_LENGTH];

enum SIM3G_STATE sim3gState = SIM3G_STATUS;


void Sim3gInit(void){

}

void Sim3gEnable(void){

}

void Sim3gWakeUp(void){

}
void PowerSignalLow(void){

}
void PowerSignalHigh(void){

}

static void ATcommandSending(uint8_t * buffer){
	if(isUART1TransmissionReady() == SET){
		UART1_Transmit(buffer);
	}
}

void SIM3G_OPERATION(void){

	switch(sim3gState){
		case SIM3G_STATUS:
			ATcommandSending((uint8_t *)AT);
			break;
		case WAIT_FOR_SIM3G_STATUS_RESPONSE:
			break;
		case POWER_ON_SIM3G:
			SCH_Add_Task(PowerSignalLow, 0, 0);
			SCH_Add_Task(PowerSignalHigh, TIMER_TO_POWER_ON_SIM3G, 0);
			sim3gState = WAIT_FOR_SIM3G_POWER_ON;
			break;
		case WAIT_FOR_SIM3G_POWER_ON:
			break;
		case POWER_OFF_SIM3G:
			SCH_Add_Task(PowerSignalLow, 0, 0);
			SCH_Add_Task(PowerSignalHigh, TIMER_TO_POWER_OFF_SIM3G, 0);
			sim3gState = WAIT_FOR_SIM3G_POWER_OFF;
			break;
		case WAIT_FOR_SIM3G_POWER_OFF:
			break;

		case DEFINE_PDP_CONTEXT:
			break;
		case WAIT_FOR_DEFINE_PDP_CONTEXT_RESPONSE:
			break;
		case SET_AUTHENTICATION_PARAMETER:
			break;
		case SET_PDP_CONTEXT_NUMBER:
			break;
		case PDP_CONTEXT_ACTIVATION:
			break;
		case WAIT_FOR_PDP_CONTEXT_ACTIVATION_RESPONSE:
			break;
		case PDP_CONTEXT_DEACTIVATION:
			break;
		case RECEIVE_DATA_LENGTH_OPTION:
			break;
		case RECEIVE_DATA_ADDRESS_OPTION:
			break;
		case ADJUST_NUMBER_OF_RETRANSMISSION:
			break;
		case WAIT_FOR_ADJUST_NUMBER_OF_RETRANSMISSION_RESPONSE:
			break;
		case ESTABLISH_CONNECTION:
			break;
		case WAIT_FOR_ESTABLISH_CONNECTION_RESPONSE:
			break;
		case SEND_DATA:
			break;
		case WAIT_FOR_SEND_DATA_RESPONSE:
			break;
		case DISCONNECT_CONNECTION:
			break;
		case WAIT_FOR_DISCONNECT_CONNECTION_RESPONSE:
			break;
	}

}





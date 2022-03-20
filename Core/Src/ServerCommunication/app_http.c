/**
 *@file  app_http.c
 *@author thodo
 */


#include <Component/app_sim7600.h>
#include <Peripheral/app_uart.h>
#include <ServerCommunication/app_http.h>
#include <stdio.h>
#include "app_scheduler.h"
#include "app_ATProcessing.h"


#define ACTION_TIMEOUT					200		/*!< Timeout for action command : Get, post ,put ,...*/

FlagStatus http_timeout_flag = SET;
FlagStatus para_timeout = RESET;
FlagStatus default_atcommand = SET;

// Get Content Length
uint8_t http_num_ignore = 0;
uint32_t content_length = 0;
uint8_t temp;


uint16_t result = 0;
uint8_t data_index_2 = 0;

FlagStatus started_record_firmware = RESET;
FlagStatus got_total_cabinet = RESET;
FlagStatus ready_to_warmup_cloudfunction = RESET;

HTTP_State prev_http_state = HTTP_INIT;
HTTP_State http_state = HTTP_INIT;
char http_at_command[150];

//Sync duration and Total Cabinet

uint16_t total_cabinet;
uint8_t cabinet_index = 0;

uint16_t firmware_version_check;
uint16_t firmware_checksum = 0;
uint8_t checksum = 0;
uint32_t http_response_remain = 0;
uint8_t firmware_data[PAGESIZE*2];
uint32_t firmware_address = CURRENT_FIRMWARE_ADDR;
uint16_t firmware_index = 0;
uint32_t firmware_size = 0;
uint16_t one_kb_index = 0;
uint8_t update_status;
/*
 * cabinet_attribute = 0 -> boardID
 * cabinet_attribute = 1 -> portID
 */
uint8_t	cabinet_attribute = 0;
char logMsg[50];

void HTTP_Done();
FlagStatus HTTP_Firmware_Data(FlagStatus first_time);


HTTP_Machine_TypeDef http_state_machine[]={
		{HTTP_INIT					, 		HTTP_Init				},
		{HTTP_WAIT_FOR_INIT			, 		HTTP_Wait_For_Init		},

		{HTTP_PARA					, 		HTTP_Para				},
		{HTTP_WAIT_FOR_PARA			, 		HTTP_Wait_For_Para		},

		{HTTP_ACTION				, 		HTTP_Action				},
		{HTTP_WAIT_FOR_ACTION		, 		HTTP_Wait_For_Action	},

		{HTTP_READ					, 		HTTP_Read				},
		{HTTP_WAIT_FOR_READ			, 		HTTP_Wait_For_Read		},

		{HTTP_TERM					, 		HTTP_Term				},
		{HTTP_WAIT_FOR_TERM			, 		HTTP_Wait_For_Term		},
		{HTTP_DONE					, 		HTTP_Done				}

};

/**
 * HTTP_Display_State()
 * @brief This is function for display state of State Machine. It only show State when having a state changation.
 */
void HTTP_Display_State(void){
	if(prev_http_state!=http_state){
		prev_http_state = http_state;
		switch (http_state) {
			case HTTP_INIT:
				LOG("\r\nHTTP INIT\r\n");
				break;
			case HTTP_WAIT_FOR_INIT:
				LOG("\r\nHTTP WAIT FOR INIT\r\n");
				break;
			case HTTP_PARA:
				LOG("\r\nHTTP PARA\r\n");
				break;
			case HTTP_WAIT_FOR_PARA:
				LOG("\r\nHTTP WAIT FOR PARA\r\n");
				break;
			case HTTP_ACTION:
				LOG("\r\nHTTP ACTION\r\n");
				break;
			case HTTP_WAIT_FOR_ACTION:
				LOG("\r\nHTTP WAIT FOR ACTION\r\n");
				//TODO recheck, break or not ?
				break;
			case HTTP_READ:
				LOG("\r\nHTTP READ\r\n");
				break;
			case HTTP_WAIT_FOR_READ:
				LOG("\r\nHTTP WAIT FOR READ\r\n");
				break;
			case HTTP_TERM:
				LOG("\r\nHTTP TERM\r\n");
				break;
			case HTTP_WAIT_FOR_TERM:
				LOG("\r\nHTTP WAIT FOR TERM\r\n");
				break;
			case HTTP_DONE:
				LOG("\r\nHTTP DONE\r\n");
				break;
			default:
				break;
		}
	}

}

/**
 * HTTP_Run()
 * @brief This is function can be called from external file. It run follow state machine method. Not have param.
 */
uint8_t HTTP_Run(){
	HTTP_Display_State();
	if(http_state < HTTP_DONE){
		(*http_state_machine[http_state].func)();
		return 0;
	}
	else if(http_state == HTTP_DONE){
		LOG("Get into HTTP DONE");
		return 1;
	}
	else{
		//HTTP Error
		return 2;
	}
}

/**
 * HTTP_Init()
 * @brief This is function for initiating Http service
 */
void HTTP_Init(){
	content_length = 0;
	Clear_Reiceive_Buffer();
	if(default_atcommand){
		sprintf(http_at_command,"AT+HTTPINIT\r\n");
	}
	UART_SIM7600_Transmit((uint8_t *)http_at_command);
	Clear_Http_Command();
	http_state = HTTP_WAIT_FOR_INIT;
}


/**
 * HTTP_Wait_For_Init()
 * @brief This is function for waiting respone from initiating Http service
 */
void HTTP_Wait_For_Init(){
//	Wait_For_Respone(AT_OK);
	switch (Get_AT_Result()){
		case AT_OK:
			Clear_AT_Result();
			http_state=HTTP_PARA;
			break;
		case AT_ERROR:
			Clear_AT_Result();
			http_state=HTTP_MAX_STATE;
			break;
		default:
			break;
	}
}


/**
 * HTTP_Para()
 * @brief This is function for passing parameter to HTTP Request
 */
void HTTP_Para(){
	if (default_atcommand) {
		sprintf(http_at_command,"AT+HTTPPARA=\"URL\",\"https://%s-%s.cloudfunctions.net/handle_firmware_request?lockerid=%s&current_version=%s&checking=%d&update_status=%d\"\r\n",CLOUDFUNCTION_REGION,CLOUDFUNCTION_PROJECTID,Sim7600_Get_IMEI(),"v0.0.1",0,Get_Update_Firmware_Status());
	}
	UART_SIM7600_Transmit((uint8_t*)http_at_command);
	Clear_Http_Command();
	http_state = HTTP_WAIT_FOR_PARA;
}

/**
 * HTTP_Wait_For_Para()
 * @brief This is function for waiting respone from  HTTP_PARA state
 * If AT_Result is AT_OK so switch to HTTP_ACTION else it's AT_ERROR so switch to HTTP_MAX_STATE
 */
void HTTP_Wait_For_Para(){
//	Wait_For_Respone(AT_OK);
	switch (Get_AT_Result()){
		case AT_OK:
			Clear_AT_Result();
			http_state=HTTP_ACTION;
			break;
		case AT_ERROR:
			Clear_AT_Result();
			http_state=HTTP_MAX_STATE;
			break;
		default:
			break;
	}
}


/**
 * HTTP_Action()
 * @brief This is function for create request with method : GET,POST,PUT
 * Passing 0 :GET ,1:POST ,...
 */
void HTTP_Action(){
//	Clear_Reiceive_Buffer();
	if (default_atcommand) {
		sprintf(http_at_command,"AT+HTTPACTION=0\r\n");
	}
	UART_SIM7600_Transmit((uint8_t*)http_at_command);
	Clear_Http_Command();
	Clear_Http_Timeout_Flag();
	SCH_Add_Task(Set_Http_Timeout_Flag, ACTION_TIMEOUT, 0);
	http_state = HTTP_WAIT_FOR_ACTION;
}


/**
 * HTTP_Wait_For_Action()
 * @brief This is function for waiting HTTP_ACTION respone
 * If AT_Result is AT_OK so switch to HTTP_READ to read HTTP Respone body
 * else AT_ERROR so switch to HTTP_MAX_STATE to reset Simcom7600
 */
void HTTP_Wait_For_Action(){
	if(is_Http_TimeOutFlag()){
//		Wait_For_Respone(AT_OK);
		switch (Get_AT_Result()){
			case AT_OK:
				Clear_AT_Result();
				break;
			case AT_HTTP_RESPONSE:
				// Get Content-Length:
				if(HTTP_Get_Content_Length()){
					http_response_remain = HTTP_Return_Content_Length();
					content_length = 0;
					sprintf(logMsg,"\r\n%d\r\n",http_response_remain);
					LOG(logMsg);
					Clear_AT_Result();
					http_state = HTTP_READ;
				}
				break;
			case AT_ERROR:
				Clear_AT_Result();
				http_state = HTTP_MAX_STATE;
				break;
			default:
				break;
		}
	}
}


/**
 * HTTP_Read()
 * @brief This is function for read HTTP respone body, passing for AT number of read data. Default is MAX_HTTP_BODY = 400
 */
void HTTP_Read(){
	uint32_t read_size;
	if(http_response_remain > (FIRMWARE_READ_SIZE_PER_TIME)){
		read_size = FIRMWARE_READ_SIZE_PER_TIME;
		http_response_remain = http_response_remain -  (FIRMWARE_READ_SIZE_PER_TIME);
	}
	else if (http_response_remain > 0 && http_response_remain < (FIRMWARE_READ_SIZE_PER_TIME)){
		read_size = http_response_remain;
		http_response_remain = 0;
	}
	if (default_atcommand) {
		sprintf(http_at_command,"AT+HTTPREAD=0,%d\r\n",read_size);
	}
//	Clear_Reiceive_Buffer();
	UART_SIM7600_Transmit((uint8_t *)http_at_command);
	Clear_Http_Timeout_Flag();
	SCH_Add_Task(Set_Http_Timeout_Flag, 100, 0);
	Clear_Http_Command();
	HAL_Delay(100);
	http_state = HTTP_WAIT_FOR_READ;
}


/**
 * HTTP_Wait_For_Read()
 * @brief This is function for waiting HTTP_Read respone
 * If AT_Result is AT_OK so clear AT result
 * else if AT_Result = AT_HTTP_RESPONE so Get BoardID and PortID , If It's done so switch to HTTP_PARA state.
 * else if AT_Result = AT_NOT_FOUND switch to HTTP_PARA every timeout
 * else if AT_Result = AT_ERROR switch to HTTP_MAX_STATE to reset Simcom7600
 */
char log[50];
uint32_t firmware_index_end;
void HTTP_Wait_For_Read(){
	FlagStatus flag_ret;
	switch (Get_AT_Result()){
		case AT_OK:
//			LOG("b");
			if(!started_record_firmware){
				Clear_AT_Result();
			}
			else{
				flag_ret = HTTP_Firmware_Data(RESET);
				if(flag_ret){
					Clear_AT_Result();
					// If still have firmware data
					char log[10];
					sprintf(logMsg,"\r\nhttp_response_remain: %d\r\n",http_response_remain);
					LOG(logMsg);
					if(http_response_remain > 0){
						http_state = HTTP_READ;
					}
					else{
						sprintf(log,"\r\nchecksum : %d\r\n",checksum);
						LOG(log);
						if(checksum == firmware_checksum){
							LOG("Checksum is correct");
							Update_Firmware_Success();
							Jump_To_Current_Firmware();
							http_state = HTTP_DONE;
							started_record_firmware = RESET;
						}
						else{
							LOG("Checksum is incorrect");
							Update_Firmware_Failed();
							Jump_To_Factory_Firmware();
							http_state = HTTP_DONE;
							started_record_firmware = RESET;
						}
					}
				}
			}
			break;
		case AT_FIRMWARE_VERSION:
			firmware_version_check = Get_IntegerValue_From_HTTP_Respone();
			if(firmware_version_check != 0xFFFF){
				Clear_AT_Result();
				Reset_Result();
				if(firmware_version_check  == 0){
					Clear_AT_Result();
					LOG("DON'T HAVE NEW VERSION\r\n");
					Update_Firmware_Success();
					Jump_To_Current_Firmware();

				}
				else{
					Flash_Erase(firmware_address, http_response_remain);
					LOG("HAVE NEW VERSION\r\n");
				}
			}
			break;
		case AT_FIRMWARE_CHECKSUM:
			firmware_checksum = Get_IntegerValue_From_HTTP_Respone();
			if(firmware_checksum != 0xFFFF){
				sprintf(log,"\r\nChecksum : %d\r\n",firmware_checksum);
				LOG(log);
				Clear_AT_Result();
				Reset_Result();
			}
			break;
		// Receive from UART Buffer and Save to Flash
		case AT_FIRMWARE_DATA:
			// If dont have new version
			// If have new Version
			started_record_firmware = SET;
			flag_ret = HTTP_Firmware_Data(SET);
			// If Get done
			if(flag_ret){
				Clear_AT_Result();
				one_kb_index++;
				// If still have firmware data
				sprintf(log,"firmware_index : %d\r\n",firmware_index);
				LOG(log);
				if(http_response_remain > 0){
					http_state = HTTP_READ;
				}
				// If done write data to firmware -> Jump to Current Firmware
				else{
					sprintf(log,"\r\nchecksum : %d\r\n",checksum);
					LOG(log);
					if(checksum == firmware_checksum){
						LOG("checksum_ok");
						http_state = HTTP_DONE;
						started_record_firmware = RESET;
						Update_Firmware_Success();
					}
					else{
						http_state = HTTP_DONE;
						LOG("checksum_error");
						started_record_firmware = RESET;
						Update_Firmware_Failed();
					}
				}
			}
			break;
		case AT_ERROR:
			Clear_AT_Result();
			break;
		default:
			break;
	}
}



/**
 * HTTP_Term()
 * @brief This is function for terminaring HTTP Service
 */
void HTTP_Term(){
//	Clear_Reiceive_Buffer();
	if (default_atcommand) {
		sprintf(http_at_command,"AT+HTTPTERM\r\n");
	}
	UART_SIM7600_Transmit((uint8_t*)http_at_command);
	Clear_Http_Command();
	http_state = HTTP_WAIT_FOR_TERM;
}


/**
 * HTTP_Wait_For_Term()
 * @brief This is function for waiting HTTP_TERM state respone
 * If AT_Result is AT_OK so switch to HTTP_DONE
 * else switch to HTTP_MAX_STATE and reset Simcom7600
 */
void HTTP_Wait_For_Term(){
//	Wait_For_Respone(AT_OK);
	switch (Get_AT_Result()){
		case AT_OK:
			Clear_AT_Result();
			http_state=HTTP_INIT;
			break;
		case AT_ERROR:
			Clear_AT_Result();
			http_state=HTTP_MAX_STATE;
			break;
		default:
			break;
	}
}


/**
 * HTTP_Done()
 * @brief This is function for waiting FSM get out of HTTP Operation
 */
void HTTP_Done(){
	return;
}


/**
 * is_Http_TimeOutFlag()
 * @brief Get http_timeout_flag
 * @return http_timeout_flag
 */
FlagStatus is_Http_TimeOutFlag(void){
	return http_timeout_flag;
}



/**
 * Set_Http_Timeout_Flag()
 * @brief Set http_timeout_flag to SET
 */
void Set_Http_Timeout_Flag(void){
	http_timeout_flag = SET;
}



/**
 * Clear_Http_Timeout_Flag()
 * @brief Clear http_timeout_flag to RESET
 */
void Clear_Http_Timeout_Flag(void){
	http_timeout_flag = RESET;
}



/**
 * Get_IntegerValue_From_HTTP_Respone()
 * @brief This is function for Get Interger value from HTTP Respone body. It will read 4 character in UART buffer to ignore unnessesary character and then read Interger value until meet '"' charracter.
 * @return uint8_t value
 */
uint16_t Get_IntegerValue_From_HTTP_Respone(){
//	"integerValue$(pointer here)": "0"
//	-> need to ignore 4 character
	if(UART_SIM7600_Received_Buffer_Available()){
		uint8_t val_of_character = UART_SIM7600_Read_Received_Buffer();
		if(data_index_2 ++< 2){
			UART_DEBUG_Transmit("1");
			return 0xFFFF;
		}
		if(val_of_character!=','){
			UART_DEBUG_Transmit("2");
			result = result *10 + (uint8_t)val_of_character-48;
			return 0xFFFF;
		}
		else{
			sprintf(log,"\r\n%d",result);
			UART_DEBUG_Transmit("3\r\n");
			UART_DEBUG_Transmit(log);
			data_index_2 = 0;
			return result;
		}
	}
	else{
		return 0xFFFF;
	}
}

/*
 * Return Size of Firmware Data which Received from UART
 */
FlagStatus prepare_record = RESET;
FlagStatus start_record = RESET;
uint16_t count = 0;
uint8_t temp_hex_1 ;
uint8_t temp_hex_2 ;
uint8_t offset = 0;
uint8_t temp_buffer[TEMP_BUFFER_SIZE];
uint16_t temp_index = 0;
uint8_t temp_char;
char new_log[10];
FlagStatus HTTP_Firmware_Data(FlagStatus first_time){
	/*
	 * The first time get data pattern is
	 * {
	 *  	"new_version": 2,
	 *  	"checksum": 115,
	 *  	"data": "0ABCDGEGGASD...
	 *  			ASDB12312512412...
	 *  			01231291512925192"
	 *  So We need seperate "new_version" and "checksum" field out of "data"
	 */
	if(first_time){
		if(UART_SIM7600_Received_Buffer_Available()){
			temp_buffer[temp_index] = UART_SIM7600_Read_Received_Buffer();
			//If get '"' so start record firmware data
			if(temp_buffer[temp_index] == '"'){
				start_record = SET;
				return RESET;
			}
			else if(start_record){
				// If get '\r' so done get partial firmware data from UART
				if(temp_buffer[temp_index] == '\r'){
					start_record = RESET;
					return SET;
				}
				// If get '"' so done get total firmware data from UART
				if(temp_buffer[temp_index] == '"'){
					Flash_Write_Char(firmware_address, firmware_data, firmware_index);
					start_record = RESET;
					return SET;
				}
				// Else record firmware data to buffer to save into Flash Memory
				else{
					// Assign temp buffer to firmware buffer
					firmware_data[firmware_index] = temp_buffer[temp_index];
//					sprintf(log,"%d\r\n",firmware_data[firmware_index]);
//					LOG(log);
					/*
					 * If uart data is 0ABC5FDE so firmware_data is [0x0A,0xBC,0x5F,0xDE]
					 * In offset = 0 is 0,B,5,D, convert it from Char to Hex and assign to temp_hex_1
					 * ex: firmware_data[0] = [0x0]
					 */
					if(offset == 0){
						temp_hex_1 = Char2Hex(firmware_data[firmware_index]);
						firmware_index++;
					}
					/*
					 * In offset = 1 is A,C,F,E, convert it from Char to Hex and assign to temp_hex_2
					 * ex: firmware_data[1] = [0xA]
					 * So calculate and reassigin to firmware_data[0] = 0<<4|A = 0x0A
					 * and Calculate checksum = checksum ^ firmware_data[0]
					 */
					else if(offset == 1){
						temp_hex_2 = Char2Hex(firmware_data[firmware_index]);
						firmware_data[firmware_index==0?(PAGESIZE-1):(firmware_index-1)] = (temp_hex_1<<4)|temp_hex_2;
						checksum = checksum ^ firmware_data[firmware_index==0?(PAGESIZE-1):(firmware_index-1)];
//						sprintf(log,"check sum %d\r\n",checksum);
//						LOG(log);
					}
					// Increase Offset after get and assign to firmware_data
					offset= (offset+1)%2;
					/*
					 * Check whether offset == 0 so Check firmware_index
					 * If firmware_index == FIRMWARE_DATA_BUFFER_SIZE so save it to Flash, reset firmware_index and increase firmware_address
					 */
					if(offset==0){
						if(firmware_index == PAGESIZE){
							char log[10];
							Flash_Write_Char(firmware_address, firmware_data, firmware_index);
							firmware_address += PAGESIZE;
							firmware_index = 0;
							return RESET;
						}
					}
				}
			}
			temp_index = (temp_index +1)%TEMP_BUFFER_SIZE;
		}
	}
	else{
		if(UART_SIM7600_Received_Buffer_Available()){
			temp_buffer[temp_index] = UART_SIM7600_Read_Received_Buffer();
			// If get response "+HTTPREAD: DATA," from UART switch to Prepare Record
			if(isReceiveData_New(temp_buffer, temp_index + 1,TEMP_BUFFER_SIZE, "+HTTPREAD: DATA,")){
				prepare_record = SET;
				return RESET;
			}
			// Prepare Record wait to get "\r\n" to get data
			else if(prepare_record){
				// If get "\r\n" so set start_record to record hex data
				if(isReceiveData_New(temp_buffer, temp_index  + 1,TEMP_BUFFER_SIZE, "\r\n")){
					prepare_record = RESET;
					start_record = SET;
					return RESET;
				}
			}
			// start_record firmware data from UART
			else if(start_record){
				// If get '\r' so done get partial firmware data from UART
				if(temp_buffer[temp_index] == '\r'){
					start_record = RESET;
					return SET;
				}
				// If get '"' so done get total firmware data from UART
				else if(temp_buffer[temp_index] == '"'){
					Flash_Write_Char(firmware_address, firmware_data, firmware_index);
					start_record = RESET;
					return SET;
				}
				// Else record firmware data to buffer to save into Flash Memory
				else {
					// Assign temp buffer to firmware buffer
					firmware_data[firmware_index] = temp_buffer[temp_index];
					/*
					 * If uart data is 0ABC5FDE so firmware_data is [0x0A,0xBC,0x5F,0xDE]
					 * In offset = 0 is 0,B,5,D, convert it from Char to Hex and assign to temp_hex_1
					 * ex: firmware_data[0] = [0x0]
					 */
					if(offset == 0){
						temp_hex_1 = Char2Hex(firmware_data[firmware_index]);
						firmware_index++;
					}
					/*
					 * In offset = 1 is A,C,F,E, convert it from Char to Hex and assign to temp_hex_2
					 * ex: firmware_data[1] = [0xA]
					 * So calculate and reassigin to firmware_data[0] = 0<<4|A = 0x0A
					 * and Calculate checksum = checksum ^ firmware_data[0]
					 */
					else if(offset == 1){
						temp_hex_2 = Char2Hex(firmware_data[firmware_index]);
						firmware_data[firmware_index==0?(PAGESIZE-1):(firmware_index-1)] = (temp_hex_1<<4)|temp_hex_2;
						checksum = checksum ^ firmware_data[firmware_index==0?(PAGESIZE-1):(firmware_index-1)];
//						sprintf(log,"check sum %d\r\n ",checksum);
//						LOG(log);
					}
					// Increase Offset after get and assign to firmware_data
					offset= (offset+1)%2;
					/*
					 * Check whether offset == 0 so Check firmware_index
					 * If firmware_index == FIRMWARE_DATA_BUFFER_SIZE so save it to Flash, reset firmware_index and increase firmware_address
					 */
					if(offset==0){
						if(firmware_index == PAGESIZE){
							sprintf(logMsg,"\r\n%x\r\n",firmware_address);
							LOG((uint8_t*)logMsg);
							Flash_Write_Char(firmware_address, firmware_data, firmware_index);
							firmware_address += PAGESIZE;
							firmware_index = 0;
							return RESET;
						}
					}
				}
			}
			temp_index = (temp_index +1)%TEMP_BUFFER_SIZE;
		}
	}
	return RESET;
}



FlagStatus HTTP_Get_Content_Length(){
	if(UART_SIM7600_Received_Buffer_Available()){
		temp = UART_SIM7600_Read_Received_Buffer();
		if(temp == '\r'){
			http_num_ignore = 0;
			return SET;
		}
		if(http_num_ignore==2){
			content_length = content_length*10 + (uint32_t)temp - (uint32_t)48;
		}
		if(temp == ','){
			http_num_ignore ++;
		}
	}
	return RESET;
}

/**
 * HTTP_Get_State()
 * @brief Get state of HTTP State Machine
 * @return http_state
 */
HTTP_State HTTP_Get_State(){
	return http_state;
}



/**
 * HTTP_Set_State()
 * @brief This is function for setting state to HTTP State Machine
 */
void HTTP_Set_State(HTTP_State _http_state){
	http_state = _http_state;
}

void Reset_Result(){
	result = 0;
}

void Reset_HttpConfiguration_State(){
	Reset_No_Board();
	HTTP_Set_State(HTTP_INIT);
}

void Clear_Http_Command(){
	default_atcommand = SET;
}

void Set_Http_Command(char * atcommand){
	sprintf(http_at_command,"%s",atcommand);
	default_atcommand = RESET;
}

uint32_t HTTP_Return_Content_Length(){
	return content_length;
}

uint32_t Get_Firmware_Size(){
	return firmware_size;
}


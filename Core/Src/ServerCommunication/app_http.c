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
#define TEMP_VERSION_BUFFER_LENGTH 		100
#define LINE_BUFFER_LENGTH				300

FlagStatus http_timeout_flag = SET;
FlagStatus para_timeout = RESET;
FlagStatus default_atcommand = SET;
FlagStatus fota_check_version = SET;
FlagStatus checksum_correct = SET;
uint8_t version[VERSION_LENGTH];
uint8_t version_index= 0;

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
FlagStatus is_Firmware_Line_Data_Correct(uint8_t *buffer, uint16_t buffer_len);
FlagStatus HTTP_Firmware_Version();
FlagStatus HTTP_Firmware_Data();


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
 * @brief This is function for waiting respond from initiating Http service
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
		if(fota_check_version){
			sprintf(http_at_command,"AT+HTTPPARA=\"URL\",\"http://ota.chipfc.com/ebox_firmware/version.txt\"\r\n");
		}
		else{
			sprintf(http_at_command,"AT+HTTPPARA=\"URL\",\"http://ota.chipfc.com/ebox_firmware/%s/eBoxK2.hex\"\r\n",version);
		}
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
	if(fota_check_version){
		read_size = http_response_remain;
	}
	else{
		if(http_response_remain > (FIRMWARE_READ_SIZE_PER_TIME)){
			read_size = FIRMWARE_READ_SIZE_PER_TIME;
			http_response_remain = http_response_remain -  (FIRMWARE_READ_SIZE_PER_TIME);
		}
		else if (http_response_remain > 0 && http_response_remain < (FIRMWARE_READ_SIZE_PER_TIME)){
			read_size = http_response_remain;
			http_response_remain = 0;
		}
	}
	if (default_atcommand) {
		sprintf(http_at_command,"AT+HTTPREAD=0,%d\r\n",read_size);
	}
//	Clear_Reiceive_Buffer();
	sprintf(logMsg,"http_response_remain: %ld\r\n",http_response_remain);
	LOG(logMsg);
	sprintf(logMsg,"firmware_index: %ld\r\n",firmware_index);
	LOG(logMsg);
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
	if(fota_check_version){
		switch (Get_AT_Result()) {
			case AT_OK:
				flag_ret = HTTP_Firmware_Version();
				if(flag_ret){
					fota_check_version = RESET;
					Clear_AT_Result();
					http_state = HTTP_PARA;
					return;
				}
				break;
			default:
				break;
		}
	}
	else{
		switch (Get_AT_Result()) {
			case AT_OK:
				flag_ret = HTTP_Firmware_Data();
				if(flag_ret){
					if(checksum_correct){
						if(http_response_remain == 0){
							LOG("Jump To Current Firmware");
							Jump_To_Current_Firmware();
						}
					}
					else{
						LOG("Jump To Factory Firmware");
						Jump_To_Factory_Firmware();
					}

					Clear_AT_Result();
					http_state = HTTP_READ;
				}
				break;
			default:
				break;
		}
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


uint8_t temp_version_name_buffer[TEMP_VERSION_BUFFER_LENGTH];
uint8_t temp_version_name_index = 0;
FlagStatus prepare_record_version_name = RESET;
FlagStatus start_record_version_name = RESET;
FlagStatus HTTP_Firmware_Version(){
	if(UART_SIM7600_Received_Buffer_Available()){
		temp_version_name_buffer[temp_version_name_index] = UART_SIM7600_Read_Received_Buffer();
//		UART_DEBUG_Transmit_Size(temp_version_name_buffer + temp_version_name_index, 1);
		if(isReceiveData_New(temp_version_name_buffer, temp_version_name_index + 1, TEMP_VERSION_BUFFER_LENGTH, "+HTTPREAD: DATA")){
			prepare_record_version_name = SET;
		}
		if(prepare_record_version_name){
			if(start_record_version_name){
				//Check whether stop record version name
				if(temp_version_name_buffer[temp_version_name_index]=='\r'){
					start_record_version_name = RESET;
					prepare_record_version_name = RESET;
					return SET;
				}
				else{
					version[version_index++] = temp_version_name_buffer[temp_version_name_index];
				}
			}
			if(isReceiveData_New(temp_version_name_buffer, temp_version_name_index + 1, TEMP_VERSION_BUFFER_LENGTH, "\r\n")){
				start_record_version_name = SET;
			}

		}
		temp_version_name_index = (temp_version_name_index + 1)% TEMP_VERSION_BUFFER_LENGTH;
	}
	return RESET;
}

/*
 * Line is: ":0101010101010 and checksum :01, last is "\r\n""
 * We ignore ':' character and calculate checksum from 01....010 and ignore checksum value
 */
FlagStatus is_Firmware_Line_Data_Correct(uint8_t *buffer, uint16_t buffer_len){
	FlagStatus flag_ret;
	checksum = 0;

	for (uint16_t var = 0; var < buffer_len - 3 - 2; var=var+2) {
		sprintf(log,"%c-%c\r\n",buffer[var],buffer[var+1]);
		LOG(log);
		checksum = checksum + (Char2Hex(buffer[var]) << 4) + Char2Hex(buffer[var+1]);
	}
	checksum =~checksum;
	checksum += 1;
	uint8_t checksum_inline = (Char2Hex(buffer[buffer_len -3-2 ]) << 4) + Char2Hex(buffer[buffer_len -3-2 +1 ]);
	sprintf(log,"Check sum %x\r\n",checksum);
	LOG(log);
	sprintf(log,"Calculated Check sum %x\r\n",checksum_inline);
	LOG(log);
	flag_ret = (checksum == checksum_inline);
	return flag_ret;
}
/*
 * Return Size of Firmware Data which Received from UART
 */
FlagStatus prepare_record_firmware_data = RESET;
FlagStatus start_record_firmware_data = RESET;
uint16_t count = 0;
uint8_t temp_hex_1 ;
uint8_t temp_hex_2 ;
uint8_t offset = 0;
uint8_t temp_firmware_data_buffer[TEMP_BUFFER_SIZE];
uint16_t temp_firmware_data_index = 0;
uint8_t line_buffer[LINE_BUFFER_LENGTH];
uint16_t line_buffer_index;
uint8_t temp_at_response_buffer[LINE_BUFFER_LENGTH];
uint16_t temp_at_response_index = 0;
uint8_t temp_char;
char new_log[10];
FlagStatus first_http_read = SET;
FlagStatus HTTP_Firmware_Data(){
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
	if(UART_SIM7600_Received_Buffer_Available()){
		temp_at_response_buffer[temp_at_response_index] = UART_SIM7600_Read_Received_Buffer();
//		UART_DEBUG_Transmit_Size(line_buffer + line_buffer_index, 1);
		//Check if end of SIM respond
		if(isReceiveData_New(temp_at_response_buffer, temp_at_response_index+1, LINE_BUFFER_LENGTH, "\r\n+HTTPREAD: 0")){
//			LOG("1");
			if(firmware_index >= PAGESIZE){
				Flash_Write_Char(firmware_address, firmware_data, PAGESIZE);
//				LOG("3");
				firmware_address+= PAGESIZE;
				memcpy(firmware_data,firmware_data+PAGESIZE,firmware_index-PAGESIZE);
				firmware_index-=PAGESIZE;
//				for (int var = PAGESIZE; var < firmware_index; ++var) {
//					firmware_data[var-PAGESIZE] = firmware_data[var];
//				}
				LOG("4");
			}
			else if(http_response_remain == 0){
				Flash_Write_Char(firmware_address, firmware_data, firmware_index);
				firmware_address+= firmware_index;
				firmware_index = 0;
			}
			line_buffer_index = line_buffer_index - strlen("\r\n+HTTPREAD: 0") + 1;
			start_record_firmware_data = RESET;
			return SET;
		}
		// Check whether start of SIM Respond
		if(isReceiveData_New(temp_at_response_buffer, temp_at_response_index+1, LINE_BUFFER_LENGTH, "HTTPREAD: DATA")){
//			LOG("2");
			prepare_record_firmware_data = SET;
		}
		if(prepare_record_firmware_data){
//			LOG("3");
			if(isReceiveData_New(temp_at_response_buffer, temp_at_response_index+1, LINE_BUFFER_LENGTH, "\r\n")){
				start_record_firmware_data = SET;
				prepare_record_firmware_data = RESET;
				temp_at_response_index = 0;
				// reset line_index
			}
		}
		if(start_record_firmware_data){
//			LOG("4");
			// Check whether that data is not end of HTTP READ
			line_buffer[line_buffer_index] = temp_at_response_buffer[temp_at_response_index];
			line_buffer_index = (line_buffer_index +1)%LINE_BUFFER_LENGTH;
			if(isReceiveData_New(temp_at_response_buffer, temp_at_response_index +1, LINE_BUFFER_LENGTH, "\r\n:")){
//				UART_DEBUG_Transmit_Size(line_buffer, line_buffer_index+1);
				// Calculator checksum
				if(first_http_read){
					first_http_read = RESET;
					line_buffer_index = 0;
				}
				else{
					if(is_Firmware_Line_Data_Correct(line_buffer, line_buffer_index)){
						// Check whether that line is the firmware data or not
						if(Char2Hex(line_buffer[9])==0){
							for (int var = 10; var < line_buffer_index -4 -2; var=var+2) {
								//Save line to firmware data
								firmware_data[firmware_index++] = Char2Hex(line_buffer[var])<<4 + Char2Hex(line_buffer[var+1]);
							}
						}
						line_buffer_index = 0;
					}
					else{
						checksum_correct = RESET;
						LOG("Checksum is Wrong");
					}
				}
			}
		}
		temp_at_response_index = (temp_at_response_index +1)%LINE_BUFFER_LENGTH;
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


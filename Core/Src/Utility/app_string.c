/**
 * @file app_string.c
 * @author thodo
 */
#include <Utility/app_string.h>
#include <string.h>

/**
 * isReceiveData(char *buffer , uint8_t buffer_len , const char * data)
 * @param buffer: point to buffer address
 * @param buffer_len: length of buffer
 * @param data: string to compare
 * @retval SET: When 2 string equal
 * 			else : RESET
 * @brief: Compare ending of buffer to data string. If equal return SET else return RESET
 * Example: buffer = "I love C"
 * 			buffer_len = 5 => It just point to "I lov"
 * 			data = "love"
 *
 * 			=> return RESET ~ False
 *
 * An other example:
 * 			buffer = "I love C"
 * 			buffer_len = 6 => It just point to "I love"
 * 			data = "love"
 *
 * 			=> return SET ~ True
 */
FlagStatus isReceiveData(char *buffer , uint16_t buffer_len , const char * data){
	uint8_t tmpLen = strlen((char*)data);
	if(buffer_len < tmpLen ){
		return RESET;
	}
	else{
		uint8_t tmpLen = strlen((char*)data);
		for (uint8_t index = 0; index < tmpLen; index++) {
			if(buffer[buffer_len -tmpLen + index] != data[index]){
				return RESET;
			}
		}
	}
	return SET;
}


FlagStatus isReceiveData_New(char *buffer , uint16_t buffer_len , uint16_t buffer_size , const char * data){
	uint16_t tmpLen = strlen((char*)data);
	for (uint16_t index = 0; index < tmpLen; index++) {
		if(buffer[buffer_len -tmpLen <0 ? (buffer_size + buffer_len - tmpLen + index)%buffer_size :(buffer_len - tmpLen + index)%buffer_size ] != data[index]){
			return RESET;
		}
	}
	return SET;
}

uint16_t String2Int(char * buffer , uint16_t buffer_length){
	uint16_t result = 0;
	for (uint16_t var = 0; var < buffer_length; ++var) {
		result = result*10 + (uint8_t)buffer[var] - 48;
	}
	return result;
}


uint8_t Char2Hex(char character){
	uint8_t result;
	if( character >='0' && character <= '9'){
		result = character - (uint8_t)('0') ;
	}
	else if(character >='A' && character <= 'F'){
		result = character - (uint8_t)('A') + 10;
	}
	return result;
}



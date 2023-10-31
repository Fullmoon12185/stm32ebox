#include "utils_string.h"

bool utils_string_is_receive_data(char *buffer , uint16_t buffer_len , const char * data){
	if(buffer == NULL || data == NULL){
		return false;
	}
	uint16_t data_len = strlen(data);
	uint16_t actual_buffer_len = strlen(buffer);
	// Validate valid buffer
	if(actual_buffer_len == 0 || actual_buffer_len < buffer_len || data_len == 0){
		return false;
	}
	else{
		for (uint16_t index = 0; index < data_len; index++) {
			if(buffer[buffer_len -data_len + index] != data[index]){
				return false;
			}
		}
	}
	return true;
}


/**
 * @brief Convert String to Interger
 *
 * @param buffer Pointer to buffer
 * @param buffer_length Buffer Len
 * @return true Interger
 */
uint16_t utils_string_to_int(char * buffer , uint16_t buffer_length){
	uint16_t result = 0;
	for (uint16_t var = 0; var < buffer_length; ++var) {
		result = result*10 + (uint8_t)buffer[var] - 48;
	}
	return result;
}

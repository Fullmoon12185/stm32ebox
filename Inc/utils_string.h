#ifndef UTILS_BUFFER_H
#define UTILS_STRING_H

#include "stdint.h"
#include "stdbool.h"
#include "string.h"


/**
 * @brief Compare Buffer with Len and Data String from the end of buffer to start of Buffer
 * 
 * @param buffer Pointer to buffer 
 * @param buffer_len Buffer Len
 * @param data String
 * @return true if Matched
 * @return false if not Matched
 */
bool utils_string_is_receive_data(char *buffer , uint16_t buffer_len , const char * data);


/**
 * @brief Convert String to Interger
 *
 * @param buffer Pointer to buffer
 * @param buffer_length Buffer Len
 * @return true Interger
 */
uint16_t utils_string_to_int(char * buffer , uint16_t buffer_length);

#endif //UTILS_BUFFER_H

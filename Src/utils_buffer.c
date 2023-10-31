#include "utils_buffer.h"

#include "string.h"



/**
 * @brief Initialize Buffer
 * 
 * @param buffer Pointer to Buffer Object
 * @return true if OK
 * @return false if Failed
 */
bool utils_buffer_init(utils_buffer_t * buffer, uint16_t sizeOfObject){
    buffer->head = 0;
    buffer->tail = 0;
    buffer->count = 0;
    buffer->size = sizeOfObject;
    memset(buffer->buffer , 0, sizeof(buffer->buffer));
    return true;
}

/**
 * @brief Push data to Buffer
 * 
 * @param buffer Pointer to Buffer
 * @param data Pointer to data in
 * @param data_size Data Size
 * @return true if OK
 * @return false if Failed
 */
bool utils_buffer_push(utils_buffer_t * buffer, void * object){
	uint8_t * data_p = (uint8_t*) object;
	if(utils_buffer_is_full(buffer)){
		return false;
	}
    for (size_t i = 0; i < buffer->size; i++)
    {
        buffer->buffer[buffer->head] = data_p[i];
        buffer->head = (buffer->head + 1) % BUFFER_MAX_SIZE;
    }
    return true;
}

/**
 * @brief Pop data from Buffer
 * 
 * @param buffer Pointer to Buffer
 * @param data Pointer to data out
 * @param data_size Data size
 * @return true If OK
 * @return false If failed
 */
bool utils_buffer_pop(utils_buffer_t  * buffer, void *object){
	uint8_t * data_p = (uint8_t *)object;
    for (size_t i = 0; i < buffer->size; i++)
    {
        /* code */
    	data_p[i] = buffer->buffer[buffer->tail];
        buffer->tail = (buffer->tail + 1) % BUFFER_MAX_SIZE;
    }
    return true;
}


/**
 * @brief Check Buffer is available or not
 * 
 * @param buffer Pointer to buffer
 * @return true if buffer available
 * @return false if buffer not available
 */

bool utils_buffer_is_available(utils_buffer_t * buffer){
    if(buffer->head >= buffer->tail){
        return (buffer->head - buffer->tail >= buffer->size);
    }else{
    	return (BUFFER_MAX_SIZE - buffer->tail + buffer->head >= buffer->size);
    }
}


/**
 * @brief Check Buffer is available or not
 * 
 * @param buffer Pointer to buffer
 * @return true if buffer available
 * @return false if buffer not available
 */
bool utils_buffer_peek_idx(utils_buffer_t * buffer, size_t idx, void *object){
    if(idx * buffer->size < 0 || idx * buffer->size  > BUFFER_MAX_SIZE){
        return false;
    }
    memcpy(object, &buffer->buffer[buffer->size], buffer->size);
}

bool utils_buffer_is_full(utils_buffer_t * buffer){
	size_t remain;
	if(buffer->head >= buffer->tail){
		remain = BUFFER_MAX_SIZE - (buffer->head - buffer->tail);
	}else{
		remain = buffer->tail - buffer->head;
	}
	if(remain >= buffer->size){
		return false;
	}else{
		return true;
	}
}

bool utils_buffer_drop_all(utils_buffer_t * buffer){
    buffer->head = 0;
    buffer->tail = 0;
    buffer->count = 0;
    memset(buffer->buffer , 0, sizeof(buffer->buffer));
    return true;
}


#ifndef UTILS_BUFFER_H
#define UTILS_BUFFER_H

#include "stdio.h"
#include "stdbool.h"
#include "stdint.h"

#define BUFFER_MAX_SIZE     2048

typedef struct {
    size_t tail;
    size_t head;
    size_t count;
    uint8_t buffer[BUFFER_MAX_SIZE];
    uint16_t size;
} utils_buffer_t;


/**
 * @brief Initialize Buffer
 * 
 * @param buffer Pointer to Buffer Object
 * @param sizeOfObject Size of Object
 * @return true if OK
 * @return false if Failed
 */

bool utils_buffer_init(utils_buffer_t * buffer, uint16_t sizeOfObject);

/**
 * @brief Push data to Buffer
 * 
 * @param buffer Pointer to Buffer
 * @param data Pointer to data in
 * @param data_size Data Size
 * @return true if OK
 * @return false if Failed
 */
bool utils_buffer_push(utils_buffer_t * buffer, void * object);

/**
 * @brief Pop data from Buffer
 * 
 * @param buffer Pointer to Buffer
 * @param data Pointer to data out
 * @param data_size Data size
 * @return true If OK
 * @return false If failed
 */
bool utils_buffer_pop(utils_buffer_t  * buffer, void *object);


/**
 * @brief Check Buffer is available or not
 * 
 * @param buffer Pointer to buffer
 * @return true if buffer available
 * @return false if buffer not available
 */
bool utils_buffer_is_available(utils_buffer_t * buffer);

/**
 * @brief Peek Element at Index
 * 
 * @param buffer Pointer to Netif Buffer Object
 * @param idx Index of Data in buffer->buffer
 * @return uint8_t* Pointer to Data[index]
 */
bool utils_buffer_peek_idx(utils_buffer_t * buffer, size_t idx, void *object);

bool utils_buffer_is_full(utils_buffer_t * buffer);

bool utils_buffer_drop_all(utils_buffer_t * buffer);

#endif

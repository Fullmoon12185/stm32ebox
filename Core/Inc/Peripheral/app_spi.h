/*
 * app_spi.h
 *
 *  Created on: Dec 14, 2019
 *      Author: VAIO
 */

#ifndef APP_SPI_H_
#define APP_SPI_H_

HAL_StatusTypeDef SPI2_Init(void);
void ResetChipSelect(void);
void SetChipSelect(void);

#endif /* APP_SPI_H_ */

/*
 * app_25LC512.h
 *
 *  Created on: Dec 14, 2019
 *      Author: VAIO
 */

#ifndef APP_25LC512_H_
#define APP_25LC512_H_


/*EEProm Definitions */
#define MC25LCxxx_SPI_READ 			0x03 	//0000 0011 Read data from memory array beginning at selected address
#define MC25LCxxx_SPI_WRITE 		0x02  //0000 0010 Write data to memory array beginning at selected address
#define MC25LCxxx_SPI_WREN 			0x06	//0000 0110 Set the write enable latch (enable write operations)
#define MC25LCxxx_SPI_WRDI 			0x04	//0000 0100 Reset the write enable latch (disable write operations)
#define MC25LCxxx_SPI_RDSR 			0x05	//0000 0101 Read STATUS register
#define MC25LCxxx_SPI_WRSR 			0x01	//0000 0001 Write STATUS register
#define MC25LCxxx_SPI_PE 			0x42	//0100 0010 Page Erase � erase one page in memory array
#define MC25LCxxx_SPI_SE 			0xD8	//1101 1000 Sector Erase � erase one sector in memory array
#define MC25LCxxx_SPI_CE 			0xC7	//1100 0111 Chip Erase � erase all sectors in memory array
#define MC25LCxxx_SPI_RDID 			0xAB	//1010 1011 Release from Deep power-down and read electronic signature
#define MC25LCxxx_SPI_DPD 			0xB9	//1011 1001 Deep Power-Down mode

#define MC25LCxxx_STATUS_WPEN		0x80  //Read Write Array Address Write Protect Bit 1
#define MC25LCxxx_STATUS_BP1		0x08	//Read Write Array Address Write Protect Bit 1
#define MC25LCxxx_STATUS_BP0		0x04	//Read Write Array Address Write Protect Bit 0
#define MC25LCxxx_STATUS_WEL		0x02	//Read only Write Enable Latch
#define MC25LCxxx_STATUS_WIP		0x01	//Read only Write In Process

#define EEPROM_Enable 				1
#define EEPROM_Disable 				0
#define EEPROM_CS_PIN_SET 			1
#define EEPROM_CS_PIN_RESET 		0
//extern unsigned char Enable,Disable;

void MC25LC512_Initialize(void);
void MC25LC512_Write_Bytes(uint16_t AddresOfData, uint8_t *WriteArrayOfEEProm, uint16_t SizeOfArray);
void MC25LC512_Write_HalfWords(uint16_t AddresOfData, uint16_t *WriteArrayOfEEProm, uint16_t sizeOfArray);
void MC25LC512_Write_Words(uint16_t AddresOfData, uint32_t *WriteArrayOfEEProm, uint16_t sizeOfArray);

uint8_t MC25LC512_ReadStatusRegister(void);
uint8_t MC25LC512_Read_Bytes(uint16_t AddresOfData, uint8_t *DataArrayOfEEProm, uint16_t SizeOfArray);
uint8_t MC25LC512_Read_HalfWords(uint16_t AddresOfData, uint16_t *DataArrayOfEEProm, uint16_t SizeOfArray);
uint8_t  MC25LC512_Read_Words(uint16_t AddresOfData, uint32_t *DataArrayOfEEProm, uint16_t SizeOfArray);

void MC25LC512_CS(uint8_t CS_Status);

#endif /* APP_25LC512_H_ */

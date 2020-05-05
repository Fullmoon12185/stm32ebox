/*
 * app_eeprom.h
 *
 *  Created on: 24 Mar 2020
 *      Author: phamvanvinh
 */

#ifndef APP_EEPROM_H_
#define APP_EEPROM_H_
#include "main.h"
//eeprom dùng trong trường hợp bị cúp điện hoặc mạch bị reset
// lưu lại trạng thái hoạt động trước đó của outlet và gia trị nănng lương
//Khi bị reset thì hệ thống hoạt động tiếp, không cần phải hỏi server

// nếu rút ra trước khi bị ngắt điện thì sao => hoạt động lại => charge full => timeout to turn off
//nếu rút ra sau khi bị ngắt điện thì sao => sau khi hoạt động lại => không có điện => full => timeout to turn off
// đang sạc => ngắt điện => có điện => sạc tiếp ? => tới giá trị limit thì dừng
// đang bình thường => ngắt điện => có điện  => bình thường
// sẵn sàng & set  limit = > ngắt điện => có điện => sẵn sàng ???


//void eeprom_start_saving();
//void eeprom_stop_saving();
//nếu kênh nào có trạng thái hoặc số năng lượnghyay đổi thì mới lưu
//hàm kiểm tra có thay dổi và trả về true false

//void eeprom_write_outlet(uint8_t outletIndex, )

void Write_First_Byte(uint8_t value);
uint8_t Read_First_Byte(void);

uint32_t Eeprom_Get_Main_Energy(void);
void Eeprom_Update_Main_Energy(uint32_t main_energy);
//boolean eeprom_is_channel_change(uint8_t outletIndex);
void Eeprom_Initialize();
uint8_t Eeprom_Read_Outlet  (uint8_t outletIndex, uint8_t * status, uint32_t * energy, uint32_t * limitEnergy, uint32_t *workingTime);

void Eeprom_Update_Status(uint8_t outletID, uint8_t status);
void Eeprom_Update_LimitEnergy(uint8_t outletID, uint32_t limitEnergy);
void Eeprom_Update_Energy(uint8_t outletID, uint32_t energy);
void Eeprom_Update_WorkingTime(uint8_t outletID, uint32_t workingTime);
#endif

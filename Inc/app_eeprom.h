/*
 * app_eeprom.h
 *
 *  Created on: 24 Mar 2020
 *      Author: phamvanvinh
 */

#ifndef APP_EEPROM_H_
#define APP_EEPROM_H_
#include <stdint.h>

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

//boolean eeprom_is_channel_change(uint8_t outletIndex);
void eeprom_initilize();
void eeprom_write_outlet (uint8_t outletIndex, uint8_t status, uint32_t energy, uint32_t limitEnergy);
void eeprom_read_outlet  (uint8_t * outletIndex, uint8_t * status, uint32_t * energy, uint32_t * limitEnergy);

#endif

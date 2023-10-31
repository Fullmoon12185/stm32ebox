#include "modbus.h"
#include "utils_buffer.h"
//#include "utils_logger.h"

#include "app_uart.h"


#define		DEBUG_MODBUS(X)		X

// Internal Rx State
enum {
	RX_GET_ADDRESS = 0,
	RX_GET_FUNCTION_CODE,
	RX_RD_OP_GET_BYTE_CNT,
	RX_RD_OP_GET_DATA,
	RX_WR_SGL_OP_GET_ADDR,
	RX_WR_SGL_OP_GET_DATA,
	RX_WR_MUL_OP_GET_ADDR,
	RX_WR_MUL_OP_GET_QUANTITY,
	RX_GET_CRC
};

// Internal Functions
static void MODBUS_process_tx_read_operation();
static void MODBUS_process_tx_write_single_operation();
static void MODBUS_process_tx_write_multiple_operation();
static void MODBUS_process_rx_read_operation();
static void MODBUS_process_rx_write_single_operation();
static void MODBUS_process_rx_write_multiple_operation();
static uint16_t MODBUS_calculate_crc(const uint8_t* data , const uint16_t data_len);

// Internal Variables

static uint8_t modbus_tx_buffer[MODBUS_TX_BUFFER_SIZE];
static utils_buffer_t modbus_rx_buffer;
static MODBUS_t modbus_pdu_tx;
static MODBUS_t modbus_pdu_rx;
static bool modbus_pdu_is_active = false;
static uint32_t modbus_pdu_start_time = 0;
static uint8_t modbus_pdu_rx_state = RX_GET_ADDRESS;




void MODBUS_init(void){

	UART5_Init();
    utils_buffer_init(&modbus_rx_buffer, sizeof(MODBUS_t));

}

bool MODBUS_run(){
	if(modbus_pdu_is_active){
	    switch (modbus_pdu_tx.function_code) {
			case MODBUS_READ_COILS:
			case MODBUS_READ_DISCRETE_INPUT:
			case MODBUS_READ_HOLDING_REGISTER:
			case MODBUS_READ_INPUT_REGISTER:
				MODBUS_process_rx_read_operation();
				break;
			case MODBUS_WRITE_SINGLE_COIL:
			case MODBUS_WRITE_SINGLE_REGISTER:
				MODBUS_process_rx_write_single_operation();
				break;
			case MODBUS_WRITE_MULTIPLE_COILS:
			case MODBUS_WRITE_MULTIPLE_REGISTER:
				MODBUS_process_rx_write_multiple_operation();
				break;
			default:
				break;
		}
		if(MODBUS_GET_TIME_MS() - modbus_pdu_start_time > MODBUS_TIMEOUT){
//			utils_log_error("MODBUS receive timeout\r\n");
			DEBUG_MODBUS(UART3_SendToHost((uint8_t*)"MODBUS receive timeout\r\n"));
			// Disable Modbus PDU active
			modbus_pdu_is_active = false;
			// Reset Rx State
			modbus_pdu_rx_state = RX_GET_ADDRESS;
		}
	}
	return true;
}

bool MODBUS_transmit(MODBUS_t* tx_message){
	if(modbus_pdu_is_active){
		return false;
	}
	// Enable Process PDU
	modbus_pdu_is_active = true;
	// Copy tx_message to Modbus Pdu
	memcpy(&modbus_pdu_tx , tx_message, sizeof(MODBUS_t));
    // Transmit Modbus Request
    modbus_pdu_start_time = MODBUS_GET_TIME_MS();
    switch (modbus_pdu_tx.function_code) {
		case MODBUS_READ_COILS:
		case MODBUS_READ_DISCRETE_INPUT:
		case MODBUS_READ_HOLDING_REGISTER:
		case MODBUS_READ_INPUT_REGISTER:
			MODBUS_process_tx_read_operation();
			break;
		case MODBUS_WRITE_SINGLE_COIL:
		case MODBUS_WRITE_SINGLE_REGISTER:
			MODBUS_process_tx_write_single_operation();
			break;
		case MODBUS_WRITE_MULTIPLE_COILS:
		case MODBUS_WRITE_MULTIPLE_REGISTER:
			MODBUS_process_tx_write_multiple_operation();
			break;
		default:
			break;
	}
    return true;
}

bool MODBUS_receive(MODBUS_t* rx_message){
	// Check Buffer is available or not
	if(utils_buffer_is_available(&modbus_rx_buffer)){
		utils_buffer_pop(&modbus_rx_buffer, (uint8_t*) rx_message);
		return true;
	}
	return false;
}

static void MODBUS_process_tx_read_operation(){
	uint8_t idx = 0;
	// Modbus Device Address: 1 byte
	modbus_tx_buffer[idx++] = modbus_pdu_tx.address;
	// Modbus Function: 1 byte
	modbus_tx_buffer[idx++] = modbus_pdu_tx.function_code;
	// Modbus Starting Address: 2 byte
	modbus_tx_buffer[idx++] = modbus_pdu_tx.data.read_req.addr >> 8;
	modbus_tx_buffer[idx++] = modbus_pdu_tx.data.read_req.addr & 0x00FF;
	// Modbus Quantity: 2 byte
	modbus_tx_buffer[idx++] = modbus_pdu_tx.data.read_req.quantity >> 8;
	modbus_tx_buffer[idx++] = modbus_pdu_tx.data.read_req.quantity & 0x00FF;
	// Caculate CRC
	modbus_pdu_tx.crc = MODBUS_calculate_crc(modbus_tx_buffer, idx);
	// Modbus Crc: 2 byte
	modbus_tx_buffer[idx++] = modbus_pdu_tx.crc & 0x00FF;
	modbus_tx_buffer[idx++] = modbus_pdu_tx.crc >> 8;
	// Send Modbus Tx Buffer
	MODBUS_TRANSMIT(modbus_tx_buffer, idx);
}

static void MODBUS_process_tx_write_single_operation(){
	uint8_t idx = 0;
	// Modbus Device Address: 1 byte
	modbus_tx_buffer[idx++] = modbus_pdu_tx.address;
	// Modbus Function: 1 byte
	modbus_tx_buffer[idx++] = modbus_pdu_tx.function_code;
	// Modbus Starting Address: 2 byte
	modbus_tx_buffer[idx++] = modbus_pdu_tx.data.write_single_req.addr >> 8;
	modbus_tx_buffer[idx++] = modbus_pdu_tx.data.write_single_req.addr & 0x00FF;
	// Modbus Data: 2 byte
	modbus_tx_buffer[idx++] = modbus_pdu_tx.data.write_single_req.data >> 8;
	modbus_tx_buffer[idx++] = modbus_pdu_tx.data.write_single_req.data & 0x00FF;
	// Caculate CRC
	modbus_pdu_tx.crc = MODBUS_calculate_crc(modbus_tx_buffer, idx);
	// Modbus Crc: 2 byte
	modbus_tx_buffer[idx++] = modbus_pdu_tx.crc & 0x00FF;
	modbus_tx_buffer[idx++] = modbus_pdu_tx.crc >> 8;
	// Send Modbus Tx Buffer
	MODBUS_TRANSMIT(modbus_tx_buffer, idx);
}

static void MODBUS_process_tx_write_multiple_operation(){
	uint8_t idx = 0;
	// Modbus Device Address: 1 byte
	modbus_tx_buffer[idx++] = modbus_pdu_tx.address;
	// Modbus Function: 1 byte
	modbus_tx_buffer[idx++] = modbus_pdu_tx.function_code;
	// Modbus Starting Address: 2 byte
	modbus_tx_buffer[idx++] = modbus_pdu_tx.data.write_multiple_req.addr >> 8;
	modbus_tx_buffer[idx++] = modbus_pdu_tx.data.write_multiple_req.addr & 0x00FF;
	// Modbus Quantity: 2 byte
	modbus_tx_buffer[idx++] = modbus_pdu_tx.data.write_multiple_req.quantity >> 8;
	modbus_tx_buffer[idx++] = modbus_pdu_tx.data.write_multiple_req.quantity & 0x00FF;
	// Modbus Byte Count: 2 byte
	modbus_tx_buffer[idx++] = modbus_pdu_tx.data.write_multiple_req.byte_cnt >> 8;
	modbus_tx_buffer[idx++] = modbus_pdu_tx.data.write_multiple_req.byte_cnt & 0x00FF;
	// Append Data to Buffer
	// Modbus Data: N=byte_cnt bytes
	for (int var = 0; var < modbus_pdu_tx.data.write_multiple_req.byte_cnt; ++var) {
		modbus_tx_buffer[idx++] = modbus_pdu_tx.data.write_multiple_req.data[var];
	}
	// Caculate CRC
	modbus_pdu_tx.crc = MODBUS_calculate_crc(modbus_tx_buffer, idx);
	// Modbus Crc: 2 byte , Low bytes first , High Byte After
	modbus_tx_buffer[idx++] = modbus_pdu_tx.crc & 0x00FF;
	modbus_tx_buffer[idx++] = modbus_pdu_tx.crc >> 8;
	// Send Modbus Tx Buffer
	MODBUS_TRANSMIT(modbus_tx_buffer, idx);
}

static void MODBUS_process_rx_read_operation(){
	static uint8_t byte_cnt_idx = 0;
	static uint8_t crc_cnt_idx = 0;
	static uint8_t data;
	static uint8_t* crc_p;
//	static uint16_t ret_crc;
	if(MODBUS_RECEIVE_IS_AVAILABLE()){
		data = MODBUS_RECEIVE();
		switch (modbus_pdu_rx_state) {
			case RX_GET_ADDRESS:
				modbus_pdu_rx.address = data;
				modbus_pdu_rx_state = RX_GET_FUNCTION_CODE;
				break;
			case RX_GET_FUNCTION_CODE:
				modbus_pdu_rx.function_code = data;
				modbus_pdu_rx_state = RX_RD_OP_GET_BYTE_CNT;
				break;
			case RX_RD_OP_GET_BYTE_CNT:
				modbus_pdu_rx.data.read_res.byte_cnt = data;
				// Check byte_cnt
				if(modbus_pdu_rx.data.read_res.byte_cnt == 0){
					// Failed to get data -> Reset State to GET Address
					modbus_pdu_rx_state = RX_GET_ADDRESS;
				}else{
					// Reset byte_cnt_idx
					byte_cnt_idx = 0;
					modbus_pdu_rx_state = RX_RD_OP_GET_DATA;
				}
				break;
			case RX_RD_OP_GET_DATA:
				modbus_pdu_rx.data.read_res.data[byte_cnt_idx++] = data;
				// Check Get Full Data or not
				if(byte_cnt_idx == modbus_pdu_rx.data.read_res.byte_cnt){
					// Reset byte_cnt_idx
					byte_cnt_idx = 0;
					// Switch to get crc data
					modbus_pdu_rx_state = RX_GET_CRC;
				}
				break;
			case RX_GET_CRC:
				crc_p = (uint8_t*)&modbus_pdu_rx.crc;
				crc_p[crc_cnt_idx++] = data;
				if(crc_cnt_idx == 2){
					// Reset crc_idx
					crc_cnt_idx = 0;
					// Push Pdu Rx to Rx Buffer
					utils_buffer_push(&modbus_rx_buffer, &modbus_pdu_rx);
					// Reset RX State
					modbus_pdu_rx_state = RX_GET_ADDRESS;
					// Disable Pdu Active
					modbus_pdu_is_active = false;
				}
				break;
			default:
				break;
		}
	}
}
static void MODBUS_process_rx_write_single_operation(){
	static uint8_t addr_idx = 0;
	static uint8_t data_idx = 0;
	static uint8_t crc_cnt_idx = 0;
//	static uint16_t ret_crc;
	static uint8_t* crc_p;
	static uint8_t data;
	static uint8_t * addr_p;
	static uint8_t * data_p;
	if(MODBUS_RECEIVE_IS_AVAILABLE()){
		data = MODBUS_RECEIVE();
		switch (modbus_pdu_rx_state) {
			case RX_GET_ADDRESS:
				modbus_pdu_rx.address = data;
				modbus_pdu_rx_state = RX_GET_FUNCTION_CODE;
				break;
			case RX_GET_FUNCTION_CODE:
				modbus_pdu_rx.function_code = data;
				// Reset addr_idx
				addr_idx = 0;
				modbus_pdu_rx_state = RX_WR_SGL_OP_GET_ADDR;
				break;
			case RX_WR_SGL_OP_GET_ADDR:
				addr_p = (uint8_t *)&modbus_pdu_rx.data.write_single_res.addr;
				addr_p[addr_idx++] = data;
				// Check Get enough 2 bytes Address (16bits) or not
				if(addr_idx == 2){
					// Reset addr_idx
					addr_idx = 0;
					// Reset data_idx
					data_idx = 0;
					modbus_pdu_rx_state = RX_WR_SGL_OP_GET_DATA;
				}
				break;
			case RX_WR_SGL_OP_GET_DATA:
				data_p = (uint8_t *)&modbus_pdu_rx.data.write_single_res.data;
				data_p[data_idx++] = data;
				// Check Get enough 2 bytes or not
				if(data_idx == 2){
					// Reset byte_cnt_idx
					data_idx = 0;
					// Switch to get CRC data
					modbus_pdu_rx_state = RX_GET_CRC;
				}
				break;
			case RX_GET_CRC:
				crc_p = (uint8_t*)&modbus_pdu_rx.crc;
				crc_p[crc_cnt_idx++] = data;
				if(crc_cnt_idx == 2){
					// Reset crc_idx
					crc_cnt_idx = 0;
					// Push Pdu Rx to Rx Buffer
					utils_buffer_push(&modbus_rx_buffer, &modbus_pdu_rx);
					// Reset RX State
					modbus_pdu_rx_state = RX_GET_ADDRESS;
					// Disable Pdu Active
					modbus_pdu_is_active = false;
				}
				break;
			default:
				break;
		}
	}
}
static void MODBUS_process_rx_write_multiple_operation(){
	static uint8_t addr_idx = 0;
	static uint8_t quantity_idx = 0;
	static uint8_t data;
	static uint8_t * addr_p;
	static uint8_t * quantity_p;
	static uint8_t crc_cnt_idx = 0;
//	static uint16_t ret_crc;
	static uint8_t* crc_p;
	if(MODBUS_RECEIVE_IS_AVAILABLE()){
		data = MODBUS_RECEIVE();
		switch (modbus_pdu_rx_state) {
			case RX_GET_ADDRESS:
				modbus_pdu_rx.address = data;
				modbus_pdu_rx_state = RX_GET_FUNCTION_CODE;
				break;
			case RX_GET_FUNCTION_CODE:
				modbus_pdu_rx.function_code = data;
				// Reset addr_idx
				addr_idx = 0;
				modbus_pdu_rx_state = RX_WR_SGL_OP_GET_ADDR;
				break;
			case RX_WR_SGL_OP_GET_ADDR:
				addr_p = (uint8_t *)&modbus_pdu_rx.data.write_multiple_res.addr;
				addr_p[addr_idx++] = data;
				// Check Get enough 2 bytes Address (16bits) or not
				if(addr_idx == 2){
					// Reset addr_idx
					addr_idx = 0;
					// Reset data_idx
					quantity_idx = 0;
					modbus_pdu_rx_state = RX_WR_SGL_OP_GET_DATA;
				}
				break;
			case RX_WR_SGL_OP_GET_DATA:
				quantity_p = (uint8_t *)&modbus_pdu_rx.data.write_multiple_res.quantity;
				quantity_p[quantity_idx++] = data;
				// Check Get enough 2 bytes or not
				if(quantity_idx == 2){
					// Reset byte_cnt_idx
					quantity_idx = 0;
					// Switch to get CRC
					modbus_pdu_rx_state = RX_GET_CRC;
				}
				break;
			case RX_GET_CRC:
				crc_p = (uint8_t*)&modbus_pdu_rx.crc;
				crc_p[crc_cnt_idx++] = data;
				if(crc_cnt_idx == 2){
					// Reset crc_idx
					crc_cnt_idx = 0;
					// Push Pdu Rx to Rx Buffer
					utils_buffer_push(&modbus_rx_buffer, &modbus_pdu_rx);
					// Reset RX State
					modbus_pdu_rx_state = RX_GET_ADDRESS;
					// Disable Pdu Active
					modbus_pdu_is_active = false;
				}
				break;
			default:
				break;
		}
	}
}


// Internal Functions
static uint16_t MODBUS_calculate_crc(const uint8_t* data ,const uint16_t data_len){
	uint8_t * data_p = (uint8_t*)data;
	uint16_t data_length = data_len;
	static const uint16_t crc_table[] = {
		0X0000, 0XC0C1, 0XC181, 0X0140, 0XC301, 0X03C0, 0X0280, 0XC241,
		0XC601, 0X06C0, 0X0780, 0XC741, 0X0500, 0XC5C1, 0XC481, 0X0440,
		0XCC01, 0X0CC0, 0X0D80, 0XCD41, 0X0F00, 0XCFC1, 0XCE81, 0X0E40,
		0X0A00, 0XCAC1, 0XCB81, 0X0B40, 0XC901, 0X09C0, 0X0880, 0XC841,
		0XD801, 0X18C0, 0X1980, 0XD941, 0X1B00, 0XDBC1, 0XDA81, 0X1A40,
		0X1E00, 0XDEC1, 0XDF81, 0X1F40, 0XDD01, 0X1DC0, 0X1C80, 0XDC41,
		0X1400, 0XD4C1, 0XD581, 0X1540, 0XD701, 0X17C0, 0X1680, 0XD641,
		0XD201, 0X12C0, 0X1380, 0XD341, 0X1100, 0XD1C1, 0XD081, 0X1040,
		0XF001, 0X30C0, 0X3180, 0XF141, 0X3300, 0XF3C1, 0XF281, 0X3240,
		0X3600, 0XF6C1, 0XF781, 0X3740, 0XF501, 0X35C0, 0X3480, 0XF441,
		0X3C00, 0XFCC1, 0XFD81, 0X3D40, 0XFF01, 0X3FC0, 0X3E80, 0XFE41,
		0XFA01, 0X3AC0, 0X3B80, 0XFB41, 0X3900, 0XF9C1, 0XF881, 0X3840,
		0X2800, 0XE8C1, 0XE981, 0X2940, 0XEB01, 0X2BC0, 0X2A80, 0XEA41,
		0XEE01, 0X2EC0, 0X2F80, 0XEF41, 0X2D00, 0XEDC1, 0XEC81, 0X2C40,
		0XE401, 0X24C0, 0X2580, 0XE541, 0X2700, 0XE7C1, 0XE681, 0X2640,
		0X2200, 0XE2C1, 0XE381, 0X2340, 0XE101, 0X21C0, 0X2080, 0XE041,
		0XA001, 0X60C0, 0X6180, 0XA141, 0X6300, 0XA3C1, 0XA281, 0X6240,
		0X6600, 0XA6C1, 0XA781, 0X6740, 0XA501, 0X65C0, 0X6480, 0XA441,
		0X6C00, 0XACC1, 0XAD81, 0X6D40, 0XAF01, 0X6FC0, 0X6E80, 0XAE41,
		0XAA01, 0X6AC0, 0X6B80, 0XAB41, 0X6900, 0XA9C1, 0XA881, 0X6840,
		0X7800, 0XB8C1, 0XB981, 0X7940, 0XBB01, 0X7BC0, 0X7A80, 0XBA41,
		0XBE01, 0X7EC0, 0X7F80, 0XBF41, 0X7D00, 0XBDC1, 0XBC81, 0X7C40,
		0XB401, 0X74C0, 0X7580, 0XB541, 0X7700, 0XB7C1, 0XB681, 0X7640,
		0X7200, 0XB2C1, 0XB381, 0X7340, 0XB101, 0X71C0, 0X7080, 0XB041,
		0X5000, 0X90C1, 0X9181, 0X5140, 0X9301, 0X53C0, 0X5280, 0X9241,
		0X9601, 0X56C0, 0X5780, 0X9741, 0X5500, 0X95C1, 0X9481, 0X5440,
		0X9C01, 0X5CC0, 0X5D80, 0X9D41, 0X5F00, 0X9FC1, 0X9E81, 0X5E40,
		0X5A00, 0X9AC1, 0X9B81, 0X5B40, 0X9901, 0X59C0, 0X5880, 0X9841,
		0X8801, 0X48C0, 0X4980, 0X8941, 0X4B00, 0X8BC1, 0X8A81, 0X4A40,
		0X4E00, 0X8EC1, 0X8F81, 0X4F40, 0X8D01, 0X4DC0, 0X4C80, 0X8C41,
		0X4400, 0X84C1, 0X8581, 0X4540, 0X8701, 0X47C0, 0X4680, 0X8641,
		0X8201, 0X42C0, 0X4380, 0X8341, 0X4100, 0X81C1, 0X8081, 0X4040 };

	uint8_t temp;
	uint16_t crc = 0xFFFF;
//	uint16_t data_idx = 0;
	while (data_length--)
	{
		temp = *data_p++ ^ crc;
		crc >>= 8;
		crc  ^= crc_table[temp];
	}
	return crc;
}

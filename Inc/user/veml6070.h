
#include "main.h"


#define I2C_ADDR_VEML6070_WRITE 0x70
#define I2C_ADDR_VEML6070_READ_LSB 0x71
#define I2C_ADDR_VEML6070_READ_MSB 0x73


#define UV_CONF_UV_IT_MASK 0x70
#define UV_CONF_HD_NORMAL 0x00
#define UV_CONF_HD_HIGH 0x08
#define UV_CONF_UV_TRIG 0x04
#define UV_CONF_UV_AF_ENABLE 0x02
#define UV_CONF_UV_AF_DISABLE 0x00
#define UV_CONF_SD_ON 0x00
#define UV_CONF_SD_OFF 0x01

#define UV_CONF_1T 0x01
#define UV_CONF_2T 0x02
#define UV_CONF_4T 0x03



#define UV_DATA_1T_EXTREME 2055
#define UV_DATA_1T_VERY_HIGH 1494
#define UV_DATA_1T_HIGH 1121
#define UV_DATA_1T_MODERATE 561
#define UV_DATA_1T_LOW 0

#define UV_DATA_2T_EXTREME 4109
#define UV_DATA_2T_VERY_HIGH 2989
#define UV_DATA_2T_HIGH 2242
#define UV_DATA_2T_MODERATE 1121
#define UV_DATA_2T_LOW 0

#define UV_DATA_4T_EXTREME 8217
#define UV_DATA_4T_VERY_HIGH 5977
#define UV_DATA_4T_HIGH 4483
#define UV_DATA_4T_MODERATE 2241
#define UV_DATA_4T_LOW 0




typedef struct __VEML6070_TypeDef
{
	uint8_t config_ack;
	uint8_t config_ack_thd;
	uint8_t config_it;
	uint8_t config_sd;
	
	HAL_StatusTypeDef i2c_status;
	
	uint8_t tx_data;
	uint8_t rx_data[2];
	
	uint16_t raw_data;
	uint8_t uv_level;
	
	uint32_t uv_data_accumulation;
	double uv_data_average;
	
	double gain;
	uint32_t gain_uint32;
} VEML6070_TypeDef;




void VEML6070_Initialization(I2C_HandleTypeDef *hi2c,VEML6070_TypeDef *p_handle_veml6070);
void VEML6070_Measurement_Routine(VEML6070_TypeDef *p_handle_veml6070);
void VEML6070_Measure(VEML6070_TypeDef *p_handle_veml6070);
uint8_t VEML6070_UV_Level_Calculation(uint16_t uv_data, VEML6070_TypeDef *p_handle_veml6070);





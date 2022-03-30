
#include "main.h"
#include "string.h"

#include "user/veml6070.h"
#include "user/user_app.h"

static I2C_HandleTypeDef *user_hi2c;

void VEML6070_Initialization(I2C_HandleTypeDef *hi2c, VEML6070_TypeDef *p_handle_veml6070)
{
	user_hi2c = hi2c;

	p_handle_veml6070->tx_data = (p_handle_veml6070->config_ack << 5) | (p_handle_veml6070->config_ack_thd << 4) | (p_handle_veml6070->config_it << 2) | 0x02 |p_handle_veml6070->config_sd;

	p_handle_veml6070->i2c_status = HAL_I2C_Master_Transmit(user_hi2c , I2C_ADDR_VEML6070_WRITE ,  &p_handle_veml6070->tx_data, 1 , 0xFF);
	
}

void VEML6070_Measurement_Routine(VEML6070_TypeDef *p_handle_veml6070)
{

	VEML6070_Measure(p_handle_veml6070);
	p_handle_veml6070->raw_data = (uint16_t) ((double) p_handle_veml6070->raw_data * p_handle_veml6070->gain);
	p_handle_veml6070->uv_level = VEML6070_UV_Level_Calculation(p_handle_veml6070->raw_data,p_handle_veml6070);

}


void VEML6070_Measure(VEML6070_TypeDef *p_handle_veml6070)
{
	p_handle_veml6070->i2c_status = HAL_I2C_Master_Receive(user_hi2c , I2C_ADDR_VEML6070_READ_MSB , &p_handle_veml6070->rx_data[1] , 1 , 0xFF);
	p_handle_veml6070->i2c_status = HAL_I2C_Master_Receive(user_hi2c , I2C_ADDR_VEML6070_READ_LSB , &p_handle_veml6070->rx_data[0] , 1 , 0xFF);
	
	p_handle_veml6070->raw_data = ((uint16_t)p_handle_veml6070->rx_data[1] << 8) | ((uint16_t)p_handle_veml6070->rx_data[0]);

}

uint8_t VEML6070_UV_Level_Calculation(uint16_t uv_data, VEML6070_TypeDef *p_handle_veml6070)
{
	uint8_t uv_level;

	switch(p_handle_veml6070->config_it)
	{
		case UV_CONF_1T:
			
			if(uv_data >= UV_DATA_1T_EXTREME)
				uv_level = UV_LEVEL_EXTREME;
			else if(uv_data >= UV_DATA_1T_VERY_HIGH)
				uv_level = UV_LEVEL_VERY_HIGH;
			else if(uv_data >= UV_DATA_1T_HIGH)
				uv_level = UV_LEVEL_HIGH;
			else if(uv_data >= UV_DATA_1T_MODERATE)
				uv_level = UV_LEVEL_MODERATE;
			else if(uv_data >= UV_DATA_1T_LOW)
				uv_level = UV_LEVEL_LOW;
			
			break;
		case UV_CONF_2T:
			
			if(uv_data >= UV_DATA_2T_EXTREME)
				uv_level = UV_LEVEL_EXTREME;
			else if(uv_data >= UV_DATA_2T_VERY_HIGH)
				uv_level = UV_LEVEL_VERY_HIGH;
			else if(uv_data >= UV_DATA_2T_HIGH)
				uv_level = UV_LEVEL_HIGH;
			else if(uv_data >= UV_DATA_2T_MODERATE)
				uv_level = UV_LEVEL_MODERATE;
			else if(uv_data >= UV_DATA_2T_LOW)
				uv_level = UV_LEVEL_LOW;
						
			break;
		case UV_CONF_4T:
			
			if(uv_data >= UV_DATA_4T_EXTREME)
				uv_level = UV_LEVEL_EXTREME;
			else if(uv_data >= UV_DATA_4T_VERY_HIGH)
				uv_level = UV_LEVEL_VERY_HIGH;
			else if(uv_data >= UV_DATA_4T_HIGH)
				uv_level = UV_LEVEL_HIGH;
			else if(uv_data >= UV_DATA_4T_MODERATE)
				uv_level = UV_LEVEL_MODERATE;
			else if(uv_data >= UV_DATA_4T_LOW)
				uv_level = UV_LEVEL_LOW;
						
			break;
		default:
			break;
	}
	
	return uv_level;
}

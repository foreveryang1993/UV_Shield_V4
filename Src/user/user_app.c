

#include "user/user_app.h"
#include "user/si1145.h"
#include "user/veml6070.h"
#include "user/led.h"
#include "user/cmd_parsing.h"

SI1145_TypeDef handle_si1145;
VEML6070_TypeDef handle_veml6070;
LED_Config_TypeDef user_led_config;

#define CDC_TX_BUFFER_SIZE 300

static uint8_t cdc_tx_buffer[CDC_TX_BUFFER_SIZE];
static uint16_t cdc_tx_buffer_len;
static uint8_t user_usb_status;

uint8_t tim_trig_status = TIM_TRIGGER_RESET;

static uint32_t measurement_count = 0;

extern MSG_Table_TypeDef user_msg_table[];
extern const char user_msg_instruction[];

TIM_HandleTypeDef *p_user_led_htim;
TIM_HandleTypeDef *p_user_trig_htim;
I2C_HandleTypeDef *p_user_sensor_hi2c;
TIM_OC_InitTypeDef *p_user_led_sConfigOC;
IWDG_HandleTypeDef *p_user_hiwdg;

HAL_StatusTypeDef flash_status;
static FLASH_EraseInitTypeDef flash_erase_init;
static uint32_t flash_page_error;

HAL_StatusTypeDef user_operation_status;

void While_Loop_Routine(void)
{
	if(measurement_count >= 0xFFFFFFFF)
	{
		measurement_count = 1;
	}
	else
	{
		measurement_count++;
	}
	
	VEML6070_Measurement_Routine(&handle_veml6070);
	handle_veml6070.uv_data_accumulation += handle_veml6070.raw_data;
	handle_veml6070.uv_data_average = ((double) handle_veml6070.uv_data_accumulation) / measurement_count;

	
	if(handle_si1145.int_output_status == INT_OUTPUT_SET)
	{
		Si1145_Get_Data(&handle_si1145);
		handle_si1145.uv_accumulation += handle_si1145.aux_data;
		handle_si1145.uv_index_average = ((double) handle_si1145.uv_accumulation) / 100 / measurement_count;
	}
		
			
	switch(user_led_config.indication_mode)
	{
		case LED_MOODE_NULL:
			user_led_config.r_value = 0;
			user_led_config.g_value = 0;
			user_led_config.b_value = 0;
			LED_PWM_Set_Value(&user_led_config,p_user_led_sConfigOC);
		break;
		case LED_MOODE_SI1145:
			LED_UV_Level_Indication(&user_led_config,p_user_led_sConfigOC,handle_si1145.uv_level);
		break;				
		case LED_MOODE_VEML6070:
			LED_UV_Level_Indication(&user_led_config,p_user_led_sConfigOC,handle_veml6070.uv_level);
		break;
		default:
			user_led_config.r_value = 50;
			user_led_config.g_value = 50;
			user_led_config.b_value = 50;
			LED_PWM_Set_Value(&user_led_config,p_user_led_sConfigOC);
		break;
		
	}
	
	
	if(user_led_config.indication_mode != LED_MOODE_NULL)
	{	
		cdc_tx_buffer_len = (uint16_t) sprintf((char*) cdc_tx_buffer,"\r\n[Count] %u",measurement_count);
		user_usb_status |= CDC_Transmit_FS(cdc_tx_buffer, cdc_tx_buffer_len);
		HAL_Delay(5);
			
		cdc_tx_buffer_len = (uint16_t) sprintf((char*) cdc_tx_buffer,"\r\n[Si1145] UV Index: %05.2f UV Level: %u",handle_si1145.uv_index,handle_si1145.uv_level);
		user_usb_status |= CDC_Transmit_FS(cdc_tx_buffer, cdc_tx_buffer_len);
		HAL_Delay(5);
		
		cdc_tx_buffer_len = (uint16_t) sprintf((char*) cdc_tx_buffer,"\r\nAccumulation: %d UV Index Average: %05.2f",handle_si1145.uv_accumulation,handle_si1145.uv_index_average);
		user_usb_status |= CDC_Transmit_FS(cdc_tx_buffer, cdc_tx_buffer_len);
		HAL_Delay(5);
		
		cdc_tx_buffer_len = (uint16_t) sprintf((char*) cdc_tx_buffer,"\r\n[VEML6070] Raw Data: %06d UV Level: %u",handle_veml6070.raw_data,handle_veml6070.uv_level);
		user_usb_status |= CDC_Transmit_FS(cdc_tx_buffer, cdc_tx_buffer_len);
		HAL_Delay(5);

		cdc_tx_buffer_len = (uint16_t) sprintf((char*) cdc_tx_buffer,"\r\nAccumulation: %d UV Data Average: %08.1f",handle_veml6070.uv_data_accumulation,handle_veml6070.uv_data_average);
		user_usb_status |= CDC_Transmit_FS(cdc_tx_buffer, cdc_tx_buffer_len);
		HAL_Delay(5);
		
		cdc_tx_buffer_len = (uint16_t) sprintf((char*) cdc_tx_buffer,"\r\n");
		user_usb_status |= CDC_Transmit_FS(cdc_tx_buffer, cdc_tx_buffer_len);
		HAL_Delay(5);

	}

	user_operation_status = handle_si1145.i2c_status & handle_veml6070.i2c_status & user_usb_status & flash_status;
	Operation_Status_Check(user_operation_status);

}

void Operation_Status_Check(HAL_StatusTypeDef status)
{
	if(status == 0)
	{
		HAL_IWDG_Refresh(p_user_hiwdg);
	}
	else
	{
		user_led_config.r_value = 5;
		user_led_config.g_value = 5;
		user_led_config.b_value = 5;
		LED_PWM_Set_Value(&user_led_config,p_user_led_sConfigOC);
		
		cdc_tx_buffer_len = (uint16_t) sprintf((char*) cdc_tx_buffer,"Error!");
		user_usb_status |= CDC_Transmit_FS(cdc_tx_buffer, cdc_tx_buffer_len);
		HAL_Delay(5);
	}
}


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	switch(GPIO_Pin)
	{
		case SI1145_INT_Pin:
			handle_si1145.int_output_status = INT_OUTPUT_SET;
		
		break;
		case VEML6070_ACK_Pin:

		
		break;
		case USER_BTN_Pin:
				
			user_led_config.indication_mode = user_led_config.indication_mode + 1;
			if(user_led_config.indication_mode >= 3)
				user_led_config.indication_mode = 0;
			
		break;
		default:
		
		break;
	}
}

void Trigger_Timer_IT_Callback(void)
{
	tim_trig_status = TIM_TRIGGER_SET;
}

void MSG_Send_Instruction(void)
{
	cdc_tx_buffer_len = (uint16_t) sprintf((char*) cdc_tx_buffer,"%s",user_msg_instruction);
	user_usb_status |= CDC_Transmit_FS(cdc_tx_buffer, cdc_tx_buffer_len);
	HAL_Delay(5);

	cdc_tx_buffer_len = (uint16_t) sprintf((char*) cdc_tx_buffer,"\r\n[SI1145] Gain: %07.3f",handle_si1145.gain);
	user_usb_status |= CDC_Transmit_FS(cdc_tx_buffer, cdc_tx_buffer_len);
	HAL_Delay(5);
	
	cdc_tx_buffer_len = (uint16_t) sprintf((char*) cdc_tx_buffer,"\r\n[VEML6070] Gain: %07.3f\r\n\r\n",handle_veml6070.gain);
	user_usb_status |= CDC_Transmit_FS(cdc_tx_buffer, cdc_tx_buffer_len);
	HAL_Delay(5);
}

void MSG_Send(uint8_t index)
{
	cdc_tx_buffer_len = (uint16_t) sprintf((char*) cdc_tx_buffer,"\r\n%s",(const char*)user_msg_table[index].table_string);
	user_usb_status |= CDC_Transmit_FS(cdc_tx_buffer, cdc_tx_buffer_len);
	HAL_Delay(5);
}

 
HAL_StatusTypeDef Flash_Read(uint32_t address, uint32_t *ptr_data,uint32_t size)
{
	uint32_t k;
	HAL_StatusTypeDef status = HAL_OK;
	address = address + FLASH_USER_CONFIG_OFFSET;
	for(k=0;k < size;k++)
	{
		ptr_data[k] = *(uint32_t*)(address+k);
	}
	return status;
}


HAL_StatusTypeDef Flash_User_Config_Write(void)
{
	HAL_StatusTypeDef status = HAL_OK;
	
	flash_erase_init.NbPages = 1;
	flash_erase_init.PageAddress = FLASH_USER_CONFIG_OFFSET;
	flash_erase_init.TypeErase = FLASH_TYPEERASE_PAGES;
	
	status |= HAL_FLASH_Unlock();
	status |= HAL_FLASHEx_Erase(&flash_erase_init, &flash_page_error);
	
	
	status |= HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, 
		FLASH_USER_CONFIG_OFFSET + FLASH_USER_CONFIG_SI1145_GAIN,(uint64_t)(handle_si1145.gain_uint32));
	
	status |= HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, 
		FLASH_USER_CONFIG_OFFSET + FLASH_USER_CONFIG_VEML6070_GAIN,(uint64_t)(handle_veml6070.gain_uint32));
	
	
	status |= HAL_FLASH_Lock(); 
	
	return status;
	
}


void LED_Init(TIM_OC_InitTypeDef *p_sConfigOC)
{
	/*LED Initialization*/

	user_led_config.r_value = 10;
	user_led_config.g_value = 10;
	user_led_config.b_value = 10;
	user_led_config.r_tim = p_user_led_htim;
	user_led_config.g_tim = p_user_led_htim;
	user_led_config.b_tim = p_user_led_htim;
	user_led_config.r_tim_ch = TIM_CHANNEL_1;
	user_led_config.g_tim_ch = TIM_CHANNEL_2;
	user_led_config.b_tim_ch = TIM_CHANNEL_3;
	user_led_config.indication_mode = LED_MOODE_SI1145;
	
	LED_PWM_Set_Value(&user_led_config,p_sConfigOC);
}

void Sensor_Init(void)
{
	
	/*VEML6070 Initialization*/
	handle_veml6070.config_ack = 0;
	handle_veml6070.config_ack_thd = 0;
	handle_veml6070.config_it = UV_CONF_4T;
	handle_veml6070.config_sd = 0;
	handle_veml6070.uv_data_accumulation = 0;
	handle_veml6070.uv_data_average = 0;
	
	VEML6070_Initialization(p_user_sensor_hi2c,&handle_veml6070);
//	VEML6070_Measurement_Routine(&handle_veml6070);

	/*SI1145 Initialization*/

	Si1145_Init(p_user_sensor_hi2c,&handle_si1145);
	handle_si1145.uv_accumulation = 0;
	handle_si1145.uv_index_average = 0;

//	Si1145_Get_Data(&handle_si1145);

}

void User_Initialization(void)
{
	LED_Init(p_user_led_sConfigOC);
	HAL_Delay(500);
	
	/*Load Configuration*/
	flash_status |= Flash_Read(FLASH_USER_CONFIG_SI1145_GAIN, &handle_si1145.gain_uint32,1);
	flash_status |= Flash_Read(FLASH_USER_CONFIG_VEML6070_GAIN, &handle_veml6070.gain_uint32,1);
	
	if(handle_si1145.gain_uint32 <= 0 || handle_si1145.gain_uint32 >= 1e6)
	{
		handle_si1145.gain = 1;
		handle_si1145.gain_uint32 = handle_si1145.gain * 1e3;
		flash_status |= Flash_User_Config_Write();
	}
	else
	{
		handle_si1145.gain = (double)(handle_si1145.gain_uint32) / 1e3;
	}
	
	if(handle_veml6070.gain_uint32 <= 0 || handle_veml6070.gain_uint32 >= 1e6)
	{
		handle_veml6070.gain = 1;
		handle_veml6070.gain_uint32 = handle_veml6070.gain * 1e3;
		flash_status |= Flash_User_Config_Write();
	}
	else
	{
		handle_veml6070.gain = (double)(handle_veml6070.gain_uint32) / 1e3;
	}

	
	Sensor_Init();
	user_operation_status = handle_si1145.i2c_status & handle_veml6070.i2c_status & user_usb_status & flash_status;
	Operation_Status_Check(user_operation_status);
	
	
	if(user_operation_status == HAL_OK)
	{
//		user_led_config.r_value = 10;
//		user_led_config.g_value = 0;
//		user_led_config.b_value = 0;
//		LED_PWM_Set_Value(&user_led_config,p_user_led_sConfigOC);
//		
//		HAL_Delay(300);
//		
//		user_led_config.r_value = 0;
//		user_led_config.g_value = 10;
//		user_led_config.b_value = 0;
//		LED_PWM_Set_Value(&user_led_config,p_user_led_sConfigOC);
//		
//		HAL_Delay(300);
//		
//		user_led_config.r_value = 0;
//		user_led_config.g_value = 0;
//		user_led_config.b_value = 10;
//		LED_PWM_Set_Value(&user_led_config,p_user_led_sConfigOC);
//		
//		HAL_Delay(300);
		
		LED_UV_Level_Indication(&user_led_config,p_user_led_sConfigOC,UV_LEVEL_LOW);
		HAL_Delay(200);
		LED_UV_Level_Indication(&user_led_config,p_user_led_sConfigOC,UV_LEVEL_MODERATE);
		HAL_Delay(200);
		LED_UV_Level_Indication(&user_led_config,p_user_led_sConfigOC,UV_LEVEL_HIGH);
		HAL_Delay(200);
		LED_UV_Level_Indication(&user_led_config,p_user_led_sConfigOC,UV_LEVEL_VERY_HIGH);
		HAL_Delay(200);
		LED_UV_Level_Indication(&user_led_config,p_user_led_sConfigOC,UV_LEVEL_EXTREME);
		HAL_Delay(200);

		
	}
	
	
	user_led_config.r_value = 10;
	user_led_config.g_value = 10;
	user_led_config.b_value = 10;
	LED_PWM_Set_Value(&user_led_config,p_user_led_sConfigOC);
	
	HAL_TIM_Base_Start_IT(p_user_trig_htim);

}



#include "main.h"
#include "usbd_cdc_if.h"


#define UV_LEVEL_EXTREME 0x05
#define UV_LEVEL_VERY_HIGH 0x04
#define UV_LEVEL_HIGH 0x03
#define UV_LEVEL_MODERATE 0x02
#define UV_LEVEL_LOW 0x01


#define TIM_TRIGGER_RESET 0x00
#define TIM_TRIGGER_SET 0x01


#define FLASH_USER_CONFIG_OFFSET 0x08018000

#define FLASH_USER_CONFIG_SI1145_GAIN 0x00000000
#define FLASH_USER_CONFIG_VEML6070_GAIN 0x00000004


void While_Loop_Routine(void);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void Trigger_Timer_IT_Callback(void);
void MSG_Send_Instruction(void);
void MSG_Send(uint8_t index);
void LED_Init(TIM_OC_InitTypeDef *p_sConfigOC);
void Sensor_Init(void);
void User_Initialization(void);
void Operation_Status_Check(HAL_StatusTypeDef status);

//HAL_StatusTypeDef Flash_Erase(uint32_t address,uint32_t size);
//HAL_StatusTypeDef Flash_Write(uint32_t address, uint32_t *ptr_data,uint32_t size);
HAL_StatusTypeDef Flash_Read(uint32_t address, uint32_t *ptr_data,uint32_t size);

HAL_StatusTypeDef Flash_User_Config_Write(void);

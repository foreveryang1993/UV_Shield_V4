#include "main.h"


typedef struct __LED_Config_TypeDef
{
	uint32_t r_value;
	uint32_t g_value;
	uint32_t b_value;
	
	TIM_HandleTypeDef *r_tim;
	TIM_HandleTypeDef *g_tim;
	TIM_HandleTypeDef *b_tim;
	
	uint32_t r_tim_ch;
	uint32_t g_tim_ch;
	uint32_t b_tim_ch;

	uint8_t indication_mode;
	
	
} LED_Config_TypeDef;



void LED_PWM_Set_Value(LED_Config_TypeDef *p_led_config,TIM_OC_InitTypeDef *p_sConfigOC);
void LED_UV_Level_Indication(LED_Config_TypeDef *p_led_config,TIM_OC_InitTypeDef *p_sConfigOC,uint8_t uv_level);


#define LED_MOODE_NULL 0
#define LED_MOODE_SI1145 1
#define LED_MOODE_VEML6070 2

//#define LED_MODE_OFF 0x00
//#define LED_MODE_UVI 0x01
//#define LED_MODE_UV_DOSE 0x02


//#define LED_POWER_OFF 0x10
//#define LED_POWER_ON 0x20
//#define LED_POWER_MASK 0x30


//#define LED_COLOR_BLUE 0x01
//#define LED_COLOR_GREEN 0x02
//#define LED_COLOR_CYAN 0x03
//#define LED_COLOR_RED 0x04
//#define LED_COLOR_MAGENTA 0x05
//#define LED_COLOR_YELLOW 0x06
//#define LED_COLOR_WHITE 0x07
//#define LED_COLOR_OFF 0x08
//#define LED_COLOR_MASK 0x0F
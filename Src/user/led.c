
#include "user/led.h"
#include "user/user_app.h"

void LED_PWM_Set_Value(LED_Config_TypeDef *p_led_config,TIM_OC_InitTypeDef *p_sConfigOC)
{

	p_sConfigOC->Pulse = p_led_config->r_value;
	HAL_TIM_PWM_Stop(p_led_config->r_tim, p_led_config->r_tim_ch);
	HAL_TIM_PWM_ConfigChannel(p_led_config->r_tim, p_sConfigOC, p_led_config->r_tim_ch);
	HAL_TIM_PWM_Start(p_led_config->r_tim, p_led_config->r_tim_ch);

	
	p_sConfigOC->Pulse = p_led_config->g_value;
	HAL_TIM_PWM_Stop(p_led_config->g_tim, p_led_config->g_tim_ch);
	HAL_TIM_PWM_ConfigChannel(p_led_config->g_tim, p_sConfigOC, p_led_config->g_tim_ch);
	HAL_TIM_PWM_Start(p_led_config->g_tim, p_led_config->g_tim_ch);

	
	p_sConfigOC->Pulse = p_led_config->b_value;
	HAL_TIM_PWM_Stop(p_led_config->b_tim, p_led_config->b_tim_ch);
	HAL_TIM_PWM_ConfigChannel(p_led_config->b_tim, p_sConfigOC, p_led_config->b_tim_ch);
	HAL_TIM_PWM_Start(p_led_config->b_tim, p_led_config->b_tim_ch);
}

void LED_UV_Level_Indication(LED_Config_TypeDef *p_led_config,TIM_OC_InitTypeDef *p_sConfigOC,uint8_t uv_level)
{
	switch(uv_level)
	{
		case UV_LEVEL_LOW:
			p_led_config->r_value = 0;
			p_led_config->g_value = 40;
			p_led_config->b_value = 0;
			LED_PWM_Set_Value(p_led_config,p_sConfigOC);
			
		break;
		case UV_LEVEL_MODERATE:
			p_led_config->r_value = 40;
			p_led_config->g_value = 10;
			p_led_config->b_value = 0;
			LED_PWM_Set_Value(p_led_config,p_sConfigOC);

		break;
		case UV_LEVEL_HIGH:
			p_led_config->r_value = 45;
			p_led_config->g_value = 5;
			p_led_config->b_value = 0;
			LED_PWM_Set_Value(p_led_config,p_sConfigOC);
			
		break;
		case UV_LEVEL_VERY_HIGH:
			p_led_config->r_value = 55;
			p_led_config->g_value = 0;
			p_led_config->b_value = 0;
			LED_PWM_Set_Value(p_led_config,p_sConfigOC);
			
		break;
		case UV_LEVEL_EXTREME:
			p_led_config->r_value = 35;
			p_led_config->g_value = 0;
			p_led_config->b_value = 25;
			LED_PWM_Set_Value(p_led_config,p_sConfigOC);
			
		break;
		
	}
}


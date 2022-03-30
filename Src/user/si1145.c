
#include "user/si1145.h"

/*Global variable*/
static I2C_HandleTypeDef *user_hi2c;


/*I2C functions*/
char Si1145_I2C_Mem_Read(SI1145_TypeDef *p_handle_si1145,uint8_t register_addr)
{	  
	uint8_t DATA_Rx;
	p_handle_si1145->i2c_status |= HAL_I2C_Mem_Read(user_hi2c,I2C_ADDR_SI1145, register_addr,1,&DATA_Rx,1, 0xFFFF);
	return DATA_Rx;
}

void Si1145_I2C_Mem_Read_Multibyte(SI1145_TypeDef *p_handle_si1145,uint8_t register_addr,uint8_t *DATA_Rx,unsigned int data_num)
{
	p_handle_si1145->i2c_status |= HAL_I2C_Mem_Read(user_hi2c,I2C_ADDR_SI1145, register_addr , 1 , DATA_Rx ,data_num, 0xFFFF);
}

void Si1145_I2C_Mem_Write(SI1145_TypeDef *p_handle_si1145,unsigned int register_addr,uint8_t DATA_Tx)
{
	p_handle_si1145->i2c_status |= HAL_I2C_Mem_Write(user_hi2c,I2C_ADDR_SI1145, register_addr, 1, &DATA_Tx,1, 0x00FF);

}

void Si1145_I2C_Mem_Write_Multibyte(SI1145_TypeDef *p_handle_si1145,uint8_t register_addr,uint8_t *DATA_Tx,unsigned int data_num)
{
	p_handle_si1145->i2c_status |= HAL_I2C_Mem_Write(user_hi2c,I2C_ADDR_SI1145, register_addr , 1 , DATA_Tx ,data_num, 0xFFFF);
}

/*Si1145 functions*/
void Si1145_Parameter_Set(SI1145_TypeDef *p_handle_si1145,char reg,char data_set)
{
	reg |= 0xA0;
	Si1145_I2C_Mem_Write(p_handle_si1145,REG_PARAM_WR,data_set);
	Si1145_I2C_Mem_Write(p_handle_si1145,REG_COMMAND,reg);
}

void Si1145_Send_Command(SI1145_TypeDef *p_handle_si1145,unsigned int data_set)
{
	Si1145_I2C_Mem_Write(p_handle_si1145,REG_COMMAND,data_set);
}

void Si1145_Init(I2C_HandleTypeDef *hi2c,SI1145_TypeDef *p_handle_si1145)
{ 

	uint8_t k;
	
	user_hi2c = hi2c;
	
  //SI114X_CAL_S si114x_cal;

  // Turn off RTC
  Si1145_I2C_Mem_Write(p_handle_si1145,REG_MEAS_RATE,     0 );
  Si1145_I2C_Mem_Write(p_handle_si1145,REG_PS_RATE,       0 );
  Si1145_I2C_Mem_Write(p_handle_si1145,REG_ALS_RATE,      0 );

  // Note that the Si114x_Reset() actually performs the following functions:
  //     1. Pauses all prior measurements
  //     2. Clear  i2c registers that need to be cleared
	//     3. Clears irq status to make sure INT* is negated
	//     4. Delays 10 ms
  //     5. Sends HW Key
    
	Si114x_Reset(p_handle_si1145);

  // Program LED Currents

  uint8_t i21, i3; 

  i21 = (0x0f<<4) + 0x0f;
  i3  = 0x00;
  Si1145_I2C_Mem_Write(p_handle_si1145,REG_PS_LED21,      i21);
  Si1145_I2C_Mem_Write(p_handle_si1145,REG_PS_LED3 ,      i3);

  // UV Coefficients
    
	for(k=0;k<4;k++)
	{
		Si1145_I2C_Mem_Write(p_handle_si1145,(k+0x13),UV_COEF[k]);
	}

  // Initialize CHLIST Parameter from caller to enable measurement    
  // Valid Tasks are: ALS_VIS_TASK, ALS_IR_TASK, PS1_TASK
  //                  PS2_TASK, PS3_TASK and AUX_TASK
  // However, if we are passed a 'negative' task, we will
  // turn on ALS_IR, ALS_VIS and PS1. Otherwise, we will use the 
  // task list specified by the caller.
    
	Si1145_Parameter_Set(p_handle_si1145,PARAM_CH_LIST,0x83);
    
  // Set IRQ Modes and INT CFG to interrupt on every sample
    
	Si1145_I2C_Mem_Write(p_handle_si1145,REG_INT_CFG,ICG_INTOE);  

  // If UV task is enabled, enable PS3
	Si1145_I2C_Mem_Write(p_handle_si1145,REG_IRQ_ENABLE,IE_ALS_EVRYSAMPLE);
	//Si1145_I2C_Mem_Write(p_handle_si1145,REG_IRQ_ENABLE,IE_ALS_EVRYSAMPLE + IE_PS1_EVRYSAMPLE + IE_PS2_EVRYSAMPLE + IE_PS3_EVRYSAMPLE );  

	Si1145_Parameter_Set(p_handle_si1145, PARAM_PS1_ADC_MUX,            MUX_LARGE_IR);
	Si1145_Parameter_Set(p_handle_si1145, PARAM_PS2_ADC_MUX,            MUX_LARGE_IR);
	Si1145_Parameter_Set(p_handle_si1145, PARAM_IR_ADC_MUX,             MUX_SMALL_IR);

	Si1145_Parameter_Set(p_handle_si1145, PARAM_PS_ADC_GAIN,            0);
	Si1145_Parameter_Set(p_handle_si1145, PARAM_ALSIR_ADC_GAIN,         0);
	Si1145_Parameter_Set(p_handle_si1145, PARAM_ALSVIS_ADC_GAIN,        0);
    
	Si1145_Parameter_Set(p_handle_si1145, PARAM_PSLED12_SELECT,         (LED2_EN<<4)+LED1_EN);
	Si1145_Parameter_Set(p_handle_si1145, PARAM_PSLED3_SELECT,          LED3_EN);
    
	Si1145_Parameter_Set(p_handle_si1145, PARAM_PS_ADC_COUNTER,         RECCNT_511);
	Si1145_Parameter_Set(p_handle_si1145, PARAM_ALSIR_ADC_COUNTER,      RECCNT_511);
	Si1145_Parameter_Set(p_handle_si1145, PARAM_ALSVIS_ADC_COUNTER,     RECCNT_511);

	Si1145_Parameter_Set(p_handle_si1145, PARAM_PS_ADC_MISC,            RANGE_EN*1 + PS_MEAS_MODE);
	Si1145_Parameter_Set(p_handle_si1145, PARAM_ALSIR_ADC_MISC,         RANGE_EN*1 );
	Si1145_Parameter_Set(p_handle_si1145, PARAM_ALSVIS_ADC_MISC,        RANGE_EN*1);

  // Set up how often the device wakes up to make measurements    
	Si1145_I2C_Mem_Write(p_handle_si1145,REG_MEAS_RATE_MSB, (32000&0xff00)>>8);
	Si1145_I2C_Mem_Write(p_handle_si1145,REG_MEAS_RATE_LSB, (32000&0x00ff));

  // Enable Autonomous Operation
	Si1145_Send_Command(p_handle_si1145,0x0F);

}

void Si114x_Reset(SI1145_TypeDef *p_handle_si1145)
{
  /*===========================================================
	Do not access the Si114x earlier than 25 ms from power-up. 
	Uncomment the following lines if Si114x_Reset() is the first
	instruction encountered, and if your system MCU boots up too 
	quickly.
	===========================================================*/

	HAL_Delay(10);

	Si1145_I2C_Mem_Write(p_handle_si1145, REG_MEAS_RATE,  0x00);
    
	//Si114xPauseAll(si114x_handle);////////////
	Si1145_Send_Command(p_handle_si1145,0x0B);

  // The clearing of the registers could be redundant, but it is okay.
  // This is to make sure that these registers are cleared.
	Si1145_I2C_Mem_Write(p_handle_si1145, REG_MEAS_RATE,  0x00);
	Si1145_I2C_Mem_Write(p_handle_si1145, REG_IRQ_ENABLE, 0x00);
	Si1145_I2C_Mem_Write(p_handle_si1145, REG_IRQ_MODE1,  0x00);
	Si1145_I2C_Mem_Write(p_handle_si1145, REG_IRQ_MODE2,  0x00);
	Si1145_I2C_Mem_Write(p_handle_si1145, REG_INT_CFG  ,  0x00);
	Si1145_I2C_Mem_Write(p_handle_si1145, REG_IRQ_STATUS, 0xFF);

  // Perform the Reset Command
	Si1145_I2C_Mem_Write(p_handle_si1145, REG_COMMAND, 1);

  // Delay for 10 ms. This delay is needed to allow the Si114x
  // to perform internal reset sequence. 
  HAL_Delay(10);

  // Write Hardware Key
	Si1145_I2C_Mem_Write(p_handle_si1145, REG_HW_KEY, HW_KEY_VAL0);
}

void Si1145_Get_Data(SI1145_TypeDef *p_handle_si1145)
{		
	uint8_t k,data_temp[12],irq_status;

	p_handle_si1145->int_output_status = INT_OUTPUT_RESET;
	
	irq_status = Si1145_I2C_Mem_Read(p_handle_si1145,REG_IRQ_STATUS);
	Si1145_I2C_Mem_Write(p_handle_si1145,REG_IRQ_STATUS,irq_status);

	if(irq_status != 0x00)
	{
		
		Si1145_I2C_Mem_Read_Multibyte(p_handle_si1145,0x22,&data_temp[0],12);
		
		for(k=0;k<6;k++)
		{   
			p_handle_si1145->raw_data[k] = ((uint16_t)data_temp[(k*2+1)] << 8) + ((uint16_t)data_temp[(k*2)]);
		}
		
		p_handle_si1145->aux_data = p_handle_si1145->raw_data[5];
		
		p_handle_si1145->aux_data = (uint16_t) ((double) p_handle_si1145->aux_data * p_handle_si1145->gain);
		
		p_handle_si1145->uv_index = ((float) p_handle_si1145->aux_data) / 100;
		p_handle_si1145->uv_level = SI1145_UV_Level_Calculation(p_handle_si1145->uv_index);
	}
}

uint8_t SI1145_UV_Level_Calculation(float uv_index)
{
	uint8_t uv_level;

			
	if(uv_index >= UV_DATA_EXTREME)
		uv_level = UV_LEVEL_EXTREME;
	else if(uv_index >= UV_DATA_VERY_HIGH)
		uv_level = UV_LEVEL_VERY_HIGH;
	else if(uv_index >= UV_DATA_HIGH)
		uv_level = UV_LEVEL_HIGH;
	else if(uv_index >= UV_DATA_MODERATE)
		uv_level = UV_LEVEL_MODERATE;
	else if(uv_index >= UV_DATA_LOW)
		uv_level = UV_LEVEL_LOW;
						
	return uv_level;
}

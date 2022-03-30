
#include "string.h"
#include "user/cmd_parsing.h"
#include "user/led.h"
#include "user/si1145.h"
#include "user/veml6070.h"

#define CDC_RX_BUFFER_SIZE 50
CMD_Status_TypeDef cmd_status;
extern HAL_StatusTypeDef flash_status;

static uint8_t cdc_rx_buffer[CDC_RX_BUFFER_SIZE];
static uint32_t cdc_rx_buffer_ptr = 0;

extern SI1145_TypeDef handle_si1145;
extern VEML6070_TypeDef handle_veml6070;
extern LED_Config_TypeDef user_led_config;


CMD_Table_TypeDef user_cmd_table[]  = {
	{""},
	{"help"},
	{"gain adj si1145"},
	{"gain adj veml6070"},
	{"reset"}
};


MSG_Table_TypeDef user_msg_table[]  = {
	{""},
	{"Unknown Command\r\n"},
	{"Syntax Error\r\n"},
	{"OK\r\n"},
	{"Input: "},
	{"Out Of Range\r\n"}
};

const char user_msg_instruction[]={"\r\n\r\n[Instruction]\r\n\
help: Show the instruction.\r\n\
gain adj si1145: Adjust the gain of si1145.\r\n\
gain adj veml6070: Adjust the gain of veml6070.\r\n\
reset: System reset.\r\n\
"
};


void CDC_Receive(uint8_t *input_ptr,uint32_t length)
{

	char *crlf_ptr_cr,*crlf_ptr_lf;
	user_led_config.indication_mode = LED_MOODE_NULL;
	
	if(cdc_rx_buffer_ptr + length > sizeof(cdc_rx_buffer))
	{
		cdc_rx_buffer_ptr = 0;
//		memset(cdc_rx_buffer,'\0',sizeof(cdc_rx_buffer));
	}
	
	if(length <= sizeof(cdc_rx_buffer))
	{
		if(input_ptr[0] == '\b')
		{
			if(cdc_rx_buffer_ptr > 0)
			{
				cdc_rx_buffer_ptr--;
				cdc_rx_buffer[cdc_rx_buffer_ptr] = 0;
			}
		}
		else
		{
			cdc_rx_buffer[cdc_rx_buffer_ptr] = 0;
			strcat((char*)cdc_rx_buffer,(char*)input_ptr);
			cdc_rx_buffer_ptr += length;
			
			if(cdc_rx_buffer_ptr < sizeof(cdc_rx_buffer))
			{
			cdc_rx_buffer[cdc_rx_buffer_ptr] = 0;
			}
			else
			{
				cdc_rx_buffer_ptr = 0;
				cdc_rx_buffer[cdc_rx_buffer_ptr] = 0;
			}
			
		}
	}

	crlf_ptr_cr = strstr((const char*)cdc_rx_buffer,"\r");
	crlf_ptr_lf = strstr((const char*)cdc_rx_buffer,"\n");

	
	if(crlf_ptr_cr != 0)
	{
		*(crlf_ptr_cr) = 0;
		cdc_rx_buffer_ptr = 0;
		
		cmd_status.crlf_flag = CMD_CRLF_SET;
//		CMD_Parasing();
	}
	else if(crlf_ptr_lf != 0)
	{
		*(crlf_ptr_lf) = 0;
		cdc_rx_buffer_ptr = 0;
		
		cmd_status.crlf_flag = CMD_CRLF_SET;
//		CMD_Parasing();
	}

}

void CMD_Parasing(void)
{
	uint8_t k;
	double gain;
	
	switch(cmd_status.cmd_ptr)
	{
		case CMD_TABLE_NULL:
			cmd_status.cmp_result = 0;
			for(k=1;k < sizeof(user_cmd_table)/sizeof(user_cmd_table[0]);k++)
			{
				cmd_status.cmp_result = strcmp((const char*)cdc_rx_buffer,(const char*)&user_cmd_table[k].table_string);
				if(cmd_status.cmp_result == 0)
				{
					cmd_status.cmd_ptr = k;
					break;
				}
			}
			
			
			if(cmd_status.cmp_result != 0)
			{
				if(strlen((const char*)cdc_rx_buffer) > 0)
				{
					MSG_Send(MSG_TABLE_UKN_CMD);
				}
			}
			else
			{
				switch(cmd_status.cmd_ptr)
				{	
						case CMD_TABLE_GAIN_ADJ_SI1145:
						MSG_Send(MSG_TABLE_INPUT);
						break;		
					case CMD_TABLE_GAIN_ADJ_VEML6070:
						MSG_Send(MSG_TABLE_INPUT);
						break;		
					default:
						break;
				}	
			}
			
			break;
		case CMD_TABLE_GAIN_ADJ_SI1145:
			if(sscanf((const char*)cdc_rx_buffer,"%lf%*s",&gain) != 0)
			{
				if(gain > 0 && gain <= 1e3)
				{
					handle_si1145.gain = gain;
					handle_si1145.gain_uint32 = (uint32_t)(gain * 1e3);
					
					flash_status |= Flash_User_Config_Write();
					if(flash_status == HAL_OK)
					{
						MSG_Send(MSG_TABLE_OK);
						cmd_status.cmd_ptr = CMD_TABLE_NULL;
					}
				}
				else	
				{
					MSG_Send(MSG_TABLE_OUT_OF_RANGE);
				}
			}
			else
			{
				MSG_Send(MSG_TABLE_SYN_ERR);
			}
			
			break;
		case CMD_TABLE_GAIN_ADJ_VEML6070:
			if(sscanf((const char*)cdc_rx_buffer,"%lf%*s",&gain) != 0)
			{
				if(gain > 0 && gain <= 1e3)
				{
					handle_veml6070.gain = gain;
					handle_veml6070.gain_uint32 = (uint32_t)(gain * 1e3);
				
					flash_status |= Flash_User_Config_Write();
					if(flash_status == HAL_OK)
					{
						MSG_Send(MSG_TABLE_OK);
						cmd_status.cmd_ptr = CMD_TABLE_NULL;
					}
				}
				else
				{
					MSG_Send(MSG_TABLE_OUT_OF_RANGE);
				}
			}
			else
			{
					MSG_Send(MSG_TABLE_SYN_ERR);
			}
			
			break;

		default:
			break;
	}
	
	switch(cmd_status.cmd_ptr)
	{
		case CMD_TABLE_HELP:
			
			MSG_Send_Instruction();
			cmd_status.cmd_ptr = CMD_TABLE_NULL;
			break;
		case CMD_TABLE_SYSTEM_RESET:
			
			HAL_NVIC_SystemReset();
		
			break;
		default:
			break;
	}
}


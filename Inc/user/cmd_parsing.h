

#include "main.h"
//#include "stm32f0xx_hal.h"

//uint8_t Error_Message[]="Error!!\r\n";
//uint8_t Data_Begin_Message[]="\r\n@DATA_BEGIN\r\n",Data_End_Message[]="@DATA_END\r\n";
//uint8_t Help_Message[]="\r\nCommand List:\r\n@UVI: Current UV index\r\n@UV_DOSE: UV dose\r\n@UVI_REC: UV index record\r\n@BATTERY: Battery status\r\n@HELP: Command list\r\n@BT_NAME: BT name configuration\r\n\r\n";
//uint8_t CMD_On[]="$$$",CMD_Off[]="---\r\n",CMD_Reboot[]="r,1\r\n",OK_Message[]="OK!!\r\n",CRLF[]="\r\n";

#define CMD_TABLE_STRING_LENGTH 20
#define MSG_TABLE_STRING_LENGTH 30

#define CMD_CRLF_RESET 0
#define CMD_CRLF_SET 1

#define CMD_TABLE_NULL 0
#define CMD_TABLE_HELP 1
#define CMD_TABLE_GAIN_ADJ_SI1145 2
#define CMD_TABLE_GAIN_ADJ_VEML6070 3
#define CMD_TABLE_SYSTEM_RESET 4

#define MSG_TABLE_UKN_CMD 1
#define MSG_TABLE_SYN_ERR 2
#define MSG_TABLE_OK 3
#define MSG_TABLE_INPUT 4
#define MSG_TABLE_OUT_OF_RANGE 5

typedef struct __CMD_Table_TypeDef{
const char table_string[CMD_TABLE_STRING_LENGTH];
}CMD_Table_TypeDef;




typedef struct __MSG_Table_TypeDef{
const char table_string[MSG_TABLE_STRING_LENGTH];
}MSG_Table_TypeDef;

typedef struct __CMD_Status_TypeDef
{
	
	uint8_t crlf_flag;
	uint8_t cmd_ptr;
	int16_t cmp_result;
	
}CMD_Status_TypeDef;


void CDC_Receive(uint8_t *input_ptr,uint32_t length);
void CMD_Parasing(void);






#ifndef __MY_IIC_H_
#define __MY_IIC_H_

#include "main.h"
#include "usart.h"
#include "string.h"
#include "cmsis_os.h"


#define MODULE_BUFF uart5_buff

//服务器IP
//#define MODULE_IP_ADDR1 	120
//#define MODULE_IP_ADDR2 	76
//#define MODULE_IP_ADDR3 	100
//#define MODULE_IP_ADDR4 	197
//#define MODULE_IP_PORT 	10002

//徐凯ip
#define MODULE_IP_ADDR1 194
#define MODULE_IP_ADDR2 163
#define MODULE_IP_ADDR3 175
#define MODULE_IP_ADDR4 136
#define MODULE_IP_PORT 	35616

#define MODULE4G_ENABLE    	HAL_GPIO_WritePin(EC600_EN_GPIO_Port, EC600_EN_Pin, GPIO_PIN_RESET)
#define MODULE4G_DISABLE    HAL_GPIO_WritePin(EC600_EN_GPIO_Port, EC600_EN_Pin, GPIO_PIN_SET)

typedef enum
{
ATCMD_START       =0X00,
ATCMD_SEND        =0X01,
ATCMD_WAIT_REV    =0X02,
ATCMD_REVOK       =0X03,
ATCMD_TIMEOUT     =0X04
}ATCMD_StatusTypeDef;

typedef enum
{
	ATERROR           =0X0F,        //进入该状态说明SIM卡有问题，可以考虑重启
	ATRESET           =0X1F,         //重启
	ATSend_START      =0X2F,
	ATSend_ATE0       =0X00,
	ATSend_CPIN       =0X01,
	ATSend_CREG       =0X02,
	ATSend_CGREG      =0X03,
	ATSend_QICSGP     =0X04,
	ATSend_QIACT1     =0X05,
	ATSend_QIDEACT    =0X06,
	ATSend_QIACT2     =0X07,
	ATSend_QIOPEN     =0X08,
	ATSend_OK         =0X09        //初始化完成

}ATSend_StatusTypeDef;


extern osThreadId Module4G_TaskHandle;

void Inc_AT_Tick(void);
ATCMD_StatusTypeDef AT_CMD_SEND(const char* cmd,uint16_t timeout,uint8_t count);
ATSend_StatusTypeDef ATSend(void);
void Module4G_Init(void);

#endif



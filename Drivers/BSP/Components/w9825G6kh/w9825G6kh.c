#include "w9825G6kh.h"
#include "main.h"

extern SDRAM_HandleTypeDef hsdram1;

void SDRAM_init(void)
{
	
	uint32_t temp=0;
	//给SDRAM提供时钟
	FMC_SDRAM_CommandTypeDef Command = {
		.CommandMode = FMC_SDRAM_CMD_CLK_ENABLE,
		.CommandTarget = FMC_SDRAM_CMD_TARGET_BANK1,
		.AutoRefreshNumber = 1,
		.ModeRegisterDefinition = 0,
	};
	HAL_SDRAM_SendCommand(&hsdram1, &Command, 0xffff);
	
	//延时至少200us
	HAL_Delay(1);
	
	//对所有Bank进行预充电
	Command.CommandMode = FMC_SDRAM_CMD_PALL;
	HAL_SDRAM_SendCommand(&hsdram1, &Command, 0xffff);
	
	//插入8个自动刷新周期
	Command.CommandMode = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
	Command.AutoRefreshNumber = 8;
	HAL_SDRAM_SendCommand(&hsdram1, &Command, 0xffff);
	
	//编程SDRAM的加载模式寄存器
	Command.CommandMode = FMC_SDRAM_CMD_LOAD_MODE;
	Command.AutoRefreshNumber = 1;
	temp=(uint32_t)SDRAM_MODEREG_BURST_LENGTH_8       |	//设置突发长度:1(可以是1/2/4/8)
              SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |	//设置突发类型:连续(可以是连续/交错)
              SDRAM_MODEREG_CAS_LATENCY_2           |	//设置CAS值:2(可以是2/3)
              SDRAM_MODEREG_OPERATING_MODE_STANDARD |   //设置操作模式:0,标准模式
              SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;     //设置突发写模式:1,单点访问
	Command.ModeRegisterDefinition = temp;
	HAL_SDRAM_SendCommand(&hsdram1, &Command, 0xffff); 
	
	//配置FMC的SDRAM控制器的自动刷新周期
	HAL_SDRAM_SetAutoRefreshNumber(&hsdram1, 677);   //761   //677
}

	







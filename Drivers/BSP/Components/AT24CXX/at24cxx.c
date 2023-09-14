#include "at24cxx.h"
#include "main.h"
#include "at24cxx_iic.h"

EEPROM_DATA eerom_data;//EEPROM读写buff
EEPROM_BSMU bsmuSetting;

//延迟一个写周期至少5ms
void AT24Cxx_DelayWriteCycleTime()
{
	HAL_Delay(10);
}
//初始化AT24C02
void AT24Cxx_Init()
{
	AT24CXX_IIC_Init();
}
/*******************************************************************************
  * 函数名：x24Cxx_ReadOneByte
  * 功  能：读一个字节
  * 参  数：u16Addr要读取的地址0-0xff
  * 返回值：u8Data读出的数据
  * 说  明：无
*******************************************************************************/
uint8_t AT24Cxx_ReadOneByte(uint16_t u16Addr)
{
	uint8_t u8Data = 0;
	AT24CXX_IIC_Start();//起始信号	
	AT24CXX_IIC_Send_Byte(AT24C02_WRITE_ADDRESS);//器件寻址+写
	AT24CXX_IIC_Wait_Ack();//等待应答
	AT24CXX_IIC_Send_Byte((uint8_t)(u16Addr & 0xFF));//只取地址的低字节
	AT24CXX_IIC_Wait_Ack();//等待应答
	AT24CXX_IIC_Start();//起始信号
	AT24CXX_IIC_Send_Byte(AT24C02_READ_ADDRESS);//器件寻址+读
	AT24CXX_IIC_Wait_Ack();//等待应答
	u8Data = AT24CXX_IIC_Read_Byte(0);
	AT24CXX_IIC_Stop();
	return u8Data;
}


/*******************************************************************************
  * 函数名：x24Cxx_WriteByte
  * 功  能：写一个字节
  * 参  数：u16Addr要写入的地址0-0xff
			u8Data要写入的数据
  * 返回值：无
  * 说  明：无
	* 注意：一个写周期5ms
*******************************************************************************/
void AT24Cxx_WriteOneByte(uint16_t u16Addr, uint8_t u8Data)
{
	AT24CXX_IIC_Start();//起始信号
	AT24CXX_IIC_Send_Byte(AT24C02_WRITE_ADDRESS);//器件寻址+写
	AT24CXX_IIC_Wait_Ack();//等待应答
	AT24CXX_IIC_Send_Byte((uint8_t)(u16Addr & 0xFF));//只取地址的低字节
	AT24CXX_IIC_Wait_Ack();//等待应答
	AT24CXX_IIC_Send_Byte(u8Data);
	AT24CXX_IIC_Wait_Ack();//等待应答
	AT24CXX_IIC_Stop();
	AT24Cxx_DelayWriteCycleTime();
}


/*******************************************************************************
  * 函数名：AT24Cxx_SeqRead
  * 功  能：顺序读取
  * 参  数：u16Addr要读取的地址0-0xff
	* 参  数：u16len从地址0要读取的长度
  * 返回值：u8*Buff读出的数据保存地址
	* 说  明：u16Addr+u16len <= 最大为CAPACITY_SIZE
*******************************************************************************/
void AT24Cxx_SeqRead(uint16_t u16Addr, uint16_t len, uint8_t* Buff)
{
	//写入读取地址
	AT24CXX_IIC_Start();//起始信号	
	AT24CXX_IIC_Send_Byte(AT24C02_WRITE_ADDRESS);//器件寻址+写
	AT24CXX_IIC_Wait_Ack();//等待应答
	AT24CXX_IIC_Send_Byte((uint8_t)(u16Addr & 0xFF));//只取地址的低字节
	AT24CXX_IIC_Wait_Ack();//等待应答
	
	AT24CXX_IIC_Start();//起始信号 
	AT24CXX_IIC_Send_Byte(AT24C02_READ_ADDRESS);//器件寻址+读
	AT24CXX_IIC_Wait_Ack();//等待应答
	for(int i=0; i<len; i++)
	{	   
    	Buff[i] = AT24CXX_IIC_Read_Byte(i==(len-1)?0:1); //读数据	  
	} 
    AT24CXX_IIC_Stop();//产生一个停止条件 
}
	
/*******************************************************************************
  * 函数名：x24Cxx_SeqWrit
  * 功  能：顺序写入
  * 参  数：u16Addr要写入的起始地址0-0xff(要考虑跨页问题，目前支持从0开始)
	* 参  数：u16len从地址0要读取的长度
  * 返回值：u8*Buff写入的数据的指针
	* 说  明：u16Addr+u16len <= 最大为CAPACITY_SIZE
*******************************************************************************/
void AT24Cxx_SeqWrite(uint16_t u16Addr, uint16_t u16Len, uint8_t *pData)
{
	uint16_t Len = u16Len;
	uint16_t zLen = 0;
	uint16_t yLen = 0;
	uint16_t index = 0;
	
	if (u16Addr + u16Len > CAPACITY_SIZE)//长度大于最大容量
	{
		Len = CAPACITY_SIZE - u16Addr;
	}
	zLen = Len/PAGE_SIZE;
	yLen = Len%PAGE_SIZE;
	
	for(int j = 0; j<zLen; j++){
		AT24CXX_IIC_Start();//起始信号
		AT24CXX_IIC_Send_Byte(AT24C02_WRITE_ADDRESS);//器件寻址+写
		AT24CXX_IIC_Wait_Ack();//等待应答
		AT24CXX_IIC_Send_Byte((uint8_t)(index & 0xFF));//只取地址的低字节
		AT24CXX_IIC_Wait_Ack();//等待应答
		for (int i = 0; i < PAGE_SIZE; i++)
		{
			AT24CXX_IIC_Send_Byte(*(pData + index));
			AT24CXX_IIC_Wait_Ack();//等待应答
			index++;
		}
		AT24CXX_IIC_Stop();
		AT24Cxx_DelayWriteCycleTime();
	}
	
	AT24CXX_IIC_Start();//起始信号
	AT24CXX_IIC_Send_Byte(AT24C02_WRITE_ADDRESS);//器件寻址+写
	AT24CXX_IIC_Wait_Ack();//等待应答
	AT24CXX_IIC_Send_Byte((uint8_t)(index & 0xFF));//只取地址的低字节
	AT24CXX_IIC_Wait_Ack();//等待应答
	for (int i = 0; i < yLen; i++)
	{
		AT24CXX_IIC_Send_Byte(*(pData + i));
		AT24CXX_IIC_Wait_Ack();//等待应答
	}
	AT24CXX_IIC_Stop();
	AT24Cxx_DelayWriteCycleTime();	
}




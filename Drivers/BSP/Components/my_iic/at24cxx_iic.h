#ifndef __AT24CXX_IIC_H_
#define __AT24CXX_IIC_H_

#include "main.h"
#include "stdint.h"
#include "my_delay.h"
#include "gpio.h"

//定义IIC引脚
#define AT24CXX_I2C_SCL_Pin 				AT24CXX_SCL_Pin
#define AT24CXX_I2C_SCL_GPIO_Port 	AT24CXX_SCL_GPIO_Port
#define AT24CXX_I2C_SDA_Pin 				AT24CXX_SDA_Pin
#define AT24CXX_I2C_SDA_GPIO_Port 	AT24CXX_SDA_GPIO_Port


//SDA方向设置
#define AT24CXX_I2C_SDA_IN()  {AT24CXX_SDA_GPIO_Port->MODER&=~(3<<(13*2));AT24CXX_SDA_GPIO_Port->MODER|=0<<13*2;}	//PD13输入模式
#define AT24CXX_I2C_SDA_OUT() {AT24CXX_SDA_GPIO_Port->MODER&=~(3<<(13*2));AT24CXX_SDA_GPIO_Port->MODER|=1<<13*2;} //PD13输出模式


//IO操作
#define AT24CXX_IIC_SCL(n)  (n?HAL_GPIO_WritePin(AT24CXX_SCL_GPIO_Port,AT24CXX_SCL_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(AT24CXX_SCL_GPIO_Port,AT24CXX_SCL_Pin,GPIO_PIN_RESET)) //SCL
#define AT24CXX_IIC_SDA(n)  (n?HAL_GPIO_WritePin(AT24CXX_SDA_GPIO_Port,AT24CXX_SDA_Pin,GPIO_PIN_SET):HAL_GPIO_WritePin(AT24CXX_SDA_GPIO_Port,AT24CXX_SDA_Pin,GPIO_PIN_RESET)) //SDA
#define AT24CXX_I2C_READ_SDA    HAL_GPIO_ReadPin(AT24CXX_SDA_GPIO_Port,AT24CXX_SDA_Pin)  //输入SDA

//IIC所有操作函数
void AT24CXX_IIC_Init(void);                //初始化IIC的IO口				 
void AT24CXX_IIC_Start(void);				//发送IIC开始信号
void AT24CXX_IIC_Stop(void);	  			//发送IIC停止信号
void AT24CXX_IIC_Send_Byte(uint8_t  txd);			//IIC发送一个字节
uint8_t AT24CXX_IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
uint8_t AT24CXX_IIC_Wait_Ack(void); 				//IIC等待ACK信号
void AT24CXX_IIC_Ack(void);					//IIC发送ACK信号
void AT24CXX_IIC_NAck(void);				//IIC不发送ACK信号

void AT24CXX_IIC_Write_One_Byte(uint8_t  daddr,uint8_t  addr,uint8_t  data);
uint8_t AT24CXX_IIC_Read_One_Byte(uint8_t  daddr,uint8_t  addr);	 


#endif


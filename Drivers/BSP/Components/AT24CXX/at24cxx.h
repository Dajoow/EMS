/*
 * @Author: yhb 952872276@qq.com
 * @Date: 2023-03-30 21:42:05
 * @LastEditors: yhb 952872276@qq.com
 * @LastEditTime: 2023-05-30 21:33:22
 * @FilePath: \MDK-ARMd:\documents\STM32_prj\BSMU_H750IBT6\BSMU_H750IB\Drivers\BSP\Components\AT24CXX\AT24CXX.h
 * @Description: 
 * 
 * Copyright (c) 2023 by ${git_name_email}, All Rights Reserved. 
 */
#ifndef __AT24CXX_H_
#define __AT24CXX_H_

#include "stdint.h"
/* 
 * AT24C02 2kb = 2048bit = 2048/8 B = 256 B
 * 32 pages of 8 bytes each
 *
 * Device Address
 * 1 0 1 0 A2 A1 A0 R/W
 * 1 0 1 0 0  0  0  0 = 0XA0
 * 1 0 1 0 0  0  0  1 = 0XA1 
 */
//写地址
#define     AT24C02_WRITE_ADDRESS       0xA0
//读地址
#define     AT24C02_READ_ADDRESS        0xA1

#define x24C02//器件名称，x24C01或x24C02

#ifdef x24C01
	#define PAGE_NUM			16						//页数
	#define PAGE_SIZE			8						//页面大小(字节)
	#define CAPACITY_SIZE		(PAGE_NUM * PAGE_SIZE)	//总容量(字节)
	#define ADDR_BYTE_NUM		1						//地址字节个数
#endif


#ifdef x24C02
	#define PAGE_NUM			32						//页数 
	#define PAGE_SIZE			8						//页面大小(字节)
	#define CAPACITY_SIZE		(PAGE_NUM * PAGE_SIZE)	//总容量(字节)
	#define ADDR_BYTE_NUM		1						//地址字节个数
#endif


//存储结构体(我们用来测试的)
typedef struct EEPROM_ST
{
	uint8_t WriteBuff[CAPACITY_SIZE];
	uint8_t ReadBuff[CAPACITY_SIZE];
}EEPROM_DATA;

//存储结构体(我们用来测试的)
typedef struct EEPROM_BSMU_t
{
	 uint16_t cu_num;
	uint16_t poll_T;      
    uint16_t IP_ADD_1[4];	 //本地服务器
	uint16_t port; //本地服务器port
    uint16_t IP_ADD_2[4];	 //4G
	uint16_t port_1;//4g模块port
    uint16_t canBps;			//CAN波特率
    uint16_t RS485Bps;			//485波特率
    uint16_t local_flag;     //开、关本地网络
    uint16_t yunduan_flag;   //开、关云端服务器
}EEPROM_BSMU;

extern EEPROM_DATA eerom_data;
extern EEPROM_BSMU bsmuSetting;

//void Default_Setting(void);
////从AT24C02读取数据
//int AT24C02_Read(uint16_t ReadAddr, uint8_t *pBuffer, uint16_t NumToRead);
////写数据到AT24C02
//int AT24C02_Write(uint16_t WriteAddr, uint8_t *pBuffer, uint16_t NumToWrite);

void AT24Cxx_Init(void);
uint8_t AT24Cxx_ReadOneByte(uint16_t u16Addr);
void AT24Cxx_WriteOneByte(uint16_t u16Addr, uint8_t u8Data);
void AT24Cxx_SeqRead(uint16_t u16Addr, uint16_t len, uint8_t* Buff);
void AT24Cxx_SeqWrite(uint16_t u16Addr, uint16_t u16Len, uint8_t *pData);
#endif



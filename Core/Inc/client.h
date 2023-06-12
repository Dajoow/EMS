/**
  ******************************************************************************
  * File Name          : client.h
  * Description        : serve as client
  ******************************************************************************
  * @attention
  *
  *
  * @author:yhb
  *
  *
  ******************************************************************************
  */

#ifndef CLIENT_H
#define CLIENT_H

#include "stdint.h"

//yhb电脑
#define SERVER_PORT			 3333                      //服务器端口
#define SERVER_IP			   "10.2.133.77"						 //服务器IP

//车
// #define SERVER_PORT			 9000                      //服务器端口
// //#define SERVER_IP			   "10.2.133.80"						 //服务器IP
// #define SERVER_IP			   "169.254.98.14"						 //服务器IP

//心跳包
#define keepAlive        0

#define RECV_DATA_MAX 	 1024


void ClientInit(void);
void LAN8720_RESET(void);

#endif





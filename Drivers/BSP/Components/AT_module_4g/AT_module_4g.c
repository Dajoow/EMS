#include "AT_module_4g.h"
#include "stdio.h"
#include "cmsis_os.h"

uint16_t Wait_Tick = 0;
uint16_t REV_Tick = 0;

osThreadId Module4G_TaskHandle;
extern osMutexId DebugUartMutexHandle;
/*
计时，放在systick中断中
*/
void Inc_AT_Tick(){
   Wait_Tick++;
   REV_Tick++;
}

void ResetModuleUsartREV(){
    ResetUart5REV();
}

/*
cmd：AT指令，末尾要加\r\n
timeout:超时时间,Wait_Tick的值在SysTick_Handler中每次+1，
            Wait_Tick>timeout时AT指令变为ATCMD_TIMEOUT状态
count：AT指令超时重新发送次数
*/
ATCMD_StatusTypeDef AT_CMD_SEND(const char* cmd,uint16_t timeout,uint8_t count)
{
	static ATCMD_StatusTypeDef atcmd_status=ATCMD_START;
	static uint8_t cnt=1;	
	
	switch(atcmd_status){
	    case ATCMD_START:
            cnt=count;
            atcmd_status=ATCMD_SEND;
            break;
	    case ATCMD_SEND:
            Module4G_printf("%s", cmd);
			Debug_printf("->%s", cmd);
            atcmd_status=ATCMD_WAIT_REV;
            Wait_Tick=0;
            cnt--;
            break;
	    case ATCMD_WAIT_REV:
            if(Wait_Tick<timeout){
				if(MODULE_BUFF.recv_end_flag ==1){
					if(strstr((char*)MODULE_BUFF.recv_buf,"\r\n")!=NULL)
					{
						atcmd_status = ATCMD_REVOK;	
					}
					Debug_printf("-----------------\r\n");
                    Debug_printf("%s\r\n", MODULE_BUFF.recv_buf);
					Debug_printf("-----------------\r\n\r\n");
                    //这里不进行接收的rst
                }
			}
			else{
				if(cnt>0)
					atcmd_status=ATCMD_SEND;
				else
					atcmd_status=ATCMD_TIMEOUT;										
			}
			break;
	    case ATCMD_REVOK:
			atcmd_status=ATCMD_START;
			break;
	    case ATCMD_TIMEOUT:
			atcmd_status=ATCMD_START;
			break;
	    default:
			break;
	
	}

	return atcmd_status;	
}



ATSend_StatusTypeDef ATSend(void)
{
	static ATSend_StatusTypeDef atsend_status=ATRESET;
 	static ATCMD_StatusTypeDef res;
	char str[128];
	switch (atsend_status){
		case ATRESET:
				/*
			这里可以添加控制重启程序
			*/	
				MODULE4G_DISABLE;
				vTaskDelay(3000);
				MODULE4G_ENABLE;
				REV_Tick=0;
				atsend_status=ATSend_START;
				ResetModuleUsartREV();
			break;
		case ATSend_START:
				if(REV_Tick<30000){
					if(MODULE_BUFF.recv_end_flag ==1)
					{

						if(strstr((char*)MODULE_BUFF.recv_buf,"RDY")!=NULL)
                        {
							atsend_status=ATSend_ATE0;	
                            		 		
                        }
							ResetModuleUsartREV();						
					}
				}
				else 
					atsend_status=ATRESET;
				break;
		case ATSend_ATE0:
			vTaskDelay(1000);
			res=AT_CMD_SEND("ATE0\r\n",5000,3);
			if(res==ATCMD_REVOK)
			{
				if(strstr((char*)MODULE_BUFF.recv_buf,"OK")!=NULL)
				{
                   atsend_status=ATSend_CPIN;
                   ResetModuleUsartREV();
				}			
				else if(strstr((char*)MODULE_BUFF.recv_buf,"ERROR")!=NULL)
				{		 
                   atsend_status=ATSend_ATE0;
                   ResetModuleUsartREV();				
				}
			}
			else if(res==ATCMD_TIMEOUT)
				atsend_status=ATRESET;
		  break;
		case ATSend_CPIN:
			vTaskDelay(1000);
			res=AT_CMD_SEND("AT+CPIN?\r\n",5000,0);
			if(res==ATCMD_REVOK)
			{
			  	if(strstr((char*)MODULE_BUFF.recv_buf,"OK")!=NULL){
					atsend_status=ATSend_CREG;
					ResetModuleUsartREV();			
				}		
				else if(strstr((char*)MODULE_BUFF.recv_buf,"ERROR")!=NULL){
					atsend_status=ATSend_CPIN;
					ResetModuleUsartREV();	
				}
       		    else 
                    atsend_status=ATERROR;				
			}
			else if(res==ATCMD_TIMEOUT)
				atsend_status=ATRESET;
		  break;		
		case ATSend_CREG:
			vTaskDelay(1000);
			res=AT_CMD_SEND("AT+CREG?\r\n",5000,0);
			if(res==ATCMD_REVOK)
			{
			    if(strstr((char*)MODULE_BUFF.recv_buf,"0,1")!=NULL)
				{
					atsend_status=ATSend_CGREG;
					ResetModuleUsartREV();			
				}		
				else if(strstr((char*)MODULE_BUFF.recv_buf,"0,2")!=NULL)
				{
					atsend_status=ATERROR;
					ResetModuleUsartREV();		
				}
       		    else if(strstr((char*)MODULE_BUFF.recv_buf,"0,0")!=NULL)	
				{
					atsend_status=ATSend_CREG;
					ResetModuleUsartREV();	
				}				
			}
			else if(res==ATCMD_TIMEOUT)
				atsend_status=ATRESET;
			break;
		case ATSend_CGREG:
			vTaskDelay(1000);
			res=AT_CMD_SEND("AT+CGREG?\r\n",5000,0);
			if(res==ATCMD_REVOK)
			{
				if(strstr((char*)MODULE_BUFF.recv_buf,"OK")!=NULL)
				{
			   		atsend_status=ATSend_QICSGP;
					ResetModuleUsartREV(); 
				}
				else if(strstr((char*)MODULE_BUFF.recv_buf,"ERROR")!=NULL)
				{
			   		atsend_status=ATSend_CGREG;
					ResetModuleUsartREV();	
				}		
			}	
			else if(res==ATCMD_TIMEOUT)
				atsend_status=ATRESET;
			break;
		case ATSend_QICSGP:
			vTaskDelay(1000);
			res=AT_CMD_SEND("AT+QICSGP=1,1,\"CMIOT\",\"\",\"\",1\r\n",5000,0);
			if(res==ATCMD_REVOK)
			{
				if(strstr((char*)MODULE_BUFF.recv_buf,"OK")!=NULL)
				{
					atsend_status=ATSend_QIACT1;
					ResetModuleUsartREV();			
				}		
				else if(strstr((char*)MODULE_BUFF.recv_buf,"ERROR")!=NULL)
				{
			  		atsend_status=ATSend_QICSGP;
					ResetModuleUsartREV();	
				}
			}
			else if(res==ATCMD_TIMEOUT)
				atsend_status=ATRESET;
		  break;
		case ATSend_QIACT1:
			vTaskDelay(1000);
			res=AT_CMD_SEND("AT+QIACT=1\r\n",5000,0);
			if(res==ATCMD_REVOK)
			{
				if(strstr((char*)MODULE_BUFF.recv_buf,"OK")!=NULL)
				{
					atsend_status=ATSend_QIACT2;
					ResetModuleUsartREV();
				}
				else if(strstr((char*)MODULE_BUFF.recv_buf,"ERROR")!=NULL)
				{
					atsend_status=ATSend_QIDEACT;
					ResetModuleUsartREV();
				}	
			}
			else if(res==ATCMD_TIMEOUT)
				atsend_status=ATRESET;
			break;
		case ATSend_QIDEACT:
			vTaskDelay(1000);
			res=AT_CMD_SEND("AT+QIDEACT=1\r\n",5000,3);
			if(res==ATCMD_REVOK)
			{
				if(strstr((char*)MODULE_BUFF.recv_buf,"OK")!=NULL)
				{
					atsend_status=ATSend_CPIN;
					ResetModuleUsartREV();
				}
				else if(strstr((char*)MODULE_BUFF.recv_buf,"ERROR")!=NULL)
				{
					atsend_status=ATSend_QIDEACT;
					ResetModuleUsartREV();
				}	
			}
			else if(res==ATCMD_TIMEOUT)
				atsend_status=ATRESET;
	  case ATSend_QIACT2:
	  		vTaskDelay(1000);
			res=AT_CMD_SEND("AT+QIACT?\r\n",5000,0);
			if(res==ATCMD_REVOK)
			{
				if(strstr((char*)MODULE_BUFF.recv_buf,"1,1,1")!=NULL)
				{
					/*
					此时4G模块返回的数据为“+QIACT：1，1，1，“SIM卡IP地址”\r\n”
					添加程序可以获取IP地址
					*/
				}
				atsend_status=ATSend_QIOPEN;
				ResetModuleUsartREV();	
			}
			else if(res==ATCMD_TIMEOUT)
				atsend_status=ATRESET;
			break;
		case ATSend_QIOPEN:
			vTaskDelay(1000);
			memset(str, 0x00 , 128);
			sprintf(str, "AT+QIOPEN=1,0,\"TCP\",\"%d.%d.%d.%d\",%d,0,2\r\n", MODULE_IP_ADDR1, MODULE_IP_ADDR2, MODULE_IP_ADDR3,MODULE_IP_ADDR4, MODULE_IP_PORT);
			res=AT_CMD_SEND(str,5000,0);
			if(res==ATCMD_REVOK)
			{
				if(strstr((char*)MODULE_BUFF.recv_buf,"CONNECT")!=NULL)
				{
					atsend_status=ATSend_OK;		
					ResetModuleUsartREV();					
				}
				else if(strstr((char*)MODULE_BUFF.recv_buf,"ERROR")!=NULL)
				{
					atsend_status=ATRESET;
					ResetModuleUsartREV();			
				}
			}
			else if(res==ATCMD_TIMEOUT)
				atsend_status=ATRESET;	
			break;
		case ATSend_OK:
			if(MODULE_BUFF.recv_end_flag ==1)
			{
				/*
				可以添加接收数据处理
				*/
				Debug_printf((char*)MODULE_BUFF.recv_buf);
				ResetModuleUsartREV();		
			}
			break;
		case ATERROR:
			/*
		  这里以添加SIM卡错误警告
		  */
			break;
 	default:
 		break;
	
 	}

	return atsend_status;
}


void Module4G_Process(void const * argument){
	// Init_printf();//初始化串口缓存（放在main中一起初始化了）
	for(;;)
	{
		if(ATSend()==ATSend_OK){	
				
		}
		// Module4G_printf("hello");
		// vTaskDelay(100);
//		if(MODULE_BUFF.recv_end_flag ==1)
//		{
//			Module4G_printf(char*)MODULE_BUFF.recv_buf);
//			Debug_printf((char*)MODULE_BUFF.recv_buf);
//			ResetModuleUsartREV();						
//		}
		osDelay(1);
	}
}

void Module4G_Init(){
	osThreadDef(Module4G_Thread, Module4G_Process, osPriorityNormal, 0, 256);
	Module4G_TaskHandle = osThreadCreate(osThread(Module4G_Thread), NULL);
}



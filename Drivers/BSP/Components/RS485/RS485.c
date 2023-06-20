#include "RS485.h"


void ResetRS85UsartREV(){
    ResetUart2REV();
}

void RS485_REV(void)
{
	if(RS485_BUFF.recv_end_flag ==1)
	{
		Debug_printf("%s",RS485_BUFF.recv_buf);
		ResetRS85UsartREV();
	}
}








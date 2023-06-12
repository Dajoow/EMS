#include "my_delay.h"
#include "stdint.h" 


//这里240需要测
static void MyDelay_1us(){
	int i = 240;	//240
	while(i>0){
		i--;
	}
}




void MyDelay_us(uint32_t us){
	int i = us;
	while(i>0){
		MyDelay_1us();
		i--;
	}
}





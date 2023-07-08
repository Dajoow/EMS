#ifndef __SNTP_CLIENT_H__
#define __SNTP_CLIENT_H__

#include <stdint.h>

void sntp_client_init(void);
void sntp_set_rtc(uint32_t timestamp);
uint32_t rtc_get_timestamp(void);
void get_system_time(uint32_t* sec, uint32_t* us);

#endif

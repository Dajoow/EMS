#ifndef __SNTP_CLIENT_H__
#define __SNTP_CLIENT_H__

#include <stdint.h>
#include <time.h>

void sntp_client_init(void);
void sntp_set_rtc(uint32_t timestamp);
uint32_t rtc_get_timestamp(void);
void get_system_time(uint32_t* sec, uint32_t* us);
time_t mbedtls_get_time(time_t *timer);

#endif

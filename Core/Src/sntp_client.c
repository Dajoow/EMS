#include "sntp_client.h"
#include "lwip/apps/sntp.h"
#include "lwipopts.h"
#include "rtc.h"
#include "time.h"
#include "usart.h"

const ip_addr_t sntp_server[SNTP_MAX_SERVERS] = {
  { .addr = IPADDR4_INIT_BYTES (203, 107, 6, 88) },    // ntp.aliyun.com
  { .addr = IPADDR4_INIT_BYTES (223, 113, 103, 191) }, // cn.ntp.org.cn
  { .addr = IPADDR4_INIT_BYTES (114, 118, 7, 163) },   // ntp.ntsc.ac.cn
  { .addr = IPADDR4_INIT_BYTES (106, 55, 184, 199) },  // ntp1.tencent.com
};

void
sntp_set_rtc (uint32_t timestamp)
{
  RTC_TimeTypeDef sTime = { 0 };
  RTC_DateTypeDef sDate = { 0 };
  struct tm *utc_time = localtime (&timestamp); // use UTC time

  sTime.Hours = utc_time->tm_hour;
  sTime.Minutes = utc_time->tm_min;
  sTime.Seconds = utc_time->tm_sec;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;

  sDate.WeekDay = utc_time->tm_wday;
  sDate.Month = (utc_time->tm_mon) + 1;
  sDate.Date = utc_time->tm_mday;
  sDate.Year
      = (utc_time->tm_year); // workaround for wolfssl's stm32_hal_time()

  if (HAL_RTC_SetTime (&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
    {
      Debug_printf ("set RTC time failed!\r\n");
    }

  if (HAL_RTC_SetDate (&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
    {
      Debug_printf ("set RTC date failed!\r\n");
    }

  Debug_printf ("RTC time updated, timestamp:%lu\r\n", timestamp);
  Debug_printf ("Date:%d %d %d %d:%d:%d %d\r\n", utc_time->tm_year + 1900,
                utc_time->tm_mon+1, utc_time->tm_mday, utc_time->tm_hour,
                utc_time->tm_min, utc_time->tm_sec, utc_time->tm_wday);
}

uint32_t
rtc_get_timestamp (void)
{
  struct tm utc_time;
  static RTC_DateTypeDef g_Date = { 0 };
  static RTC_TimeTypeDef g_Time = { 0 };

  /// 获取时间必须在获取日期前
  HAL_RTC_GetTime (&hrtc, &g_Time, RTC_FORMAT_BIN);
  HAL_RTC_GetDate (&hrtc, &g_Date, RTC_FORMAT_BIN);

  utc_time.tm_year = g_Date.Year;
  utc_time.tm_mon
      = g_Date.Month - 1;          // RTC_Month rang 1-12,but tm_mon rang 0-11
  utc_time.tm_mday = g_Date.Date;  // RTC_Date rang 1-31 and tm_mday rang 1-31
  utc_time.tm_hour = g_Time.Hours; // RTC_Hours rang 0-23 and tm_hour rang 0-23
  utc_time.tm_min
      = g_Time.Minutes; // RTC_Minutes rang 0-59 and tm_min rang 0-59
  utc_time.tm_sec = g_Time.Seconds;

  return mktime (&utc_time);
}

void
get_system_time (uint32_t *sec, uint32_t *us)
{
  *sec = rtc_get_timestamp ();
  *us = 0;
}

void
sntp_client_init (void)
{
  // use complied time as default time
  sntp_set_rtc (1690163669);

  sntp_setoperatingmode (SNTP_OPMODE_POLL);

  for (int i = 0; i < SNTP_MAX_SERVERS; i++)
    {
      sntp_setserver (i, &sntp_server[i]);
    }

  sntp_init ();
}

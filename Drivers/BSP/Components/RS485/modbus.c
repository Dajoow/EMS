#include "FreeRTOS.h"
#include "cmsis_os.h"
#include "RS485.h"
#include "agile_modbus.h"
#include "agile_modbus_slave_util.h"
#include "string.h"

osThreadId modbus_taskhandle = NULL;

uint8_t modbus_send_buf[AGILE_MODBUS_MAX_ADU_LENGTH];
uint8_t modbus_read_buf[AGILE_MODBUS_MAX_ADU_LENGTH];

void modbus_task (void const *args);

void
modbus_init ()
{
  ResetRS85UsartREV();
  osThreadDef (modbus_task, modbus_task, osPriorityNormal, 0, 256);
  modbus_taskhandle = osThreadCreate (osThread (modbus_task), NULL);
}

void
modbus_task (void const *args)
{
  agile_modbus_rtu_t mb_rtu;
  agile_modbus_t *mb_rtu_ctx = &mb_rtu._ctx;
  agile_modbus_rtu_init (&mb_rtu, modbus_send_buf,
                         sizeof (modbus_send_buf), modbus_read_buf,
                         sizeof (modbus_read_buf));
  agile_modbus_set_slave (mb_rtu_ctx, 1);

  while (1)
    {
      int read_len = 0;
      // take semaphore to wait for uart data
     ulTaskNotifyTake (pdTRUE, portMAX_DELAY);

      if (RS485_BUFF.recv_end_flag == 1)
        {
          read_len = RS485_BUFF.recv_len;
          memcpy ( mb_rtu_ctx->read_buf,&RS485_BUFF.recv_buf, read_len);
          ResetRS85UsartREV();
        }

      if (read_len < 0)
        {
          Debug_printf("Receive error, now exit.\n");
          break;
        }

      if (read_len == 0)
        continue;

      int send_len = agile_modbus_slave_handle (
          mb_rtu_ctx, read_len, 0, agile_modbus_slave_util_callback, NULL,
          NULL);

      if (send_len > 0)
        HAL_UART_Transmit (&huart2, mb_rtu_ctx->send_buf, send_len, 0xff);
    }
}

/*
 * This file is part of the Serial Flash Universal Driver Library.
 *
 * Copyright (c) 2016-2018, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: Portable interface for each platform.
 * Created on: 2016-04-23
 */

#include <sfud.h>
#include <stdarg.h>
#include "stm32h7xx.h"
#include "usart.h"
#include "cmsis_os.h"
#include "string.h"

typedef struct
{
    SPI_HandleTypeDef *spix;
    GPIO_TypeDef *cs_gpiox;
    uint16_t cs_gpio_pin;
    osSemaphoreId s_lock;
} spi_user_data, *spi_user_data_t;

static char log_buf[256];


void sfud_log_debug(const char *file, const long line, const char *format, ...);


static inline void spi_lock(const sfud_spi *spi) {
    spi_user_data_t userdata = spi->user_data;
    xSemaphoreTake (userdata->s_lock, portMAX_DELAY);
}

static inline void spi_unlock(const sfud_spi *spi) {
    spi_user_data_t userdata = spi->user_data;
    xSemaphoreGive(userdata->s_lock);
}

static void retry_delay_1ms(void) {
    osDelay (1);
}

/**
 * SPI write data then read data
 */
static sfud_err spi_write_read(const sfud_spi *spi, const uint8_t *write_buf, size_t write_size, uint8_t *read_buf,
    size_t read_size) {
    sfud_err result = SFUD_SUCCESS;
    //uint8_t send_data, read_data;

    /**
     * add your spi write and read code
     */

    spi_user_data_t spi_dev = (spi_user_data_t) spi->user_data;

    if (write_size) {
        SFUD_ASSERT(write_buf);
    }
    if (read_size) {
        SFUD_ASSERT(read_buf);
    }

    uint8_t *temp_write = pvPortMalloc (write_size + read_size);
    uint8_t *temp_read = pvPortMalloc (write_size + read_size);

    memcpy (temp_write, write_buf, write_size);

    HAL_GPIO_WritePin(spi_dev->cs_gpiox, spi_dev->cs_gpio_pin, GPIO_PIN_RESET);
    HAL_StatusTypeDef hal_status;
    hal_status = HAL_SPI_TransmitReceive (spi_dev->spix, temp_write, temp_read,
                             (uint16_t)(write_size + read_size), 0xff);
    result = (hal_status == HAL_OK)
       ? SFUD_SUCCESS
       : SFUD_ERR_TIMEOUT;

    HAL_GPIO_WritePin (spi_dev->cs_gpiox, spi_dev->cs_gpio_pin, GPIO_PIN_SET);

    memcpy (read_buf, temp_read + write_size, read_size);

    vPortFree (temp_write);
    vPortFree (temp_read);

    return result;
}

#ifdef SFUD_USING_QSPI
/**
 * read flash data by QSPI
 */
static sfud_err qspi_read(const struct __sfud_spi *spi, uint32_t addr, sfud_qspi_read_cmd_format *qspi_read_cmd_format,
        uint8_t *read_buf, size_t read_size) {
    sfud_err result = SFUD_SUCCESS;

    /**
     * add your qspi read flash data code
     */

    return result;
}
#endif /* SFUD_USING_QSPI */

extern SPI_HandleTypeDef hspi2;

spi_user_data w25q_spi2 = { &hspi2, SPI2_NSS_GPIO_Port, SPI2_NSS_Pin, NULL };

sfud_err sfud_spi_port_init (sfud_flash * flash)
{
    sfud_err result = SFUD_SUCCESS;

    /**
     * add your port spi bus and device object initialize code like this:
     * 1. rcc initialize
     * 2. gpio initialize
     * 3. spi device initialize
     * 4. flash->spi and flash->retry item initialize
     *    flash->spi.wr = spi_write_read; //Required
     *    flash->spi.qspi_read = qspi_read; //Required when QSPI mode enable
     *    flash->spi.lock = spi_lock;
     *    flash->spi.unlock = spi_unlock;
     *    flash->spi.user_data = &spix;
     *    flash->retry.delay = null;
     *    flash->retry.times = 10000; //Required
     */
    switch (flash->index) {
        case SFUD_W25Q256_DEVICE_INDEX: {
            /* 同步 Flash 移植所需的接口及数据 */
            flash->spi.wr = spi_write_read;
            flash->spi.lock = spi_lock;
            flash->spi.unlock = spi_unlock;
            flash->spi.user_data = &w25q_spi2;
            flash->retry.delay = retry_delay_1ms;
            /* adout 10 seconds timeout */
            flash->retry.times = 10 * 1000;
            
            HAL_GPIO_WritePin(w25q_spi2.cs_gpiox, w25q_spi2.cs_gpio_pin, GPIO_PIN_SET);

            break;
        }
    }

    spi_user_data_t userdata = flash->spi.user_data;
    osSemaphoreDef(sfud_lock);
    userdata->s_lock = osSemaphoreCreate(osSemaphore(sfud_lock), 1);

    return result;
}

/**
 * This function is print debug info.
 *
 * @param file the file which has call this function
 * @param line the line number which has call this function
 * @param format output format
 * @param ... args
 */
void sfud_log_debug(const char *file, const long line, const char *format, ...) {
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);
    Debug_printf("[SFUD](%s:%ld) ", file, line);
    /* must use vprintf to print */
    vsnprintf(log_buf, sizeof(log_buf), format, args);
    Debug_printf("%s\n", log_buf);
    va_end(args);
}

/**
 * This function is print routine info.
 *
 * @param format output format
 * @param ... args
 */
void sfud_log_info(const char *format, ...) {
    va_list args;

    /* args point to the first variable parameter */
    va_start(args, format);
    Debug_printf("[SFUD]");
    /* must use vprintf to print */
    vsnprintf(log_buf, sizeof(log_buf), format, args);
    Debug_printf("%s\n", log_buf);
    va_end(args);
}

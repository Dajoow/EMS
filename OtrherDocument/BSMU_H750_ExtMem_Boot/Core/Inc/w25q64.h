
#ifndef W25Q64_H
#define W25Q64_H
#ifdef __cplusplus
extern "C" {
#endif
 
#include "main.h"
#include <stdint.h>
 
 
#define W25Q256JV_FLASH_SIZE                  0x800000 /* 64 MBits => 8MBytes */
#define W25Q256JV_SECTOR_SIZE                 0x10000   /* 128 sectors of 64KBytes */
#define W25Q256JV_SUBSECTOR_SIZE              0x1000    /* 4096 subsectors of 4kBytes */
#define W25Q256JV_PAGE_SIZE                   0x100     /* 65536 pages of 256 bytes */ 
 
 
#define W25Q64JV_WRITE_ENABLE (0x06)
/*
 * The Quad Enable (QE) bit is set to 1 by default in the factory, therefore the device supports Standard/Dual
SPI as well as Quad SPI after power on. This bit cannot be reset to 1.
 */
#define W25Q64JV_INPUT_FAST_READ (0xeb)
#define W25Q64JV_PAGE_PROGRAM (0x02)
#define W25Q64JV_STATUS_REG1 (0x05)
#define W25Q64JV_ENABLE_RESET (0x66)
#define W25Q64JV_RESET_DEVICE (0x99)
#define W25Q64JV_DEVICE_ID (0x90)
#define W25Q64JV_ID_NUMBER (0x4b)
#define W25Q64JV_ERASE_SECTOR (0x20)
#define W25Q64JV_ERASE_CHIP (0xc7)
 
/* 状态寄存器标志 */
#define W25Q256JV_FSR_BUSY                    ((uint8_t)0x01)    /*!< busy */
#define W25Q256JV_FSR_WREN                    ((uint8_t)0x02)    /*!< write enable */
#define W25Q256JV_FSR_QE                      ((uint8_t)0x02)    /*!< quad enable */ 
 
#define MEMORY_FLASH_SIZE 0x800000 /* 64MBits => 8MBytes */
#define MEMORY_BLOCK_SIZE 0x10000 /* 64KBytes */
#define MEMORY_SECTOR_SIZE 0x1000 /* 4KBytes */
#define MEMORY_PAGE_SIZE 0x100 /* 32768 pages of 256Bytes */
 
#define QPSI_TIMEOUT_DEFAULT_VALUE ((uint32_t)5000)/* 5 s */
 
uint8_t CSP_QUADSPI_Init(void);
uint8_t CSP_QSPI_EraseSector(uint32_t EraseStartAddress, uint32_t EraseEndAddress);
uint8_t CSP_QSPI_WriteMemory(uint8_t *buffer, uint32_t address, uint32_t buffer_size);
uint8_t CSP_QSPI_EnableMemoryMappedMode(void);
uint8_t CSP_QSPI_Erase_Chip(void);
 
//uint8_t QSPI_WriteEnable(void);
//uint8_t QSPI_AutoPollingMemReady(void);
//uint8_t QSPI_ResetChip(void);

#ifdef __cplusplus
}
#endif
#endif



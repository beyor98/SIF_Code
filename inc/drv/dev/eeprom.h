/**
 *
 * Copyright (C) 2016 Gree Microelectronics.  All Rights Reserved.
 * 
 * @file            eeprom.h
 *
 * @author        terry.wu
 *
 * @version       1.0.0
 * 
 * @date           2016/06/29 
 *    
 * @brief           This file emulates EEPROM with the flash in chip, it uses two pages, once a page
 *                     is full, it will transfer the valid data to another page. The data stored to emulated
 *                     EEPROM composes of 16bits value and 16bits virtual address. The first four byte
 *                     of each page indicates the page's status : valid, erase, receive.
 * 
 * @note
 *    2016/06/29, terry.wu, V1.0.0
 *        Initial version.
 */

#ifndef _EEPROM_H_
#define _EEPROM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <common.h>



/**
 * @defgroup EEPROM_PAGE_INDEX 
 * @brief Specify which two flash pages need to emulate EEPROM
 * @{
 */
#define EEPROM_PAGE0      ((int16_t)(FLASH_TOTAL_PAGES-4))
#define EEPROM_PAGE1      ((int16_t)(FLASH_TOTAL_PAGES-3))	
/**
 * @}
 */
 
/**
 * Maximum number of variable supported, since the limited of RAM size and
 * flash size, this MACRO can not set too large
 */
#define VAR_MAX_NUM             ((uint8_t)20)

/** Specify the invalid virtual address */
#define INVALID_VIRTUAL_ADDR    ((uint16_t)0xFFFF)

/** Specify the start address of EEPROM */
#define EEPROM_START_ADDR       ((uint32_t)(FLASH_LOGICAL_BASE_ADDRESS + FLASH_PAGE_SIZE * EEPROM_PAGE0))

/**
 * @defgroup EEPROM_PAGE_ADDRESS
 * Specify the start address and end address of each page for EEPROM
 * @{
 */
#define EEPROM_PAGE0_START_ADDR    EEPROM_START_ADDR
#define EEPROM_PAGE0_END_ADDR      ((uint32_t)(EEPROM_PAGE0_START_ADDR + FLASH_PAGE_SIZE - 1))
#define EEPROM_PAGE1_START_ADDR    ((uint32_t)(FLASH_LOGICAL_BASE_ADDRESS + FLASH_PAGE_SIZE * EEPROM_PAGE1))
#define EEPROM_PAGE1_END_ADDR      ((uint32_t)(EEPROM_PAGE1_START_ADDR + FLASH_PAGE_SIZE - 1))
/**
 * @}
 */

/** Indicate the page is erased */
#define PAGE_ERASED        ((uint32_t)0xFFFFFFFF)
/** Indicate the page receive data from another page */
#define PAGE_RECEIVE       ((uint32_t)0x0000FFFF)
/** Indicate the page contain valid data */
#define PAGE_VALID         ((uint32_t)0x00000000)

/**
 * List the operation of EEPROM
 */
typedef enum {
    READ_FROM_VALID_PAGE = 0,       /**< Read data from valid page */
    WRITE_TO_VALID_PAGE             /**< Write data to valid page */
} EEPROM_Operation_T;

/**
 * @brief Pass all the virtual address of the variables that need to write to EEPROM.
 *
 * @param pVirtualAddr Point to the buffer that contained all the variable's virtual address.
 * @param num Indicate the variable's number. 
 *
 * @retval RET_OK Set virtual address to EEPROM driver success.
 * @retval RET_ERROR Set virtual address to EEPROM driver fail.
 *
 * @note This API should be called first, before any other EEPROM API. It requires passing
 *           the same numbers of virtual address to EEPROM driver as the variable number
 *           that need to write to the EEPROM.
 */
RET_CODE EEPROM_SetVirtualAddrTable(uint16_t *pVirtualAddr, uint8_t num);

/**
 * @brief Initialize EEPROM driver.
 *
 * @retval RET_OK EEPROM driver init success.
 * @retval RET_ERROR EEPROM driver init fail.
 * @note This API should be called just after @ref EEPROM_SetVirtualAddrTable.
 */
RET_CODE EEPROM_Init(void);

/**
 * @brief Read data from EEPROM.
 *
 * @param virtualAddr The data's virtual address in EEPROM.
 * @param pValue Store the readback value of the data.
 *
 * @retval RET_OK EEPROM read data success.
 * @retval RET_ERROR EEPROM read data fail.
 */
RET_CODE EEPROM_Read(uint16_t virtualAddr, uint16_t *pValue);

/**
 * @brief Write data to EEPROM.
 *
 * @param virtualAddr Specify the virtual address of the EEPROM that need to write to.
 * @param value Specify the data's value.
 *
 * @retval RET_OK EEPROM write data success.
 * @retval RET_ERROR EEPROM write data fail.
 */
RET_CODE EEPROM_Write(uint16_t virtualAddr, uint16_t value);

/**
 * @brief Format emulated EEPROM and restore to an valid init state.
 *
 *
 * @retval RET_OK EEPROM format success.
 * @retval RET_ERROR EEPROM format fail.
 */
RET_CODE EEPROM_Format(void);


#ifdef __cplusplus
}
#endif

#endif


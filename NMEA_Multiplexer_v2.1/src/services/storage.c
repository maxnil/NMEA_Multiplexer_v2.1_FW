/*
 * storage.c
 *
 * Created: 2021-09-26 22:03:39
 *  Author: max
 */ 

#include <asf.h>
#include <string.h>

#include "services/storage.h"

/************************************************************************************
 * storage_init()
 * Initialize 'storage disk'
 ************************************************************************************/
int storage_init() {
    int status;
        
    status = flash_init(FLASH_ACCESS_MODE_128, 6);
    if (status != FLASH_RC_OK) {
        printf("### ERR: flash_init() failed");
    }
    
    return status;
}

    
/************************************************************************************
 * storage_erase()
 * Erases all stored data in the 'storage disk'
 ************************************************************************************/
int storage_erase_all() {
    int status;
    
    status = flash_unlock(STORAGE_ADDR, STORAGE_ADDR + IFLASH_PAGE_SIZE - 1, 0, 0);
    if (status !=  FLASH_RC_OK) {
        printf("## ERR: flash_unlock() failed %d\r\n", status);
        return status;
    }

    status = flash_erase_page(STORAGE_ADDR, IFLASH_ERASE_PAGES_16);
    if (status !=  FLASH_RC_OK) {
        printf("## ERR: flash_erase_page() failed %d\r\n", status);
        return status;
    }
    
    status = flash_lock(STORAGE_ADDR, STORAGE_ADDR + IFLASH_PAGE_SIZE - 1, 0, 0);
    if (status !=  FLASH_RC_OK) {
        printf("## ERR: flash_lock() failed %d\r\n", status);
    }
    
    return status;
}

    
/************************************************************************************
 * storage_read_file()
 * Get stored data from storage disk
 ************************************************************************************/
int storage_read_file(int file_nr, char* rbuf) {
    int i;
    char* storage_ptr;
    
    storage_ptr = (char*)(STORAGE_ADDR + (file_nr * FILE_MAX_SIZE));

    for(i = 0; (i < FILE_MAX_SIZE-1) && (*storage_ptr != 0x00) && (*storage_ptr != 0xFF); i++) {
        *rbuf++ = *storage_ptr++;
    }
    
    *rbuf++ = 0x00;
    return i;
}

/************************************************************************************
 * storage_write_file()
 * Stored data in storage disk
 ************************************************************************************/
int storage_write_file(int file_nr, char* wbuf) {
    int size;
    int status;
     
    status = flash_unlock(STORAGE_ADDR, STORAGE_ADDR + IFLASH_PAGE_SIZE - 1, 0, 0);
    if (status !=  FLASH_RC_OK) {
        printf("## ERR: flash_unlock() failed %d\r\n", status);
        return status;
    }
    
//    printf("Wsize0 = %d\r\n", strlen(wbuf));
    size = min(strlen(wbuf) + 1, FILE_MAX_SIZE);
//    printf("Wsize1 = %d\r\n", size);
    size = (size + 3) & 0xFFFC;  // Round up to nearest uint32_t
//    printf("Wsize2 = %d\r\n", size);
    
    taskENTER_CRITICAL();
    status = flash_write(STORAGE_ADDR + (file_nr * FILE_MAX_SIZE), wbuf, size, 0);
    taskEXIT_CRITICAL();

    if (status != FLASH_RC_OK) {
        printf("## ERR: flash_write() failed %d\r\n", status);
        return status;
    }
    
    status = flash_lock(STORAGE_ADDR, STORAGE_ADDR + IFLASH_PAGE_SIZE - 1, 0, 0);
    if (status !=  FLASH_RC_OK) {
        printf("## ERR: flash_lock() failed %d\r\n", status);
    }
    
    return status;
}

/*
 * storage.h
 *
 * Created: 2021-09-26 22:03:26
 *  Author: max
 */ 


#ifndef STORAGE_H_
#define STORAGE_H_

#define FILE_MAX_SIZE 1024

#define FLASH_SIZE (512 * 1024)
#define TOP_OF_FLASH (0x00400000 + FLASH_SIZE)
#define STORAGE_SIZE (8 * FILE_MAX_SIZE)
#define STORAGE_ADDR (0x480000 - STORAGE_SIZE)

/************************************************************************************
 * storage_init()
 * Initialize the storage
 ************************************************************************************/
int storage_init(void);


/************************************************************************************
 * storage_erase()
 * Erases all stored data in the 'storage disk'
 ************************************************************************************/
int storage_erase_all(void);


/************************************************************************************
 * storage_read_file()
 * Get stored data from 'storage disk'
 ************************************************************************************/
int storage_read_file(int file_nr, char* rbuf);


/************************************************************************************
 * storage_write_file()
 * Stored data to 'storage disk'
 ************************************************************************************/
int storage_write_file(int file_nr, char* wbuf);

#endif /* STORAGE_H_ */
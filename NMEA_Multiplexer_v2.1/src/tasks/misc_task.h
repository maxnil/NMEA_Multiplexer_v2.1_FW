/*
 * misc_task.h
 *
 * Created: 2021-10-09 00:02:42
 *  Author: max
 */ 


#ifndef MISC_TASK_H_
#define MISC_TASK_H_

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ INCLUDES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "task.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DEFINES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* Task priorities */
#define MISC_TASK_PRIORITY		(tskIDLE_PRIORITY + 1)

/* Task stack sizes */
#define MISC_TASK_STACK_SIZE	(configMINIMAL_STACK_SIZE)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* MISC Task creator */
void create_misc_task(void);

#endif /* MISC_TASK_H_ */
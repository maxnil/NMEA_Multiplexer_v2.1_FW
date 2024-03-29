/*
 * bt_task.h
 *
 * Created: 2017-09-01 23:28:39
 *  Author: Max
 */


#ifndef BT_TASK_H_
#define BT_TASK_H_

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ INCLUDES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "task.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DEFINES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* Task priorities */
#define BT_TASK_PRIORITY		(tskIDLE_PRIORITY + 1)

/* Task stack sizes */
#define BT_TASK_STACK_SIZE	(configMINIMAL_STACK_SIZE * 2)

#define BT_RX_BUFFER_SIZE	100

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* BT Task creator */
void create_bt_task(void);

#endif /* BT_TASK_H_ */
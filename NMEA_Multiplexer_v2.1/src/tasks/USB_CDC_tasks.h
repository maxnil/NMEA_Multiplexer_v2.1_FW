/*
 * USB_CDC_CLI_task.h
 *
 * Created: 2015-06-26 21:22:02
 *  Author: Max
 */


#ifndef USB_CDC_CLI_TASK_H_
#define USB_CDC_CLI_TASK_H_

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ INCLUDES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "task.h"
//#include "compiler.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DEFINES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* Task priorities */
#define CDC_CLI_TASK_PRIORITY		(tskIDLE_PRIORITY + 1)
#define CDC_NMEA_TASK_PRIORITY		(tskIDLE_PRIORITY + 1)

/* Task stack sizes */
#define CDC_CLI_TASK_STACK_SIZE		(4096)
#define CDC_NMEA_TASK_STACK_SIZE	(2048)

/* USB CDC Task creator */
void create_usb_cdc_tasks(void);

#endif /* USB_CDC_CLI_TASK_H_ */
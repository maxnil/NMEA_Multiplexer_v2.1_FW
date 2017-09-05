/*
 * bt_task.c
 *
 * Created: 2017-09-01 23:28:27
 *  Author: Max
 */ 

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ INCLUDES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* Standard includes. */
#include <stdint.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "freertos_usart_serial.h"
#include "task.h"
#include "queue.h"

#include "bt_task.h"
#include "task_queues.h"

//~~~~~~~~~~~~~~~~~~~~~~~~ LOCAL FUNCTION DECLARATIONS ~~~~~~~~~~~~~~~~~~~~~~~~~

static void bt_task(void *pvParameters);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


/*******************************************************************************
 * BT Tasks creator
 */
void create_bt_task(void) {
	
	/* Create the BT USART task. */
	xTaskCreate(bt_task,						/* The task that implements the GPS data handler */
				(const char *const) "BT",		/* Text name assigned to the task.  This is just to assist debugging.  The kernel does not use this name itself. */
				BT_TASK_STACK_SIZE,				/* The size of the stack allocated to the task. */
				NULL,							/* The parameter is not used. */
				BT_TASK_PRIORITY,				/* The priority allocated to the task. */
				NULL);							/* Used to store the handle to the created task - in this case the handle is not required. */
}


/*******************************************************************************
 * BlueTooth Task
 * Reads data from BlueTooth UART, and sends it to all enlisted ports
 */
static void bt_task(void *pvParameters) {
	uint8_t buffer[10];
	int bytes = 0;
	int timeout = 0;

//	printf("bt_task\n");

	for (;;) {	
		bytes = freertos_usart_serial_read_packet(CONF_UART_BT, buffer, 1, 1000/portTICK_PERIOD_MS);
		if (bytes > 0) {
			printf("BT: 0x%.2X, %c (%d)\n\r", buffer[0], buffer[0], bytes);
		}
	}
}
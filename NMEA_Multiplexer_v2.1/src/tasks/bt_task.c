/*
 * bt_task.c
 *
 * Created: 2017-09-01 23:28:27
 *  Author: Max
 */

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ INCLUDES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <asf.h>

/* Standard includes. */
#include <stdint.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "freertos_usart_serial.h"
#include "task.h"
#include "queue.h"

#include "bt_task.h"
#include "conf_uart_serial.h"
#include "task_queues.h"

//~~~~~~~~~~~~~~~~~~~~~~~~ LOCAL FUNCTION DECLARATIONS ~~~~~~~~~~~~~~~~~~~~~~~~~

static void bt_task(void *pvParameters);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

int cnt;

/*******************************************************************************
 * BT Tasks creator
 */
void create_bt_task(void) {
  cnt = 0;
	/* Create the BT USART task. */
	xTaskCreate(bt_task,						/* The task that implements the GPS data handler */
				(const char *const) "BT",		/* Text name assigned to the task.  This is just to assist debugging.  The kernel does not use this name itself. */
				BT_TASK_STACK_SIZE,				/* The size of the stack allocated to the task. */
				NULL,							/* The parameter is not used. */
				BT_TASK_PRIORITY,				/* The priority allocated to the task. */
				NULL);							/* Used to store the handle to the created task - in this case the handle is not required. */
}


#include "queue.h"
#include "task_queues.h"
#include "led_timer_task.h"

/*******************************************************************************
 * BlueTooth Task
 * Reads data from BlueTooth UART, and sends it to all enlisted ports
 */
static void bt_task(void *pvParameters) {
	uint8_t buffer[10];
	int bytes = 0;

	for (;;) {
		//bytes = freertos_usart_serial_read_packet(CONF_UART_BT, buffer, 1, 1000/portTICK_PERIOD_MS);
		//if (bytes > 0) {
		//	printf("BT: 0x%.2X, %c (%d)\n\r", buffer[0], buffer[0], bytes);
	  vTaskDelay(1000 / portTICK_PERIOD_MS);
	  led_blink(LED_BT_RX, LED_SHORT_BLINK);

	  vTaskDelay(1000 / portTICK_PERIOD_MS);
	  led_blink(LED_BT_TX, LED_SHORT_BLINK);
	//	}
	}
}
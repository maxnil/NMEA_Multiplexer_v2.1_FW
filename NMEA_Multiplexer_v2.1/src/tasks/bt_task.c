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

#include "tasks/led_timer_task.h"
#include "tasks/bt_task.h"
#include "tasks/nmea_port_tasks.h"
#include "conf_uart_serial.h"

//~~~~~~~~~~~~~~~~~~~~~~~~ LOCAL FUNCTION DECLARATIONS ~~~~~~~~~~~~~~~~~~~~~~~~~

static void bt_tx_task(void *pvParameters);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*******************************************************************************
 * BT Tasks creator
 */
void create_bt_task() {
    /* Create the BT USART task. */
    xTaskCreate(bt_tx_task, (const char *const) "NMEA_BT_TX", BT_TASK_STACK_SIZE, NULL, BT_TASK_PRIORITY, NULL);
}


/*******************************************************************************
 * BlueTooth Task
 * Reads data from BlueTooth UART, and sends it to all enlisted ports
 */
static void bt_tx_task(void *pvParameters) {
	char* tx_buffer;			// Temporary Tx Buffer pointer
    
    printf("Hello from BT task\r\n");

	for (;;) {
    	if (xQueueReceive(nmea_tx_queues[NMEA_PORT_TASK_BT_PORT], &tx_buffer, portMAX_DELAY) == pdTRUE) {
        	led_blink(LED_BT_TX, LED_SHORT_FLASH);
        	if (freertos_usart_write_packet(CONF_UART_BT, tx_buffer, strlen(tx_buffer), portMAX_DELAY) != STATUS_OK) {
            	printf("NMEA BT usart_write failed\r\n");
        	}
        	vPortFree(tx_buffer);
    	}
	}
}

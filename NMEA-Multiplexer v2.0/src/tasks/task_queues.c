/*
 * task_queues.c
 *
 * Created: 2017-08-08 15:12:33
 *  Author: Max
 */ 

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "queue.h"

#include "task_queues.h"

QueueHandle_t led_queue;
QueueHandle_t nmea_tx_queue[6];
QueueHandle_t nmea_usb_rx_queue;


void create_task_queues(void) {
	int i;
	
	/* Create CLI Response and Data Channel queues */
	led_queue = xQueueCreate(20, sizeof(uint8_t*));
	nmea_usb_rx_queue = xQueueCreate(10, sizeof(uint8_t*));

	for (i = 0; i < 6; i++) {
		nmea_tx_queue[i] = xQueueCreate(10, sizeof(int8_t*));
	}
}
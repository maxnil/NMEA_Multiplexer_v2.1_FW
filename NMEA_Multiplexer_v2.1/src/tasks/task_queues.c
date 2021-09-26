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
#include "led_timer_task.h"

QueueHandle_t led_queue;
QueueHandle_t nmea_tx_queue[6];
QueueHandle_t nmea_usb_rx_queue;


void create_task_queues(void) {
	int i;

	/* LED control queue */
	led_queue = xQueueCreate(CONF_NMEA_MUX_LED_QUEUE_SIZE, sizeof(struct LedMsg_t));

	/* CLI Response and Data Channel queues */
	nmea_usb_rx_queue = xQueueCreate(CONF_NMEA_MUX_USB_RX_QUEUE_SIZE, sizeof(uint8_t*));

	/* One TX queue for each NMEA port (including BlueTooth port) */
	for (i = 0; i < 6; i++) {
		nmea_tx_queue[i] = xQueueCreate(CONF_NMEA_MUX_NMEA_TX_QUEUE_SIZE, sizeof(int8_t*));
	}
}
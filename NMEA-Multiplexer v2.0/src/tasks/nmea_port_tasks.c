/*
 * nmea_port_tasks.c
 *
 * Created: 2017-08-08 23:25:41
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

#include "nmea_port_tasks.h"
#include "task_queues.h"

#include "services/led.h"
#include "services/nmea_func.h"

//~~~~~~~~~~~~~~~~~~~~~~~~ LOCAL FUNCTION DECLARATIONS ~~~~~~~~~~~~~~~~~~~~~~~~~

static void nmea_rx_task(void *pvParameters);
static void nmea_tx_task(void *pvParameters);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

typedef struct _nmea_task_pvParameters_t {
	int port_nr;
	freertos_usart_if p_usart;
	nmea_node_t* nmea_search_tree;
	ioport_pin_t rx_led;
	ioport_pin_t tx_led;
	ioport_pin_t err_led;
} nmea_task_pvParameters_t;


nmea_node_t* nmea_search_tree[5] = {NULL, NULL, NULL, NULL, NULL};

/*******************************************************************************
 * NMEA Port Tasks creator
 */
void create_nmea_port_tasks(void) {
	nmea_task_pvParameters_t* nmea_task_pvParameters[5];
	
	int i;
	
	for (i = 0; i < 5; i++) {
		nmea_task_pvParameters[i] = (nmea_task_pvParameters_t*)pvPortMalloc(sizeof(nmea_task_pvParameters_t));
		nmea_task_pvParameters[i]->nmea_search_tree = nmea_search_tree[i];
		nmea_task_pvParameters[i]->port_nr = i;
	}
	
	/* Create the NMEA Port 1 USART Rx Task. */
	i = 0;
	nmea_task_pvParameters[i]->p_usart = CONF_UART_NMEA_PORT_1;
	nmea_task_pvParameters[i]->rx_led = PORT_1_RX_LED_PIN;
	nmea_task_pvParameters[i]->tx_led = PORT_1_TX_LED_PIN;
	nmea_task_pvParameters[i]->err_led = PORT_1_ERR_LED_PIN;
	
	xTaskCreate(nmea_rx_task,
		(const char *const) "NMEA_1_RX",
		NMEA_PORT_TASK_STACK_SIZE,
		nmea_task_pvParameters[i],
		NMEA_PORT_TASK_PRIORITY,
		NULL);
	
	
	/* Create the NMEA Port 2 USART Rx Task. */
	i = 1;
	nmea_task_pvParameters[i]->p_usart = CONF_UART_NMEA_PORT_2;
	nmea_task_pvParameters[i]->rx_led = PORT_2_RX_LED_PIN;
	nmea_task_pvParameters[i]->tx_led = PORT_2_TX_LED_PIN;
	nmea_task_pvParameters[i]->err_led = PORT_2_ERR_LED_PIN;

	xTaskCreate(nmea_rx_task,
		(const char *const) "NMEA_2_RX",
		NMEA_PORT_TASK_STACK_SIZE,
		nmea_task_pvParameters[i],
		NMEA_PORT_TASK_PRIORITY,
		NULL);


	/* Create the NMEA Port 3 USART Rx Task. */
	i = 2;
	nmea_task_pvParameters[i]->p_usart = CONF_UART_NMEA_PORT_3;
	nmea_task_pvParameters[i]->rx_led = PORT_3_RX_LED_PIN;
	nmea_task_pvParameters[i]->tx_led = PORT_3_TX_LED_PIN;
	nmea_task_pvParameters[i]->err_led = PORT_3_ERR_LED_PIN;

	xTaskCreate(nmea_rx_task,
		(const char *const) "NMEA_3_RX",
		NMEA_PORT_TASK_STACK_SIZE,
		nmea_task_pvParameters[i],
		NMEA_PORT_TASK_PRIORITY,
		NULL);

	
	/* Create the NMEA Port 4 USART Rx Task. */
	i = 3;
	nmea_task_pvParameters[i]->p_usart = CONF_UART_NMEA_PORT_4;
	nmea_task_pvParameters[i]->rx_led = PORT_4_RX_LED_PIN;
	nmea_task_pvParameters[i]->tx_led = PORT_4_TX_LED_PIN;
	nmea_task_pvParameters[i]->err_led = PORT_4_ERR_LED_PIN;

	xTaskCreate(nmea_rx_task,
		(const char *const) "NMEA_4_RX",
		NMEA_PORT_TASK_STACK_SIZE,
		nmea_task_pvParameters[i],
		NMEA_PORT_TASK_PRIORITY,
		NULL);

	
	/* Create the NMEA Port 5 USART Rx Task. */
	i = 4;
	nmea_task_pvParameters[i]->p_usart = CONF_UART_NMEA_PORT_5;
	nmea_task_pvParameters[i]->rx_led = PORT_5_RX_LED_PIN;
	nmea_task_pvParameters[i]->tx_led = PORT_5_TX_LED_PIN;
	nmea_task_pvParameters[i]->err_led = PORT_5_ERR_LED_PIN;

	xTaskCreate(nmea_rx_task,
		(const char *const) "NMEA_5_RX",
		NMEA_PORT_TASK_STACK_SIZE,
		nmea_task_pvParameters[i],
		NMEA_PORT_TASK_PRIORITY,
		NULL);


	/* Create the NMEA Port 1 USART Tx Task. */
	i = 0;
	xTaskCreate(nmea_tx_task,
		(const char *const) "NMEA_1_TX",
		NMEA_PORT_TASK_STACK_SIZE,
		nmea_task_pvParameters[i],
		NMEA_PORT_TASK_PRIORITY,
		NULL);

	/* Create the NMEA Port 2 USART Tx Task. */
	i = 1;
	xTaskCreate(nmea_tx_task,
		(const char *const) "NMEA_2_TX",
		NMEA_PORT_TASK_STACK_SIZE,
		nmea_task_pvParameters[i],
		NMEA_PORT_TASK_PRIORITY,
		NULL);

	/* Create the NMEA Port 3 USART Tx Task. */
	i = 2;
	xTaskCreate(nmea_tx_task,
		(const char *const) "NMEA_3_TX",
		NMEA_PORT_TASK_STACK_SIZE,
		nmea_task_pvParameters[i],
		NMEA_PORT_TASK_PRIORITY,
		NULL);

	/* Create the NMEA Port 4 USART Tx Task. */
	i = 3;
	xTaskCreate(nmea_tx_task,
		(const char *const) "NMEA_4_TX",
		NMEA_PORT_TASK_STACK_SIZE,
		nmea_task_pvParameters[i],
		NMEA_PORT_TASK_PRIORITY,
		NULL);

	/* Create the NMEA Port 5 USART Tx Task. */
	i = 4;
	xTaskCreate(nmea_tx_task,
		(const char *const) "NMEA_5_TX",
		NMEA_PORT_TASK_STACK_SIZE,
		nmea_task_pvParameters[i],
		NMEA_PORT_TASK_PRIORITY,
		NULL);
}


		
/*******************************************************************************
 * NMEA Rx Task
 * Reads NMEA data from NMEA UART, and sends it to all enlisted ports
 */
static void nmea_rx_task(void *pvParameters) {
	nmea_task_pvParameters_t* param;
	
	uint8_t* rx_buffer;			// Rx Buffer start pointer
	uint8_t* tx_buffer;			// Temporary Tx Buffer pointer
	uint8_t port_mask;
	int buf_index;

	/* Task parameters */
	param = (nmea_task_pvParameters_t*)pvParameters;
	
	/* Allocate NMEA receive buffer */
	rx_buffer = (uint8_t*)pvPortMalloc(128);

	for (;;) {
restart:
		/* Find start of NMEA sentence ('$') */
		do {
			/* Get one character from NMEA UART, ignore timeout */
			freertos_usart_serial_read_packet(param->p_usart, rx_buffer, 1, portMAX_DELAY);
		} while (rx_buffer[0] != '$');
		
		// Turn on the Rx LED
		LED_On(param->rx_led);

		buf_index = 1;

		do {
			/* Get one character from NMEA UART, if time */
			if (freertos_usart_serial_read_packet(param->p_usart, &rx_buffer[buf_index], 1, portMAX_DELAY) == 0) {
				LED_Off(param->rx_led);
				goto restart;
			}
		} while (rx_buffer[buf_index++] != 0x0A);
		
		LED_Off(param->rx_led);
		
		rx_buffer[buf_index++] = 0x00;	// Terminate string

		port_mask = nmea_get_port_mask(param->nmea_search_tree, (char*)rx_buffer);

		if (port_mask == 0xFF) {
			printf("NMEA %d: --   %s", param->port_nr, rx_buffer);
			taskENTER_CRITICAL();
			nmea_tree_add(param->nmea_search_tree, (char*)rx_buffer, 0x00);
			taskEXIT_CRITICAL();
			port_mask = 0;
//		} else {
//			printf("NMEA %d: 0x%.2X %s", param->port_nr, port_mask, rx_buffer);
		}
		
		// Copy to NMEA 1
		if (port_mask & (1<<0)) {
			tx_buffer = (uint8_t*)pvPortMalloc(buf_index * sizeof(uint8_t));
			strcpy(tx_buffer, rx_buffer);
			if (!xQueueSend(nmea_tx_queue[0], &tx_buffer, 0)) {
				printf("Failed to send to NMEA 1 Tx queue\n\r");
				// Turn on the Err LED
				LED_On(PORT_1_ERR_LED_PIN);
			}
		}
		
		// Copy to NMEA 2
		if (port_mask & (1<<1)) {
			tx_buffer = (uint8_t*)pvPortMalloc(buf_index * sizeof(uint8_t));
			strcpy(tx_buffer, rx_buffer);
			if (!xQueueSend(nmea_tx_queue[1], &tx_buffer, 0)) {
				printf("Failed to send to NMEA 2 Tx queue\n\r");
				// Turn on the Err LED
				LED_On(PORT_2_ERR_LED_PIN);
			}
		}
		
		// Copy to NMEA 3
		if (port_mask & (1<<2)) {
			tx_buffer = (uint8_t*)pvPortMalloc(buf_index * sizeof(uint8_t));
			strcpy(tx_buffer, rx_buffer);
			if (!xQueueSend(nmea_tx_queue[2], &tx_buffer, 0)) {
				printf("Failed to send to NMEA 3 Tx queue\n\r");
				// Turn on the Err LED
				LED_On(PORT_3_ERR_LED_PIN);
			}
		}
		
		// Copy to NMEA 4
		if (port_mask & (1<<3)) {
			tx_buffer = (uint8_t*)pvPortMalloc(buf_index * sizeof(uint8_t));
			strcpy(tx_buffer, rx_buffer);
			if (!xQueueSend(nmea_tx_queue[3], &tx_buffer, 0)) {
				printf("Failed to send to NMEA 4 Tx queue\n\r");
				// Turn on the Err LED
				LED_On(PORT_4_ERR_LED_PIN);
			}
		}

		// Copy to NMEA 5
		if (port_mask & (1<<4)) {
			tx_buffer = (uint8_t*)pvPortMalloc(buf_index * sizeof(uint8_t));
			strcpy(tx_buffer, rx_buffer);
			if (!xQueueSend(nmea_tx_queue[4], &tx_buffer, 0)) {
				printf("Failed to send to NMEA 5 Tx queue\n\r");
				// Turn on the Err LED
				LED_On(PORT_5_ERR_LED_PIN);
			}
		}
		
		// Copy to BT
		if (port_mask & (1<<5)) {
			tx_buffer = (uint8_t*)pvPortMalloc(buf_index * sizeof(uint8_t));
			strcpy(tx_buffer, rx_buffer);
			if (!xQueueSend(nmea_tx_queue[5], &tx_buffer, 0)) {
				printf("Failed to send to BT Tx queue\n\r");
			}
		}
		
	}
}


/*******************************************************************************
 * NMEA Tx Task
 * Writes NMEA data to NMEA UART
 */
static void nmea_tx_task(void *pvParameters) {
	nmea_task_pvParameters_t* param;
	uint8_t* tx_buffer;			// Temporary Tx Buffer pointer

	/* Task parameters */
	param = (nmea_task_pvParameters_t*)pvParameters;
	
	for (;;) {
		if (xQueueReceive(nmea_tx_queue[param->port_nr], &tx_buffer, portMAX_DELAY) == pdTRUE) {
			LED_On(param->tx_led);
			if (freertos_usart_write_packet(param->p_usart, tx_buffer, strlen(tx_buffer), portMAX_DELAY) != STATUS_OK) {
				printf("NMEA %d usart_write failed\n\r", param->port_nr);
				LED_On(param->err_led);
			}
			LED_Off(param->tx_led);
			vPortFree(tx_buffer);
		}
	}
}

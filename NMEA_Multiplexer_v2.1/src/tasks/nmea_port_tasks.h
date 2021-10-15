/*
 * nmea_port_tasks.h
 *
 * Created: 2017-08-08 23:25:01
 *  Author: Max
 */ 


#ifndef NMEA_PORT_TASKS_H_
#define NMEA_PORT_TASKS_H_

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ INCLUDES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "FreeRTOS.h"
#include "queue.h"
#include "config/conf_nmea_mux.h"
//#include "services/nmea_func.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DEFINES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define NMEA_PORT_TASK_NR_NMEA_PORTS (CONF_NMEA_MUX_NR_NMEA_UART_PORTS + CONF_NMEA_MUX_NR_NMEA_USB_PORTS  + CONF_NMEA_MUX_NR_NMEA_BT_PORTS)

#define NMEA_PORT_TASK_UART_PORT_1 0
#define NMEA_PORT_TASK_UART_PORT_2 1
#define NMEA_PORT_TASK_UART_PORT_3 2
#define NMEA_PORT_TASK_UART_PORT_4 3
#define NMEA_PORT_TASK_UART_PORT_5 4
#define NMEA_PORT_TASK_BT_PORT 5
#define NMEA_PORT_TASK_USB_PORT 6

/* Task priorities */
#define NMEA_PORT_TASK_PRIORITY		(tskIDLE_PRIORITY + 1)

/* Task stack sizes */
//#define NMEA_PORT_TASK_STACK_SIZE	(configMINIMAL_STACK_SIZE * 3)
#define NMEA_PORT_TASK_STACK_SIZE	(1024)

#define NMEA_PORT_RX_BUFFER_SIZE	100

extern QueueHandle_t nmea_tx_queues[NMEA_PORT_TASK_NR_NMEA_PORTS];    // NMEA Tx Data queues

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* NMEA Task creator */
void create_nmea_port_tasks(void);

#endif /* NMEA_PORT_TASKS_H_ */
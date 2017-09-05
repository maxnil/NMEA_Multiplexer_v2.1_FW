/*
 * nmea_port_tasks.h
 *
 * Created: 2017-08-08 23:25:01
 *  Author: Max
 */ 


#ifndef NMEA_PORT_TASKS_H_
#define NMEA_PORT_TASKS_H_

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ INCLUDES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "task.h"
#include "services/nmea_func.h"
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DEFINES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* Task priorities */
#define NMEA_PORT_TASK_PRIORITY		(tskIDLE_PRIORITY + 1)

/* Task stack sizes */
//#define NMEA_PORT_TASK_STACK_SIZE	(configMINIMAL_STACK_SIZE * 3)
#define NMEA_PORT_TASK_STACK_SIZE	(1024)

#define NMEA_PORT_RX_BUFFER_SIZE	100

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

extern nmea_node_t* nmea_search_tree[5];

/* NMEA Task creator */
void create_nmea_port_tasks(void);

#endif /* NMEA_PORT_TASKS_H_ */
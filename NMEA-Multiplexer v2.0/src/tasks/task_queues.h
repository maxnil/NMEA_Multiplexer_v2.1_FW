/*
 * task_queues.h
 *
 * Created: 2016-02-25 22:43:16
 *  Author: Max
 */ 


#ifndef TASK_QUEUES_H_
#define TASK_QUEUES_H_

#include "queue.h"

extern QueueHandle_t led_queue;
extern QueueHandle_t nmea_tx_queue[6];
extern QueueHandle_t nmea_usb_rx_queue;

void create_task_queues(void);

#endif /* TASK_QUEUES_H_ */
/*
 * timer_task.h
 *
 * Created: 2016-02-10 22:56:47
 *  Author: Max
 */


#ifndef TIMER_TASK_H_
#define TIMER_TASK_H_

#include <asf.h>
#include "conf_nmea_mux.h"


#define LED_ON           (  -1)
#define LED_OFF          (   0)
#define LED_SHORT_FLASH  (  20 / CONF_NMEA_MUX_LED_TIMER_PERIOD_MS)
#define LED_MEDIUM_FLASH ( 500 / CONF_NMEA_MUX_LED_TIMER_PERIOD_MS)
#define LED_LONG_FLASH   (2000 / CONF_NMEA_MUX_LED_TIMER_PERIOD_MS)

#define NR_LEDS 17

// LED indexes
#define LED_PORT_1_RX	0
#define LED_PORT_2_RX	1
#define LED_PORT_3_RX	2
#define LED_PORT_4_RX	3
#define LED_PORT_5_RX	4
#define LED_BT_RX		5
#define LED_PORT_1_TX	6
#define LED_PORT_2_TX	7
#define LED_PORT_3_TX	8
#define LED_PORT_4_TX	9
#define LED_PORT_5_TX   10
#define LED_BT_TX		11
#define LED_PORT_1_ERR  12
#define LED_PORT_2_ERR	13
#define LED_PORT_3_ERR	14
#define LED_PORT_4_ERR	15
#define LED_PORT_5_ERR	16

void create_led_timer_task(void);

void led_blink(uint8_t led_nr, uint16_t on_cycles);

#endif /* TIMER_TASK_H_ */
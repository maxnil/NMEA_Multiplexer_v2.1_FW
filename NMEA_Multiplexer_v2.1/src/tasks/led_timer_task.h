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

#define LED_SHORT_BLINK  (  25 / CONF_NMEA_MUX_LED_TIMER_PERIOD_MS)
#define LED_MEDIUM_BLINK ( 500 / CONF_NMEA_MUX_LED_TIMER_PERIOD_MS)
#define LED_LONG_BLINK   (2000 / CONF_NMEA_MUX_LED_TIMER_PERIOD_MS)

#define NR_LEDS 17

// LED indexes
#define LED_PORT_1_ERR	0
#define LED_PORT_2_ERR	1
#define LED_PORT_3_ERR	2
#define LED_PORT_4_ERR	3
#define LED_PORT_5_ERR	4
#define LED_PORT_1_RX	5
#define LED_PORT_2_RX	6
#define LED_PORT_3_RX	7
#define LED_PORT_4_RX	8
#define LED_PORT_5_RX	9
#define LED_BT_RX		10
#define LED_PORT_1_TX	11
#define LED_PORT_2_TX	12
#define LED_PORT_3_TX	13
#define LED_PORT_4_TX	14
#define LED_PORT_5_TX	15
#define LED_BT_TX		16

struct LedMsg_t {
  uint16_t on_cycles;  // Number of milliseconds x10 that the LED will be on
  uint8_t led_nr;
};

void create_led_timer_task(void);

void led_blink(uint8_t led_nr, uint16_t on_cycles);

#endif /* TIMER_TASK_H_ */
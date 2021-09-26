/*
 * led.c
 *
 * Created: 2017-08-07 16:21:51
 *  Author: Max
 */

#include <asf.h>
#include "FreeRTOS.h"
#include "task.h"
#include "led.h"
#include "user_board.h"

/**
 * Turns LED On
 */
void LED_On(ioport_pin_t led_gpio) {
	taskENTER_CRITICAL();
	ioport_set_pin_level(led_gpio, IOPORT_PIN_LEVEL_HIGH);
	taskEXIT_CRITICAL();
}


/**
 * Turns LED Off
 */
void LED_Off(ioport_pin_t led_gpio) {
	taskENTER_CRITICAL();
	ioport_set_pin_level(led_gpio, IOPORT_PIN_LEVEL_LOW);
	taskEXIT_CRITICAL();
}


/**
 * Toggles LED
 */
void LED_Toggle(ioport_pin_t led_gpio) {
	taskENTER_CRITICAL();
	ioport_toggle_pin_level(led_gpio);
	taskEXIT_CRITICAL();
}


/**
 * LED test
 */
void LED_Test(int delay) {
	if (delay == 0)
		return;

	/** LED test for TX LED's */
	LED_On(LED_BT_TX_PIN);
	LED_On(LED_PORT_1_TX_PIN);
	LED_On(LED_PORT_2_TX_PIN);
	LED_On(LED_PORT_3_TX_PIN);
	LED_On(LED_PORT_4_TX_PIN);
	LED_On(LED_PORT_5_TX_PIN);
	delay_ms(delay);
	LED_Off(LED_BT_TX_PIN);
	LED_Off(LED_PORT_1_TX_PIN);
	LED_Off(LED_PORT_2_TX_PIN);
	LED_Off(LED_PORT_3_TX_PIN);
	LED_Off(LED_PORT_4_TX_PIN);
	LED_Off(LED_PORT_5_TX_PIN);

	/** LED test for ERR LED's */
	LED_On(LED_PORT_1_ERR_PIN);
	LED_On(LED_PORT_2_ERR_PIN);
	LED_On(LED_PORT_3_ERR_PIN);
	LED_On(LED_PORT_4_ERR_PIN);
	LED_On(LED_PORT_5_ERR_PIN);
	delay_ms(delay);
	LED_Off(LED_PORT_1_ERR_PIN);
	LED_Off(LED_PORT_2_ERR_PIN);
	LED_Off(LED_PORT_3_ERR_PIN);
	LED_Off(LED_PORT_4_ERR_PIN);
	LED_Off(LED_PORT_5_ERR_PIN);

	/** LED test for RX LED's */
	LED_On(LED_BT_RX_PIN);
	LED_On(LED_PORT_1_RX_PIN);
	LED_On(LED_PORT_2_RX_PIN);
	LED_On(LED_PORT_3_RX_PIN);
	LED_On(LED_PORT_4_RX_PIN);
	LED_On(LED_PORT_5_RX_PIN);
	delay_ms(delay);
	LED_Off(LED_BT_RX_PIN);
	LED_Off(LED_PORT_1_RX_PIN);
	LED_Off(LED_PORT_2_RX_PIN);
	LED_Off(LED_PORT_3_RX_PIN);
	LED_Off(LED_PORT_4_RX_PIN);
	LED_Off(LED_PORT_5_RX_PIN);
}

/*
 * timer_task.c
 *
 * Created: 2017-08-08 15:56:57
 *  Author: Max
 */ 

/* Drivers */
#include "services/led.h"

/* FreeRTOS */
#include "FreeRTOS.h"
#include "timers.h"

#include "tasks/timer_task.h"

static TimerHandle_t xLEDTimer;


/*
 * The callback function used by the software timer.
 */
static void LEDTimerCallback(void *pvParameters) {
	/* Toggle an LED to show the system is executing. */
	LED_Toggle(BT_RX_LED_PIN);
	ioport_set_pin_level(BT_TX_LED_PIN, !ioport_get_pin_level(BT_RX_LED_PIN));	// Just to make sure they blink in sync
}

void create_timer_task(const TickType_t xTimerPeriodInTicks) {

	/* Create the timer that toggles an LED to show that the system is running,
	and that the other tasks are behaving as expected. */
	xLEDTimer = xTimerCreate((const char * const) "LED timer",/* A text name, purely to help debugging. */
							xTimerPeriodInTicks,		/* The timer period. */
							pdTRUE,						/* This is an auto-reload timer, so xAutoReload is set to pdTRUE. */
							NULL,						/* The timer does not use its ID, so the ID is just set to NULL. */
							LEDTimerCallback			/* The function that is called each time the timer expires. */
							);

	/* Sanity check the timer's creation, then start the timer.  The timer
	will not actually start until the FreeRTOS kernel is started. */
	configASSERT(xLEDTimer);
	xTimerStart(xLEDTimer, 0);
}
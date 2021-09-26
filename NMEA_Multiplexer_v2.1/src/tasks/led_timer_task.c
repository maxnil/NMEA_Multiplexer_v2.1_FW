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

#include "config/conf_nmea_mux.h"

#include "tasks/led_timer_task.h"
#include "tasks/task_queues.h"

static TimerHandle_t xLEDTimer;

int led_on_cycles[NR_LEDS];

// Lookup table for LED pins related to LED indexes
uint8_t led_pins[NR_LEDS] = {
    LED_PORT_1_ERR_PIN,
    LED_PORT_2_ERR_PIN,
    LED_PORT_3_ERR_PIN,
    LED_PORT_4_ERR_PIN,
    LED_PORT_5_ERR_PIN,
    LED_PORT_1_RX_PIN,
    LED_PORT_2_RX_PIN,
    LED_PORT_3_RX_PIN,
    LED_PORT_4_RX_PIN,
    LED_PORT_5_RX_PIN,
    LED_BT_RX_PIN,
    LED_PORT_1_TX_PIN,
    LED_PORT_2_TX_PIN,
    LED_PORT_3_TX_PIN,
    LED_PORT_4_TX_PIN,
    LED_PORT_5_TX_PIN,
    LED_BT_TX_PIN
};

/*
* Generated a LED blink
*/
void led_blink(uint8_t led_nr, uint16_t on_cycles) {
    struct LedMsg_t led_msg;

    led_msg.led_nr = led_nr;
    led_msg.on_cycles = on_cycles;
    if (!xQueueSend(led_queue, &led_msg, 0)) {
        printf("Failed to send to LED queue\n\r");
    }
}


/*
* The callback function used by the software timer.
*/
static void LEDTimerCallback(TimerHandle_t xTimer) {
    struct LedMsg_t led_msg;
    int led_nr;

    // Get all new LED blink messages
    while (xQueueReceive(led_queue, &led_msg, 0) == pdTRUE) {
        if (led_on_cycles[led_msg.led_nr] < led_msg.on_cycles)
        led_on_cycles[led_msg.led_nr] = led_msg.on_cycles;
    }

    // Update LED state
    for (led_nr = 0; led_nr < NR_LEDS; led_nr++) {
        if (led_on_cycles[led_nr] > 0) {
            LED_On(led_pins[led_nr]);
            led_on_cycles[led_nr]--;
        } else {
            LED_Off(led_pins[led_nr]);
        }
    }
}

void create_led_timer_task() {
    int led_nr;
    for (led_nr = 0; led_nr < NR_LEDS; led_nr++)
    led_on_cycles[led_nr] = 0;

    /* Create the timer that toggles an LED to show that the system is running,
    and that the other tasks are behaving as expected. */
    xLEDTimer = xTimerCreate((const char * const) "LED timer", CONF_NMEA_MUX_LED_TIMER_PERIOD_MS / portTICK_PERIOD_MS, pdTRUE, NULL, LEDTimerCallback);

    /* Sanity check the timer's creation, then start the timer.  The timer
    will not actually start until the FreeRTOS kernel is started. */
    configASSERT(xLEDTimer);
    xTimerStart(xLEDTimer, 0);
}
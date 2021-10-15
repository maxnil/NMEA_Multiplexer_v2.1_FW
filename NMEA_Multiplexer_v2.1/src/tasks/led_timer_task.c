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

static TimerHandle_t xLEDTimer;
static QueueHandle_t led_cmd_queue;

int led_on_cycles[NR_LEDS];

struct LedCmd_t {
    uint16_t on_cycles;  // Number of milliseconds x10 that the LED will be on
    uint8_t led_nr;
};

// Lookup table for LED pins related to LED indexes
uint8_t led_pins[NR_LEDS] = {
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
    LED_BT_TX_PIN,
    LED_PORT_1_ERR_PIN,
    LED_PORT_2_ERR_PIN,
    LED_PORT_3_ERR_PIN,
    LED_PORT_4_ERR_PIN,
    LED_PORT_5_ERR_PIN
    };


/*
* Generated a LED blink
*/
void led_blink(uint8_t led_nr, uint16_t on_cycles) {
    struct LedCmd_t led_msg;

    if (led_nr > NR_LEDS)
        return;

    led_msg.led_nr = led_nr;
    led_msg.on_cycles = on_cycles;
    if (!xQueueSend(led_cmd_queue, &led_msg, 0)) {
        printf("###ERR: Failed to send to LED queue\r\n");
    }
}


/*
* The callback function used by the software timer.
*/
static void LEDTimerCallback(TimerHandle_t xTimer) {
    struct LedCmd_t led_cmd;

    // Get all new LED blink messages
    while (xQueueReceive(led_cmd_queue, &led_cmd, 0) == pdTRUE) {
        if (led_on_cycles[led_cmd.led_nr] < led_cmd.on_cycles || led_cmd.on_cycles <= 0) {
            led_on_cycles[led_cmd.led_nr] = led_cmd.on_cycles;
        }            
    }

    // Update LED state
    for (int led_nr = 0; led_nr < NR_LEDS; led_nr++) {
        if (led_on_cycles[led_nr] == 0) {
            LED_Off(led_pins[led_nr]);
        } else {
            LED_On(led_pins[led_nr]);
        }
        
        if (led_on_cycles[led_nr] > 0) {
            led_on_cycles[led_nr]--;
        }
    }                
}

void create_led_timer_task() {
    int led_nr;
    for (led_nr = 0; led_nr < NR_LEDS; led_nr++)
    led_on_cycles[led_nr] = 0;
    led_cmd_queue = xQueueCreate(CONF_NMEA_MUX_LED_CMD_QUEUE_SIZE, sizeof(struct LedCmd_t));

//    printf("Creating timer task\r\n");

    /* Create the timer that toggles an LED to show that the system is running,
    and that the other tasks are behaving as expected. */
    xLEDTimer = xTimerCreate((const char * const) "LED timer", CONF_NMEA_MUX_LED_TIMER_PERIOD_MS / portTICK_PERIOD_MS, pdTRUE, NULL, LEDTimerCallback);

    /* Sanity check the timer's creation, then start the timer.  The timer
    will not actually start until the FreeRTOS kernel is started. */
    configASSERT(xLEDTimer);
    xTimerStart(xLEDTimer, 0);
}
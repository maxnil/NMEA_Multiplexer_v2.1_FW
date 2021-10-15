/*
 * misc_task.c
 *
 * Created: 2021-10-09 00:02:14
 *  Author: max
 */ 

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ INCLUDES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//#include <asf.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

#include "tasks/led_timer_task.h"
#include "tasks/misc_task.h"

//~~~~~~~~~~~~~~~~~~~~~~~~ LOCAL FUNCTION DECLARATIONS ~~~~~~~~~~~~~~~~~~~~~~~~~

static void misc_task(void *pvParameters);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*******************************************************************************
 * MISC Task creator
 */
void create_misc_task() {
    /* Create the BT USART task. */
    xTaskCreate(misc_task, (const char *const) "MISC", MISC_TASK_STACK_SIZE, NULL, MISC_TASK_PRIORITY, NULL);
}


/*******************************************************************************
 * Misc Task
 * Blinks the BT Rx LED
 */
static void misc_task(void *pvParameters) {
	for (;;) {
        led_blink(LED_BT_RX, LED_ON);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        led_blink(LED_BT_RX, LED_OFF);
        vTaskDelay(500 / portTICK_PERIOD_MS);
	}
}
/*
 * nmea_port_tasks.c
 *
 * Created: 2017-08-08 23:25:41
 *  Author: Max
 */

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ INCLUDES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* Standard includes. */
#include <stdint.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "freertos_usart_serial.h"
#include "queue.h"
#include "task.h"

#include "services/nmea_func.h"
#include "tasks/nmea_port_tasks.h"
#include "tasks/led_timer_task.h"

//~~~~~~~~~~~~~~~~~~~~~~~~ LOCAL FUNCTION DECLARATIONS ~~~~~~~~~~~~~~~~~~~~~~~~~

static void nmea_rx_task(void *pvParameters);
static void nmea_tx_task(void *pvParameters);

QueueHandle_t nmea_tx_queues[NMEA_PORT_TASK_NR_NMEA_PORTS];    // NMEA Tx Data queues

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

typedef struct _nmea_task_pvParameters_t {
    int port_nr;
    freertos_usart_if p_usart;
    ioport_pin_t rx_led;
    ioport_pin_t tx_led;
    ioport_pin_t err_led;
} nmea_task_pvParameters_t;


/*******************************************************************************
 * NMEA Port Tasks creator (and Tx data queues creator)
 */
void create_nmea_port_tasks() {
    nmea_task_pvParameters_t* nmea_task_pvParameters;
    
    /* Initialize the Tx data queues */
    for (int i = 0; i < NMEA_PORT_TASK_NR_NMEA_PORTS; i++) {
        nmea_tx_queues[i] = xQueueCreate(CONF_NMEA_MUX_NMEA_TX_QUEUE_SIZE, sizeof(uint8_t*));
    }
    
    /* NMEA Port  Rx and Tx tasks */
    nmea_task_pvParameters = (nmea_task_pvParameters_t*)pvPortMalloc(sizeof(nmea_task_pvParameters_t));
    nmea_task_pvParameters->port_nr = NMEA_PORT_TASK_UART_PORT_1;
    nmea_task_pvParameters->p_usart = CONF_UART_NMEA_PORT_1;
    nmea_task_pvParameters->rx_led  = LED_PORT_1_RX;
    nmea_task_pvParameters->tx_led  = LED_PORT_1_TX;
    nmea_task_pvParameters->err_led = LED_PORT_1_ERR;

    xTaskCreate(nmea_rx_task, (const char *const) "NMEA_1_RX", NMEA_PORT_TASK_STACK_SIZE, nmea_task_pvParameters, NMEA_PORT_TASK_PRIORITY, NULL);
    xTaskCreate(nmea_tx_task, (const char *const) "NMEA_1_TX", NMEA_PORT_TASK_STACK_SIZE, nmea_task_pvParameters, NMEA_PORT_TASK_PRIORITY, NULL);
    
    /* NMEA Port 2 Rx and Tx tasks */
    nmea_task_pvParameters = (nmea_task_pvParameters_t*)pvPortMalloc(sizeof(nmea_task_pvParameters_t));
    nmea_task_pvParameters->port_nr = NMEA_PORT_TASK_UART_PORT_2;
    nmea_task_pvParameters->p_usart = CONF_UART_NMEA_PORT_2;
    nmea_task_pvParameters->rx_led  = LED_PORT_2_RX;
    nmea_task_pvParameters->tx_led  = LED_PORT_2_TX;
    nmea_task_pvParameters->err_led = LED_PORT_2_ERR;

    xTaskCreate(nmea_rx_task, (const char *const) "NMEA_2_RX", NMEA_PORT_TASK_STACK_SIZE, nmea_task_pvParameters, NMEA_PORT_TASK_PRIORITY, NULL);
    xTaskCreate(nmea_tx_task, (const char *const) "NMEA_2_TX", NMEA_PORT_TASK_STACK_SIZE, nmea_task_pvParameters, NMEA_PORT_TASK_PRIORITY, NULL);
    
    /* NMEA Port 3 Rx and Tx tasks */
    nmea_task_pvParameters = (nmea_task_pvParameters_t*)pvPortMalloc(sizeof(nmea_task_pvParameters_t));
    nmea_task_pvParameters->port_nr = NMEA_PORT_TASK_UART_PORT_3;
    nmea_task_pvParameters->p_usart = CONF_UART_NMEA_PORT_3;
    nmea_task_pvParameters->rx_led  = LED_PORT_3_RX;
    nmea_task_pvParameters->tx_led  = LED_PORT_3_TX;
    nmea_task_pvParameters->err_led = LED_PORT_3_ERR;

    xTaskCreate(nmea_rx_task, (const char *const) "NMEA_3_RX", NMEA_PORT_TASK_STACK_SIZE, nmea_task_pvParameters, NMEA_PORT_TASK_PRIORITY, NULL);
    xTaskCreate(nmea_tx_task, (const char *const) "NMEA_3_TX", NMEA_PORT_TASK_STACK_SIZE, nmea_task_pvParameters, NMEA_PORT_TASK_PRIORITY, NULL);
    
    /* NMEA Port 4 Rx and Tx tasks */
    nmea_task_pvParameters = (nmea_task_pvParameters_t*)pvPortMalloc(sizeof(nmea_task_pvParameters_t));
    nmea_task_pvParameters->port_nr = NMEA_PORT_TASK_UART_PORT_4;
    nmea_task_pvParameters->p_usart = CONF_UART_NMEA_PORT_4;
    nmea_task_pvParameters->rx_led  = LED_PORT_4_RX;
    nmea_task_pvParameters->tx_led  = LED_PORT_4_TX;
    nmea_task_pvParameters->err_led = LED_PORT_4_ERR;

    xTaskCreate(nmea_rx_task, (const char *const) "NMEA_4_RX", NMEA_PORT_TASK_STACK_SIZE, nmea_task_pvParameters, NMEA_PORT_TASK_PRIORITY, NULL);
    xTaskCreate(nmea_tx_task, (const char *const) "NMEA_4_TX", NMEA_PORT_TASK_STACK_SIZE, nmea_task_pvParameters, NMEA_PORT_TASK_PRIORITY, NULL);
    
    /* NMEA Port 5 Rx and Tx tasks */
    nmea_task_pvParameters = (nmea_task_pvParameters_t*)pvPortMalloc(sizeof(nmea_task_pvParameters_t));
    nmea_task_pvParameters->port_nr = NMEA_PORT_TASK_UART_PORT_5;
    nmea_task_pvParameters->p_usart = CONF_UART_NMEA_PORT_5;
    nmea_task_pvParameters->rx_led  = LED_PORT_5_RX;
    nmea_task_pvParameters->tx_led  = LED_PORT_5_TX;
    nmea_task_pvParameters->err_led = LED_PORT_5_ERR;

    xTaskCreate(nmea_rx_task, (const char *const) "NMEA_5_RX", NMEA_PORT_TASK_STACK_SIZE, nmea_task_pvParameters, NMEA_PORT_TASK_PRIORITY, NULL);
    xTaskCreate(nmea_tx_task, (const char *const) "NMEA_5_TX", NMEA_PORT_TASK_STACK_SIZE, nmea_task_pvParameters, NMEA_PORT_TASK_PRIORITY, NULL);
}



/*******************************************************************************
 * NMEA Rx Task
 * Reads NMEA data from NMEA UART, and sends it to all enlisted ports
 */
static void nmea_rx_task(void *pvParameters) {
    nmea_task_pvParameters_t* param;

    uint8_t* rx_buffer;         // Rx Buffer start pointer
    uint8_t* tx_buffer;         // Temporary Tx Buffer pointer
    uint8_t port_mask;
    int buf_index;
    int max_rx_buffer_usage;    // The maximum usage of the Rx buffer

    /* Task parameters */
    param = (nmea_task_pvParameters_t*)pvParameters;

    /* Allocate NMEA receive buffer */
    rx_buffer = (uint8_t*)pvPortMalloc(CONF_NMEA_MUX_NMEA_UARTS_RX_BUFFER_SIZE);
    max_rx_buffer_usage = 0;

    for (;;) {
restart:
        /* Find start of NMEA sentence ('$') */
        rx_buffer[0] = 0x00;
        do {
            /* Get one character from NMEA UART, ignore timeout */
            freertos_usart_serial_read_packet(param->p_usart, rx_buffer, 1, portMAX_DELAY);
#if 0
            rx_len = freertos_usart_serial_read_packet(param->p_usart, rx_buffer, 1, 2000 / portTICK_PERIOD_MS);
            if (rx_len == 0) {
                led_blink(param->err_led, LED_SHORT_BLINK);
            }                
#endif
        } while (rx_buffer[0] != '$');

        // Flash the the Rx LED upon receiving the start of an NMEA message
//        led_blink(param->rx_led, LED_SHORT_FLASH);

        buf_index = 1;

        do {
            /* Get one character from NMEA UART at a time */
            if (freertos_usart_serial_read_packet(param->p_usart, &rx_buffer[buf_index], 1,  2000 / portTICK_PERIOD_MS) == 0) {
                // led_blink(param->err_led, LED_SHORT_BLINK);  // Timeout error
                goto restart;  // Timeout
            }
            // Check that the received character is a valid character
            if ((rx_buffer[buf_index] < 32 || rx_buffer[buf_index] > 126) && rx_buffer[buf_index] != 0x0A && rx_buffer[buf_index] != 0x0D) {
//                printf("###WARN: Port %d, illegal character received (%d)\n\r", param->port_nr, rx_buffer[buf_index]);
                goto restart;
            }

            // Flash the the Rx LED upon receiving a 'valid' NMEA type (starting with '$' and ending with ','
            if (buf_index == 6 && rx_buffer[buf_index] == ',') {
                led_blink(param->rx_led, LED_SHORT_FLASH);
            }
            
        } while (rx_buffer[buf_index++] != 0x0A && buf_index < CONF_NMEA_MUX_NMEA_UARTS_RX_BUFFER_SIZE-1);

        rx_buffer[buf_index++] = 0x00;  // Terminate string
        
        /* Check if the current NMEA message length is greater than the old 'record' */
        if (buf_index > max_rx_buffer_usage) {
            max_rx_buffer_usage = buf_index;
        }
        
        /* Check if Rx-buffer is full (it should never be) */
        if (buf_index > CONF_NMEA_MUX_NMEA_UARTS_RX_BUFFER_SIZE-1) {
            printf("###WARN: Port %d Rx-buffer overrun\r\n", param->port_nr);
            led_blink(param->err_led, LED_MEDIUM_FLASH);
            continue;
        }

        port_mask = nmea_get_port_mask(nmea_search_trees[param->port_nr], (char*)rx_buffer);

        /* If Port-mask is 0xFF (no previous detection of this NMEA sentence), then clear the port-mask */
        if (port_mask == 0xFF) {
            printf("First arrival of NMEA %d: %s", param->port_nr, rx_buffer);
            taskENTER_CRITICAL();
            nmea_tree_add(nmea_search_trees[param->port_nr], (char*)rx_buffer, 0x00);
            taskEXIT_CRITICAL();
            port_mask = 0x00;
            continue;  // No need to copy it to the Tx buffers since we know that no Tx Port is 'subscribing' to the NMEA sentence
        }

        // Copy to TX Buffers
        for (int i = 0; i < NMEA_PORT_TASK_NR_NMEA_PORTS; i++) {
            if (port_mask & 0x01) {
                tx_buffer = (uint8_t*)pvPortMalloc(buf_index * sizeof(uint8_t));
                strcpy((char*)tx_buffer, (char*)rx_buffer);
                if (!xQueueSend(nmea_tx_queues[i], &tx_buffer, 0)) {
                    printf("Failed to send to NMEA %d Tx queue\r\n", i);
                    vPortFree(tx_buffer);
                    // Turn on the Err LED
                    led_blink(LED_PORT_1_ERR + i, LED_LONG_FLASH);
                }
            }
            port_mask >>= 1;
        }
    }
}


/*******************************************************************************
 * NMEA Tx Task
 * Writes NMEA data to NMEA UART
 */
static void nmea_tx_task(void *pvParameters) {
    nmea_task_pvParameters_t* param;
    char* tx_buffer;         // Temporary Tx Buffer pointer

    /* Task parameters */
    param = (nmea_task_pvParameters_t*)pvParameters;

    for (;;) {
        if (xQueueReceive(nmea_tx_queues[param->port_nr], &tx_buffer, portMAX_DELAY) == pdTRUE) {
            led_blink(param->tx_led, LED_SHORT_FLASH);
            if (freertos_usart_write_packet(param->p_usart, tx_buffer, strlen(tx_buffer), portMAX_DELAY) != STATUS_OK) {
                printf("NMEA %d usart_write failed\r\n", param->port_nr);
                led_blink(param->err_led, LED_LONG_FLASH);
            }
            vPortFree(tx_buffer);
        }
    }
}

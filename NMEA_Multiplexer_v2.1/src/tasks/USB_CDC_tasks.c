/*
 * USB_CDC_tasks.c
 *
 * Created: 2017-08-09 16:04:32
 *  Author: Max
 */

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ INCLUDES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* Standard includes. */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "FreeRTOS_CLI.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

/* Atmel library includes. */
#include <udi_cdc.h>
#include <udc.h>

#include "services/CLI-commands.h"
#include "services/usb.h"
#include "tasks/nmea_port_tasks.h"
#include "tasks/USB_CDC_tasks.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DEFINES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* Dimensions the buffer into which input characters are placed */
#define MAX_CLI_INPUT_SIZE 40

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ LOCAL VARIABLES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

typedef struct _usb_task_pvParameters_t {
    int port_nr;
    SemaphoreHandle_t access_mutex;
} usb_task_pvParameters_t;

//~~~~~~~~~~~~~~~~~~~~~~~~ LOCAL FUNCTION DECLARATIONS ~~~~~~~~~~~~~~~~~~~~~~~~~

static void usb_cdc_cli_task(void *pvParameters);
static void usb_cdc_nmea_tx_task(void *pvParameters);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


/*******************************************************************************
 * USB CDC Task creator
 * Creates both the CLI and NMEA Channel USB Task
 */
void create_usb_cdc_tasks() {
    usb_task_pvParameters_t* usb_cdc_cli_task_pvParameters;
    usb_task_pvParameters_t* usb_cdc_nmea_task_pvParameters;

    usb_cdc_cli_task_pvParameters = (usb_task_pvParameters_t*)pvPortMalloc(sizeof(usb_task_pvParameters_t));
    usb_cdc_nmea_task_pvParameters = (usb_task_pvParameters_t*)pvPortMalloc(sizeof(usb_task_pvParameters_t));

    usb_cdc_cli_task_pvParameters->port_nr = USB_CLI_PORT;
    usb_cdc_cli_task_pvParameters->access_mutex = xSemaphoreCreateMutex();

    usb_cdc_nmea_task_pvParameters->port_nr = USB_NMEA_PORT;
    usb_cdc_nmea_task_pvParameters->access_mutex = usb_cdc_cli_task_pvParameters->access_mutex;
        
    /* Create the USB CLI and NMEA task. */
    xTaskCreate(usb_cdc_cli_task, (const char *const) "USB_CLI", CDC_CLI_TASK_STACK_SIZE, usb_cdc_cli_task_pvParameters, CDC_CLI_TASK_PRIORITY, NULL);
    xTaskCreate(usb_cdc_nmea_tx_task, (const char *const) "USB_DATA", CDC_NMEA_TASK_STACK_SIZE, usb_cdc_nmea_task_pvParameters, CDC_NMEA_TASK_PRIORITY, NULL);
}


/*******************************************************************************
 * USB CDC Command Console Task
 * Reads characters from USB CLI Port and processes the commands
 */
static void usb_cdc_cli_task(void *pvParameters) {
    uint8_t received_char, input_index = 0, *output_string;
    static int8_t input_string[MAX_CLI_INPUT_SIZE];
    portBASE_TYPE returned_value;
    int add_zero;
    usb_task_pvParameters_t* param;

    /* Task parameters */
    param = (usb_task_pvParameters_t*)pvParameters;

    /* Obtain the address of the output buffer */
    output_string = (uint8_t *) FreeRTOS_CLIGetOutputBuffer();

    for (;;) {
        /* Wait for new data. */
//        printf("usb_cdc_cli_task() Waiting for semaphore\r\n");
        xSemaphoreTake(cdc_new_rx_data_semaphores[USB_CLI_PORT], portMAX_DELAY);
//      printf("usb_cdc_cli_task() Got semaphore!\r\n");
        add_zero = 0;

        /* Ensure mutually exclusive access is obtained as other tasks can write
        to the CLI. */
        xSemaphoreTake(param->access_mutex, portMAX_DELAY);
        /* While there are input characters. */
        while (udi_cdc_multi_is_rx_ready(USB_CLI_PORT) == true) {
//            printf("udi_cdc_multi_is_rx_ready() == true\r\n");
            // If Data Terminal Ready is lost, abort
            if (cdc_cli_dtr == false) {
                input_index = 0;
                memset(input_string, 0x00, MAX_CLI_INPUT_SIZE);
//                printf("usb_cdc_command_console_task: DTR lost\r\n");
                break;
            }

            received_char = (uint8_t) udi_cdc_multi_getc(USB_CLI_PORT);

//            if (received_char == '\r' || received_char == '\n') {
            if (received_char == '\r') {
                 udi_cdc_multi_putc(USB_CLI_PORT, received_char);
                /* Pass the received command to the command interpreter.  The command interpreter is called repeatedly until it returns pdFALSE as
                it might generate more than one string. */
                if (input_index > 0) {
                    do {
                        /* Get the string to write to the UART from the command interpreter. */
                        returned_value = FreeRTOS_CLIProcessCommand((char *)input_string,(char *)output_string, configCOMMAND_INT_MAX_OUTPUT_SIZE);

                        if (returned_value == pdFALSE)
                            add_zero = 1;

                        /* Transmit the generated string. */
                        udi_cdc_multi_write_buf(USB_CLI_PORT, (void *) output_string, strlen((char *) output_string) + add_zero);    // Include the \0 at the end of the last string /Max
//                      printf("usb_cdc_command_console_task: %s", output_string);  // '\r\n' already included in string
                    } while (returned_value != pdFALSE);
                }
//                printf("usb_cnc_command_console_task recived_char 0x%.2X\r\n", received_char);
                udi_cdc_multi_write_buf(USB_CLI_PORT, (void *) "=> ", 3);
                /* All the strings generated by the input command have been sent.
                Clear the input string ready to receive the next command. */
                input_index = 0;
                memset(input_string, 0x00, MAX_CLI_INPUT_SIZE);
            } else {
                if (received_char == '\b' || received_char == 0x7F) {
                    /* Backspace was pressed.  Erase the last character in the string - if any. */
                    if (input_index > 0) {
                        input_index--;
                        input_string[input_index] = '\0';
                        /* Echo the character. */
                        udi_cdc_multi_putc(USB_CLI_PORT, received_char);                    }
                } else if (received_char != '\r' && received_char != '\n') {
                    /* A character was entered.  Add it to the string
                    entered so far.  When a \r is entered the complete
                    string will be passed to the command interpreter. */
                    if (input_index < MAX_CLI_INPUT_SIZE) {
                        input_string[input_index] = received_char;
                        input_index++;
                        /* Echo the character. */
                        udi_cdc_multi_putc(USB_CLI_PORT, received_char);
                    }
                } else {
                    udi_cdc_multi_putc(USB_CLI_PORT, received_char);
                }
            }
        }

        /* Finished with the CDC port, return the mutex until more characters arrive. */
        xSemaphoreGive(param->access_mutex);
    }
}


/*******************************************************************************
 * USB CDC NMEA Data Transmit task
 * Sends all NMEA data that is in the 'nmea_channel_queue' to the USB NMEA Data Channel
 */
static void usb_cdc_nmea_tx_task(void *pvParameters) {
    char* tx_buffer;
    usb_task_pvParameters_t* param;
    
    /* Task parameters */
    param = (usb_task_pvParameters_t*)pvParameters;

    /* Loop forever */
    for (;;) {
        /* Wait for new data. */
        if (xQueueReceive(nmea_tx_queues[NMEA_PORT_TASK_USB_PORT], &tx_buffer, portMAX_DELAY) == pdPASS) {
//          printf("Data_packet received: %s", &(packet_ptr->data);

            /* Get hold of the USB port, then send packet */
            xSemaphoreTake(param->access_mutex, portMAX_DELAY);

            // If Data Terminal Ready is lost, don't send string
            if (cdc_cli_dtr == true && !disable_usb_nmea) {
              udi_cdc_multi_write_buf(USB_NMEA_PORT, tx_buffer, strlen(tx_buffer));
//                udi_cdc_multi_write_buf(USB_NMEA_PORT, "Max", 3);
            } else {
                printf("usb_cdc_data_channel_task: sending data packet to /dev/null\r\n");
            }
            xSemaphoreGive(param->access_mutex);

            /* Release buffer */
            vPortFree(tx_buffer);
        }
    }
}

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
#include "task.h"
#include "queue.h"
#include "semphr.h"

/* FreeRTOS+ includes. */
#include "FreeRTOS_CLI.h"

/* Atmel library includes. */
#include <udi_cdc.h>
#include <udc.h>

#include "tasks/USB_CDC_tasks.h"
#include "tasks/task_queues.h"
#include "services/CLI-commands.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DEFINES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* USB CDC ports */
#define CLI_USB_PORT 0
#define NMEA_USB_PORT 1

/* Dimensions the buffer into which input characters are placed */
#define MAX_CLI_INPUT_SIZE 40

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ LOCAL VARIABLES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* Semaphore used to signal the arrival of new data */
static SemaphoreHandle_t cdc_new_cli_data_semaphore = NULL;
static SemaphoreHandle_t cdc_new_nmea_data_channel_semaphore = NULL;

/* Mutex used to get access to the CDC port for transmitting */
static SemaphoreHandle_t cli_access_mutex = NULL;
static SemaphoreHandle_t nmea_access_mutex = NULL;

/* Flag used to indicated whether or not the CDC port is connected or not */
static volatile bool cdc_connected = false;

static volatile bool cdc_cli_dtr = false;
static volatile bool cdc_nmea_dtr = false;
static volatile bool cdc_cli_rts = false;
static volatile bool cdc_nmea_rts = false;

bool usb_power = false;
bool usb_connected = false;;

//~~~~~~~~~~~~~~~~~~~~~~~~ LOCAL FUNCTION DECLARATIONS ~~~~~~~~~~~~~~~~~~~~~~~~~

static void usb_cdc_cli_task(void *pvParameters);
static void usb_cdc_nmea_channel_task(void *pvParameters);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


/*******************************************************************************
 * USB CDC Task creator
 * Creates both the CLI and NMEA Channel USB Task
 */
void create_usb_cdc_tasks(void) {
	/* Register the default CLI commands. */
	vRegisterCLICommands();

	/* Create the semaphore used to signal the arrival of new data on CLI USB port */
	vSemaphoreCreateBinary(cdc_new_cli_data_semaphore);
	configASSERT(cdc_new_cli_data_semaphore);

	/* Create the semaphore used to signal the arrival of new data on NMEA USB port */
	vSemaphoreCreateBinary(cdc_new_nmea_data_channel_semaphore);
	configASSERT(cdc_new_nmea_data_channel_semaphore);

	/* Create the semaphore used to access the CDC port as it is written to from
	more than one task. */
	cli_access_mutex = xSemaphoreCreateMutex();
	configASSERT(cli_access_mutex);
	nmea_access_mutex = xSemaphoreCreateMutex();
	configASSERT(nmea_access_mutex);

	/* Start USB */
	if (!udc_include_vbus_monitoring()) {
		/* If VBUS monitoring is not available on this product.  Assume VBUS is present. */
		UDC_VBUS_EVENT(true);
	}

	/* Start USB */
	udc_start();

	/* Create the USART CLI task. */
	xTaskCreate(	usb_cdc_cli_task,				/* The task that implements the Command Line Interface. */
					(const char *const) "USB_CLI",	/* Text name assigned to the task.  This is just to assist debugging.  The kernel does not use this name itself. */
					CDC_CLI_TASK_STACK_SIZE,		/* The size of the stack allocated to the task. */
					NULL,							/* The parameter is not used. */
					CDC_CLI_TASK_PRIORITY,			/* The priority allocated to the task. */
					NULL);							/* Used to store the handle to the created task - in this case the handle is not required. */
#if 0
	/* Create the USART CLI task. */
	xTaskCreate(	usb_cdc_nmea_channel_task,		/* The task that implements the NMEA Data Channel. */
					(const char *const) "USB_NMEA",	/* Text name assigned to the task.  This is just to assist debugging.  The kernel does not use this name itself. */
					CDC_NMEA_TASK_STACK_SIZE,		/* The size of the stack allocated to the task. */
					NULL,							/* The parameter is not used. */
					CDC_NMEA_TASK_PRIORITY,			/* The priority allocated to the task. */
					NULL);							/* Used to store the handle to the created task - in this case the handle is not required. */
#endif
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

	/* Just to remove compiler warnings. */
	(void) pvParameters;

	/* Obtain the address of the output buffer */
	output_string = (uint8_t *) FreeRTOS_CLIGetOutputBuffer();

	for (;;) {
		/* Wait for new data. */
		xSemaphoreTake(cdc_new_cli_data_semaphore, portMAX_DELAY);
		add_zero = 0;

		/* Ensure mutually exclusive access is obtained as other tasks can write
		to the CLI. */
		xSemaphoreTake(cli_access_mutex, portMAX_DELAY);
		/* While there are input characters. */
		while (udi_cdc_multi_is_rx_ready(CLI_USB_PORT) == true) {
			// If Data Terminal Ready is lost, abort
			if (cdc_cli_dtr == false) {
				input_index = 0;
				memset(input_string, 0x00, MAX_CLI_INPUT_SIZE);
				printf("usb_cdc_command_console_task: DTR lost\r\n");
				break;
			}

			received_char = (uint8_t) udi_cdc_multi_getc(CLI_USB_PORT);
			/* Echo the character. */
			udi_cdc_multi_putc(CLI_USB_PORT, received_char);

			if (received_char == '\n' || received_char == '\r') {
				/* Pass the received command to the command interpreter.  The
				command interpreter is called repeatedly until it returns pdFALSE as
				it might generate more than one string. */
				if (input_index > 0) {
					do {
						/* Get the string to write to the UART from the command
						interpreter. */
						returned_value = FreeRTOS_CLIProcessCommand(
								(char *)input_string,
								(char *)output_string,
								configCOMMAND_INT_MAX_OUTPUT_SIZE);

						if (returned_value == pdFALSE)
							add_zero = 1;

						/* Transmit the generated string. */
						udi_cdc_multi_write_buf(CLI_USB_PORT, (void *) output_string,
						 strlen((char *) output_string) + add_zero);	// Include the \0 at the end of the last string /Max
//						printf("usb_cdc_command_console_task: %s\r\n", output_string);
					} while (returned_value != pdFALSE);
				}
				/* All the strings generated by the input command have been sent.
				Clear the input	string ready to receive the next command. */
				input_index = 0;
				memset(input_string, 0x00, MAX_CLI_INPUT_SIZE);
			} else {
				if (received_char == '\b' || received_char == 0x7F) {
					/* Backspace was pressed.  Erase the last character in the string - if any. */
					if (input_index > 0) {
						input_index--;
						input_string[input_index] = '\0';
					}
				} else if (received_char != '\n' && received_char != '\r') {
					/* A character was entered.  Add it to the string
					entered so far.  When a \r is entered the complete
					string will be passed to the command interpreter. */
					if (input_index < MAX_CLI_INPUT_SIZE) {
						input_string[input_index] = received_char;
						input_index++;
					}
				}
			}
		}

		/* Finished with the CDC port, return the mutex until more characters arrive. */
		xSemaphoreGive(cli_access_mutex);
	}
}


/*******************************************************************************
 * USB CDC NMEA Data Channel task
 * Sends all NMEA data that is in the 'nmea_channel_queue' to the USB NMEA Data Channel
 */
static void usb_cdc_nmea_channel_task(void *pvParameters) {
	uint8_t* string_ptr;

	/* Just to remove compiler warnings. */
	(void) pvParameters;

	/* Loop forever */
	for (;;) {
		/* Wait for new data. */
		if (xQueueReceive(nmea_usb_rx_queue, &string_ptr, portMAX_DELAY) == pdPASS) {
//			printf("Data_packet received: %s", &(packet_ptr->data);

			/* Get hold of the USB port, then send packet */
			xSemaphoreTake(nmea_access_mutex, portMAX_DELAY);

			// If Data Terminal Ready is lost, don't send string
			if (cdc_cli_dtr == true) {
//				udi_cdc_multi_write_buf(NMEA_USB_PORT, string_ptr, packet_ptr->length);
				udi_cdc_multi_write_buf(NMEA_USB_PORT, "Max", 3);
			} else {
				printf("usb_cdc_data_channel_task: sending data packet to /dev/null\r\n");
			}
			xSemaphoreGive(nmea_access_mutex);

			/* Release buffer */
			vPortFree(string_ptr);
		}
	}
}


/*******************************************************************************
 * USB CDC CLI string output
 * Sends 'message_string' to the CLI output interface
 */
void cdc_cli_output(const uint8_t *message_string) {
	if (cdc_connected == true && cdc_cli_dtr == true) {
		/* This call is outside of the CLI task, so ensure mutually exclusive
		access is obtained. */
		xSemaphoreTake(cli_access_mutex, portMAX_DELAY);
		udi_cdc_multi_write_buf(CLI_USB_PORT, (void *) message_string, strlen((char *) message_string));
		printf("cdc_cli_output: %s\r\n", message_string);
		xSemaphoreGive(cli_access_mutex);
	}
}


/*******************************************************************************
 * USB VBUS Event callback function
 */
void usb_vbus_event_callback(bool b_vbus_high) {
	if (b_vbus_high == true) {
		printf("VBUS is high\r\n");
		usb_power = true;
		udc_attach();
	} else {
		printf("VBUS is low\r\n");
		usb_power = false;
		udc_detach();
	}
}

void usb_cdc_suspend_callback() {
	printf("usb_cdc_suspend_callback\r\n");
}

void usb_cdc_resume_callback() {
	printf("usb_cdc_resume_callback\r\n");
}

bool usb_cdc_set_dtr_callback(uint8_t port, bool set) {
	printf("usb_cdc_set_dtr_callback(%d, %d)\r\n", port, set);
	switch (port) {
		case 0:
			cdc_cli_dtr = set;
			break;
		case 1:
			cdc_nmea_dtr = set;
			break;
	}
	if (port == 0) {
		usb_connected = set;
	}
	return true;
}

bool usb_cdc_set_rts_callback(uint8_t port, bool set) {
	printf("usb_cdc_set_rts_callback(%d, %d)\r\n", port, set);
	switch (port) {
		case 0:
		cdc_cli_rts = set;
		break;
		case 1:
		cdc_nmea_rts = set;
		break;
	}
	return true;
}


/*******************************************************************************
 * USB CDC Received Notify callback function
 * Signals whenever there is new data received on any of the two USB interfaces
 */
bool usb_cdc_rx_notify_callback(uint8_t port) {
	portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	/* Check which USB port that received data */
	if (port == CLI_USB_PORT) {
//Max		configASSERT(cdc_new_cli_data_semaphore);
		xSemaphoreGiveFromISR(cdc_new_cli_data_semaphore, &xHigherPriorityTaskWoken);
	} else {
//Max		configASSERT(cdc_new_data_channel_semaphore);
		xSemaphoreGiveFromISR(cdc_new_nmea_data_channel_semaphore, &xHigherPriorityTaskWoken);
	}

	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
	return true;
}


/*******************************************************************************
 * USB CDC Enable callback function
 */
bool usb_cdc_enable_callback(uint8_t port) {
	(void) port;
	printf("USB CDC enabled for port %d\r\n", port);
	cdc_connected = true;
	return true;
}


/*******************************************************************************
 * USB CDC Disable callback function
 */
bool usb_cdc_disable_callback(uint8_t port) {
	(void) port;
	printf("USB CDC disabled for port %d\r\n", port);
	cdc_connected = false;
	return true;
}
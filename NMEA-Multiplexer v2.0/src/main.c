/**
 * \file
 *
 * \brief NMEA Multiplexer v2.0 application
 *
 */

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ INCLUDES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <asf.h>
#include "services/led.h"
#include "config/conf_nmea_mux.h"
#include "application_hooks.h"

/* FreeRTOS */
#include "FreeRTOS.h"
#include "freertos_peripheral_control.h"
#include "freertos_usart_serial.h"
#include "task.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DEFINES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* The priorities at which various tasks will get created. */
//#define SOFTWARE_TIMER_RATE		(200 / portTICK_PERIOD_MS)

#define DBG_WELCOME_HEADER \
"------------------------------------\r\n" \
"-- NMEA_Multiplexer_RTOS 2017 " SW_VERSION "      --\r\n" \
"-- Compiled: "__DATE__" "__TIME__" --\r\n" \
"------------------------------------\r\n" \
"--  ## DEBUG OUTPUT INTERFACE ##  --\r\n" \
"------------------------------------\r\n"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


int main (void)
{
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Initialize PLL and clocks
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	sysclk_init();

	/* Ensure all priority bits are assigned as preemption priority bits. */
	NVIC_SetPriorityGrouping(0);
	
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Initialize the board
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	board_init();

#if CONF_NMEA_MUX_LED_TEST_DELAY_MS > 0
	LED_Test(CONF_NMEA_MUX_LED_TEST_DELAY_MS);
#endif

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Initialize interfaces
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifdef CONF_NMEA_MUX_ENABLE_DEBUG_CONSOLE
	/* Initialize Debug Console */
	const usart_serial_options_t console_uart_options = {
		.baudrate     = CONF_UART_CONSOLE_BAUDRATE,
		.paritytype   = CONF_UART_CONSOLE_PARITY
	};

	stdio_serial_init(CONF_UART_CONSOLE, &console_uart_options);
	printf(DBG_WELCOME_HEADER);			// Print welcome message on Debug UART
#endif

#ifdef CONF_NMEA_MUX_ENABLE_FREERTOS_NMEA_USART
	/* Initialize FreeRTOS USART drivers */
	uint8_t *nmea_rx_buffer;
	configASSERT(nmea_rx_buffer = (uint8_t*)pvPortMalloc(CONF_NMEA_MUX_NMEA_RX_BUFFER_SIZE));

	freertos_peripheral_options_t nmea_periph_opt = {
		.receive_buffer = nmea_rx_buffer,
		.receive_buffer_size = CONF_NMEA_MUX_NMEA_RX_BUFFER_SIZE,
		.interrupt_priority = configLIBRARY_LOWEST_INTERRUPT_PRIORITY - 1,
		.operation_mode = USART_RS232,
		.options_flags = WAIT_RX_COMPLETE | WAIT_TX_COMPLETE
	};

	sam_usart_opt_t nmea_usart_opt = {
		.baudrate = CONF_UART_NMEA_PORT_BAUDRATE,
		.char_length = CONF_UART_NMEA_PORT_CHAR_LENGTH,
		.parity_type = CONF_UART_NMEA_PORT_PARITY,
		.stop_bits = CONF_UART_NMEA_PORT_STOP_BITS,
		.channel_mode = US_MR_CHMODE_NORMAL,
		.irda_filter = 0
	};

	configASSERT(freertos_usart_serial_init(CONF_UART_NMEA_PORT_1, &nmea_usart_opt, &nmea_periph_opt));
#endif

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Create tasks
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	
	/* Create Task Queues */
//	create_task_queues();

#ifdef CONF_NMEA_ENABLE_PORT_1_TASKS
	/* Create NMEA Port 1 tasks */
	create_nmea_port_1_tasks();
#endif


#ifdef CONF_NMEA_MUX_ENABLE_TIMER_TASK
	/* Create Timer task */
	create_timer_task(SOFTWARE_TIMER_RATE);
#endif

#ifdef CONF_NMEA_MUX_ENABLE_USB_CDC_TASK
	/* Create USB CDC tasks */
	create_usb_cdc_tasks();
#endif

	printf("Starting all RTOS tasks\n");
//	vTaskStartScheduler();	// This function call should never return
	
	printf("###ERROR: vTaskStartScheduler() failed\n");
	for( ;; );
}

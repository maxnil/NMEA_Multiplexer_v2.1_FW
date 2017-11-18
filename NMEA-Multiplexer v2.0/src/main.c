/**
 * \file
 *
 * \brief NMEA Multiplexer v2.0 application
 *
 */

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ INCLUDES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <asf.h>
#include <string.h>
#include "services/led.h"
#include "services/nmea_func.h"
#include "config/conf_nmea_mux.h"
#include "application_hooks.h"

/* FreeRTOS */
#include "FreeRTOS.h"
#include "freertos_peripheral_control.h"
#include "freertos_usart_serial.h"
#include "task.h"

/* Tasks */
#include "tasks/nmea_port_tasks.h"
#include "tasks/bt_task.h"
#include "tasks/task_queues.h"
#include "tasks/timer_task.h"
#include "tasks/USB_CDC_tasks.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DEFINES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define SOFTWARE_TIMER_RATE		(200 / portTICK_PERIOD_MS)

#define DBG_WELCOME_HEADER \
"\r\n" \
"\r\n" \
"------------------------------------\r\n" \
"-- NMEA Multiplexer v2.0          --\r\n" \
"-- RTOS FW Ver: " SW_VERSION "            --\r\n" \
"-- Compiled: "__DATE__" "__TIME__" --\r\n" \
"------------------------------------\r\n" \
"--  ## DEBUG OUTPUT INTERFACE ##  --\r\n" \
"------------------------------------\r\n"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


int main (void)
{
	int i;

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
		.baudrate	= CONF_UART_CONSOLE_BAUDRATE,
		.paritytype	= CONF_UART_CONSOLE_PARITY,
		.charlength	= CONF_UART_CONSOLE_CHAR_LENGTH,
		.stopbits	= CONF_UART_CONSOLE_STOP_BITS
	};

	stdio_serial_init(CONF_UART_CONSOLE, &console_uart_options);
	printf(DBG_WELCOME_HEADER);			// Print welcome message on Debug UART
#endif


#ifdef CONF_NMEA_MUX_ENABLE_FREERTOS_NMEA_USART
	// Same settings for all NMEA ports
	sam_usart_opt_t nmea_usart_opt = {
		.baudrate		  = 4800,
		.char_length	= CONF_UART_NMEA_PORT_CHAR_LENGTH,
		.parity_type	= CONF_UART_NMEA_PORT_PARITY,
		.stop_bits		= CONF_UART_NMEA_PORT_STOP_BITS,
		.channel_mode	= US_MR_CHMODE_NORMAL,  //  US_MR_USART_MODE_HW_HANDSHAKING
		.irda_filter	= 0
	};

	// Peripheral settings
	freertos_peripheral_options_t nmea_periph_opt = {
		.receive_buffer			 = NULL,  // Will be set later
		.receive_buffer_size = CONF_NMEA_MUX_NMEA_RX_BUFFER_SIZE,
		.interrupt_priority	 = configLIBRARY_LOWEST_INTERRUPT_PRIORITY - 1,
		.operation_mode		   = USART_RS232,
		.options_flags			 = WAIT_RX_COMPLETE | WAIT_TX_COMPLETE
	};

	// NMEA Port #1
	configASSERT(nmea_periph_opt.receive_buffer = (uint8_t*)pvPortMalloc(CONF_NMEA_MUX_NMEA_RX_BUFFER_SIZE));
	configASSERT(freertos_usart_serial_init(CONF_UART_NMEA_PORT_1, &nmea_usart_opt, &nmea_periph_opt));

	// NMEA Port #2
	configASSERT(nmea_periph_opt.receive_buffer = (uint8_t*)pvPortMalloc(CONF_NMEA_MUX_NMEA_RX_BUFFER_SIZE));
	configASSERT(freertos_usart_serial_init(CONF_UART_NMEA_PORT_2, &nmea_usart_opt, &nmea_periph_opt));

	// NMEA Port #3
	configASSERT(nmea_periph_opt.receive_buffer = (uint8_t*)pvPortMalloc(CONF_NMEA_MUX_NMEA_RX_BUFFER_SIZE));
	configASSERT(freertos_usart_serial_init(CONF_UART_NMEA_PORT_3, &nmea_usart_opt, &nmea_periph_opt));

	// NMEA Port #4
	configASSERT(nmea_periph_opt.receive_buffer = (uint8_t*)pvPortMalloc(CONF_NMEA_MUX_NMEA_RX_BUFFER_SIZE));
	configASSERT(freertos_usart_serial_init(CONF_UART_NMEA_PORT_4, &nmea_usart_opt, &nmea_periph_opt));

	// NMEA Port #5
	configASSERT(nmea_periph_opt.receive_buffer = (uint8_t*)pvPortMalloc(CONF_NMEA_MUX_NMEA_RX_BUFFER_SIZE));
	configASSERT(freertos_usart_serial_init(CONF_UART_NMEA_PORT_5, &nmea_usart_opt, &nmea_periph_opt));
#endif


#ifdef CONF_NMEA_MUX_ENABLE_FREERTOS_BT_USART
	// BlueTooth port settings
	sam_usart_opt_t bt_usart_opt = {
		.baudrate		= CONF_UART_BT_BAUDRATE,
		.char_length	= CONF_UART_BT_CHAR_LENGTH,
		.parity_type	= CONF_UART_BT_PARITY,
		.stop_bits		= CONF_UART_BT_STOP_BITS,
		.channel_mode	= US_MR_CHMODE_NORMAL,  //  US_MR_USART_MODE_HW_HANDSHAKING
		.irda_filter	= 0
	};

	// BlueTooth port settings
	freertos_peripheral_options_t bt_periph_opt = {
		.receive_buffer			= NULL,			// For now...
		.receive_buffer_size	= CONF_NMEA_MUX_BT_RX_BUFFER_SIZE,
		.interrupt_priority		= configLIBRARY_LOWEST_INTERRUPT_PRIORITY - 1,
		.operation_mode			= USART_RS232,
		.options_flags			= WAIT_RX_COMPLETE | WAIT_TX_COMPLETE
	};

	// BlueTooth UART
	configASSERT(bt_periph_opt.receive_buffer = (uint8_t*)pvPortMalloc(CONF_NMEA_MUX_BT_RX_BUFFER_SIZE));
	configASSERT(freertos_usart_serial_init(CONF_UART_BT, &bt_usart_opt, &bt_periph_opt));
#endif


	for (i = 0; i < 5; i++) {
		nmea_search_tree[i] = nmea_tree_init();
		nmea_tree_add(nmea_search_tree[i], "$GPGSV", 0x03);
		nmea_tree_add(nmea_search_tree[i], "$GPGLL", 0x06);
		nmea_tree_add(nmea_search_tree[i], "$NISSE", 0x0F);
		nmea_tree_add(nmea_search_tree[i], "$OLLE", 0x1F);
	}

	char* nmea_str;
	nmea_str = (char*)pvPortMalloc(1024);
	nmea_tree_get_string(nmea_search_tree[0], nmea_str);

	printf("NMEA TREE:%s\n\r", nmea_str);

	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Play with Flash
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#if 0
	uint32_t* flash_ptr;

	flash_ptr = DISK_ADDR;
	for (i = 0; i < 10; i++) {
		printf("Flash %.8p = 0x%.8X\n\r", flash_ptr, *flash_ptr);
		flash_ptr++;
	}

//	flash_init(FLASH_ACCESS_MODE_128, 5);  // 5 Wait-states

	flash_ptr = DISK_ADDR;
//	printf("flash_erase %x\n\r", flash__write(flash_ptr, nmea_str, strlen(nmea_str), 0));

	printf("flash_write (len = %d) result = %x\n\r", strlen(nmea_str), flash_write(flash_ptr, nmea_str, strlen(nmea_str), 0));

	flash_ptr = DISK_ADDR;
	for (i = 0; i < 10; i++) {
		printf("Flash %.8p = 0x%.8X\n\r", flash_ptr, *flash_ptr);
		flash_ptr++;
	}

#if 0
	flash_ptr = DISK_ADDR;
	printf("flash_erase_page %p %d\n\r", flash_ptr, flash_erase_page(DISK_ADDR, IFLASH_ERASE_PAGES_16));

	for (i = 0; i < 10; i++) {
		printf("Flash %.8p = 0x%.8X\n\r", flash_ptr, *flash_ptr);
		flash_ptr++;
	}
#endif
#endif
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// Create tasks
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	/* Create Task Queues */
	create_task_queues();


#ifdef CONF_NMEA_MUX_ENABLE_NMEA_PORT_TASKS
	/* Create NMEA Port 1-5 tasks */
	create_nmea_port_tasks();
#endif


#ifdef CONF_NMEA_MUX_ENABLE_BT_TASK
	/* Create BT tasks */
	create_bt_task();
#endif


#ifdef CONF_NMEA_MUX_ENABLE_TIMER_TASK
	/* Create Timer task */
	create_timer_task(SOFTWARE_TIMER_RATE);
#endif


#ifdef CONF_NMEA_MUX_ENABLE_USB_CDC_TASK
	/* Create USB CDC tasks */
	create_usb_cdc_tasks();
#endif

	printf("Starting all RTOS tasks\r\n");
	vTaskStartScheduler();	// This function call should never return

	printf("###ERROR: vTaskStartScheduler() failed\r\n");
	while(1) {
		LED_Toggle(PORT_1_ERR_LED_PIN);
		LED_Toggle(PORT_2_ERR_LED_PIN);
		LED_Toggle(PORT_3_ERR_LED_PIN);
		LED_Toggle(PORT_4_ERR_LED_PIN);
		LED_Toggle(PORT_5_ERR_LED_PIN);
		delay_ms(250);
	}

	for( ;; );
}

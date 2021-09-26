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
#include "tasks/led_timer_task.h"
#include "tasks/USB_CDC_tasks.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DEFINES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define DBG_WELCOME_HEADER \
"\r\n" \
"\r\n" \
"------------------------------------\r\n" \
"-- NMEA Multiplexer v2.1          --\r\n" \
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

#ifdef CONF_NMEA_MUX_DEBUG_UART_ENABLE
	/* Initialize Debug Console */
	const usart_serial_options_t console_uart_options = {
		.baudrate	  = CONF_UART_CONSOLE_BAUDRATE,
		.paritytype	= CONF_UART_CONSOLE_PARITY,
		.charlength	= CONF_UART_CONSOLE_CHAR_LENGTH,
		.stopbits	  = CONF_UART_CONSOLE_STOP_BITS
	};

	stdio_serial_init(CONF_UART_CONSOLE, &console_uart_options);
	printf(DBG_WELCOME_HEADER);			// Print welcome message on Debug UART
#endif


#ifdef CONF_NMEA_MUX_NMEA_UARTS_ENABLE
	// Same settings for all NMEA ports
	sam_usart_opt_t nmea_usart_opt = {
		.baudrate		= CONF_NMEA_MUX_NMEA_UARTS_BAUDRATE,
		.char_length	= CONF_UART_NMEA_PORT_CHAR_LENGTH,
		.parity_type	= CONF_UART_NMEA_PORT_PARITY,
		.stop_bits		= CONF_UART_NMEA_PORT_STOP_BITS,
		.channel_mode	= US_MR_CHMODE_NORMAL,  //  US_MR_USART_MODE_HW_HANDSHAKING
		.irda_filter	= 0
	};

	// Peripheral settings
	freertos_peripheral_options_t nmea_periph_opt = {
		.receive_buffer		 = NULL,  // Will be set later
		.receive_buffer_size = CONF_NMEA_MUX_NMEA_UARTS_RX_BUFFER_SIZE,
		.interrupt_priority	 = configLIBRARY_LOWEST_INTERRUPT_PRIORITY - 1,
		.operation_mode		 = USART_RS232,
		.options_flags	     = WAIT_RX_COMPLETE | WAIT_TX_COMPLETE
	};

	// NMEA Port #1
	configASSERT(nmea_periph_opt.receive_buffer = (uint8_t*)pvPortMalloc(CONF_NMEA_MUX_NMEA_UARTS_RX_BUFFER_SIZE));
	configASSERT(freertos_usart_serial_init(CONF_UART_NMEA_PORT_1, &nmea_usart_opt, &nmea_periph_opt));

	// NMEA Port #2
	configASSERT(nmea_periph_opt.receive_buffer = (uint8_t*)pvPortMalloc(CONF_NMEA_MUX_NMEA_UARTS_RX_BUFFER_SIZE));
	configASSERT(freertos_usart_serial_init(CONF_UART_NMEA_PORT_2, &nmea_usart_opt, &nmea_periph_opt));

	// NMEA Port #3
	configASSERT(nmea_periph_opt.receive_buffer = (uint8_t*)pvPortMalloc(CONF_NMEA_MUX_NMEA_UARTS_RX_BUFFER_SIZE));
	configASSERT(freertos_usart_serial_init(CONF_UART_NMEA_PORT_3, &nmea_usart_opt, &nmea_periph_opt));
    
    // NMEA Port #4
	nmea_usart_opt.baudrate		= 9600;
	configASSERT(nmea_periph_opt.receive_buffer = (uint8_t*)pvPortMalloc(CONF_NMEA_MUX_NMEA_UARTS_RX_BUFFER_SIZE));
	configASSERT(freertos_usart_serial_init(CONF_UART_NMEA_PORT_4, &nmea_usart_opt, &nmea_periph_opt));
	
	// NMEA Port #5
	nmea_usart_opt.baudrate		= 9600;
	configASSERT(nmea_periph_opt.receive_buffer = (uint8_t*)pvPortMalloc(CONF_NMEA_MUX_NMEA_UARTS_RX_BUFFER_SIZE));
	configASSERT(freertos_usart_serial_init(CONF_UART_NMEA_PORT_5, &nmea_usart_opt, &nmea_periph_opt));
#endif

#ifdef CONF_NMEA_MUX_BT_UART_ENABLE
	// BlueTooth port settings
	sam_usart_opt_t bt_usart_opt = {
		.baudrate		  = CONF_UART_BT_BAUDRATE,
		.char_length	= CONF_UART_BT_CHAR_LENGTH,
		.parity_type	= CONF_UART_BT_PARITY,
		.stop_bits		= CONF_UART_BT_STOP_BITS,
		.channel_mode	= US_MR_CHMODE_NORMAL,  //  US_MR_USART_MODE_HW_HANDSHAKING
		.irda_filter	= 0
	};

	// BlueTooth port settings
	freertos_peripheral_options_t bt_periph_opt = {
		.receive_buffer			 = NULL,			// For now...
		.receive_buffer_size = CONF_NMEA_MUX_BT_UART_RX_BUFFER_SIZE,
		.interrupt_priority	 = configLIBRARY_LOWEST_INTERRUPT_PRIORITY - 1,
		.operation_mode			 = USART_RS232,
		.options_flags			 = WAIT_RX_COMPLETE | WAIT_TX_COMPLETE
	};

	// BlueTooth UART
	configASSERT(bt_periph_opt.receive_buffer = (uint8_t*)pvPortMalloc(CONF_NMEA_MUX_BT_UART_RX_BUFFER_SIZE));
	configASSERT(freertos_usart_serial_init(CONF_UART_BT, &bt_usart_opt, &bt_periph_opt));
#endif

	for (i = 0; i < 5; i++) {
		nmea_search_tree[i] = nmea_tree_init("$KALLE=12$PELLE=23");
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

	uint32_t* storage_ptr;
    storage_ptr = STORAGE_ADDR;
    printf("Storage addr: %p\n\r", storage_ptr);
        
	printf("flash_init: %d\n\r", flash_init(FLASH_ACCESS_MODE_128, 6));  // 6 Wait-states
    printf("Unlocking page 0x%08X: %d\n\r", STORAGE_ADDR, flash_unlock(STORAGE_ADDR, STORAGE_ADDR + IFLASH_PAGE_SIZE - 1, 0, 0));

    printf("Dumping FLASH Storage after restart:\n\r");
	for (i = 0; i < 8; i++) {
		printf("Flash %p = 0x%.8X\n\r", storage_ptr, *storage_ptr);
		storage_ptr++;
	}
    
    storage_ptr = STORAGE_ADDR;
    int buf[8];
    
    buf[0] = 0xAAAA5555;
    buf[1] = 0x12345678;
    buf[2] = 0xF0F0F0F0;
    buf[4] = 0x11111111;
    buf[5] = 0x22222222;
    buf[6] = 0x33333333;
    buf[7] = 0x44444444;
    
    printf("Erase %p: %d\n\r", STORAGE_ADDR, flash_erase_page(STORAGE_ADDR, IFLASH_ERASE_PAGES_16));
    printf("flash_write (size = %d) result = %x\n\r", sizeof(buf), flash_write(STORAGE_ADDR, buf, sizeof(buf), 0));
    
    printf("Dumping FLASH Storage after first write:\n\r");
    for (i = 0; i < 8; i++) {
        printf("Flash %p = 0x%.8X\n\r", storage_ptr, *storage_ptr);
        storage_ptr++;
    }
        
    storage_ptr = STORAGE_ADDR;
    
    uint8_t nmea_tree_buf[100];
    memset(nmea_tree_buf, 0, sizeof(nmea_tree_buf) / 4);
        
    printf("Erase %p: %d\n\r", STORAGE_ADDR, flash_erase_page(STORAGE_ADDR, IFLASH_ERASE_PAGES_16));
    printf("flash_write (size = %d) result = %x\n\r", sizeof(buf), flash_write(STORAGE_ADDR, nmea_tree_buf, sizeof(nmea_tree_buf), 0));

    printf("Dumping FLASH Storage after second write:\n\r");
    for (i = 0; i < 8; i++) {
        printf("Flash %p = 0x%.8X\n\r", storage_ptr, *storage_ptr);
        storage_ptr++;
    }
#if 0
    



	flash_ptr = DISK_ADDR;
//	printf("flash_erase %x\n\r", flash__write(flash_ptr, nmea_str, strlen(nmea_str), 0));



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

#ifdef CONF_NMEA_MUX_NMEA_PORT_TASKS_ENABLE
	/* Create NMEA Port 1-5 tasks */
	create_nmea_port_tasks();
#endif


#ifdef CONF_NMEA_MUX_BT_TASK_ENABLE
	/* Create BT tasks */
	create_bt_task();
#endif


#ifdef CONF_NMEA_MUX_TIMER_TASK_ENABLE
	/* Create Timer task */
	create_led_timer_task();
#endif


#ifdef CONF_NMEA_MUX_USB_CDC_TASK_ENABLE
	/* Create USB CDC tasks */
	create_usb_cdc_tasks();
#endif


	printf("Starting all RTOS tasks\r\n");
	vTaskStartScheduler();	// This function call should never return

	printf("###ERROR: vTaskStartScheduler() failed\r\n");
	while(1) {
		LED_Toggle(LED_PORT_1_ERR_PIN);
		LED_Toggle(LED_PORT_2_ERR_PIN);
		LED_Toggle(LED_PORT_3_ERR_PIN);
		LED_Toggle(LED_PORT_4_ERR_PIN);
		LED_Toggle(LED_PORT_5_ERR_PIN);
		delay_ms(250);
	}
	for( ;; );
}

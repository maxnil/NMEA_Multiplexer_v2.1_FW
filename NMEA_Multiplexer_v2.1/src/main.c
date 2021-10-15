/**
 * \file
 *
 * \brief NMEA Multiplexer v2.0 application
 *
 */

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ INCLUDES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <asf.h>
#include <string.h>

/* FreeRTOS */
#include "FreeRTOS.h"
#include "freertos_peripheral_control.h"
#include "freertos_usart_serial.h"
#include "task.h"

/* Configurations */
#include "config/conf_nmea_mux.h"

/* Services */
#include "services/CLI-commands.h"
#include "services/led.h"
#include "services/nmea_func.h"
#include "services/storage.h"
#include "services/usart_setting.h"
#include "services/usb.h"
#include "application_hooks.h"

/* Tasks */
#include "tasks/bt_task.h"
#include "tasks/led_timer_task.h"
#include "tasks/misc_task.h"
#include "tasks/nmea_port_tasks.h"
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
        .baudrate     = CONF_UART_CONSOLE_BAUDRATE,
        .paritytype = CONF_UART_CONSOLE_PARITY,
        .charlength = CONF_UART_CONSOLE_CHAR_LENGTH,
        .stopbits     = CONF_UART_CONSOLE_STOP_BITS
    };

    stdio_serial_init(CONF_UART_CONSOLE, &console_uart_options);
    printf(DBG_WELCOME_HEADER);         // Print welcome message on Debug UART
#endif

#ifdef CONF_NMEA_MUX_NMEA_UARTS_ENABLE
    char* br_str_ptr;
    br_str_ptr = pvPortMalloc(FILE_MAX_SIZE);
    
    printf("Read NMEA USART Port settings from flash\n\r");
    storage_read_file(7, br_str_ptr);
    update_usart_baudrate_from_str(br_str_ptr);
        
    for (int i = 0; i < 5; i++) {
        printf("usart_baudrate[%d] = %d\n\r", i, usart_baudrate[i]);
    }

    vPortFree(br_str_ptr);    

    for (int i = 0; i < CONF_NMEA_MUX_NR_NMEA_UART_PORTS; i++) {
        init_nmea_port_usart(p_usart_list[i], usart_baudrate[i]);
    }        
#endif

#ifdef CONF_NMEA_MUX_BT_UART_ENABLE
    // BlueTooth port settings
    sam_usart_opt_t bt_usart_opt = {
        .baudrate       = CONF_UART_BT_BAUDRATE,
        .char_length    = CONF_UART_BT_CHAR_LENGTH,
        .parity_type    = CONF_UART_BT_PARITY,
        .stop_bits      = CONF_UART_BT_STOP_BITS,
        .channel_mode   = US_MR_CHMODE_NORMAL,  //  US_MR_USART_MODE_HW_HANDSHAKING
        .irda_filter    = 0
    };

    // BlueTooth port settings
    freertos_peripheral_options_t bt_periph_opt = {
        .receive_buffer          = NULL,            // For now...
        .receive_buffer_size = CONF_NMEA_MUX_BT_UART_RX_BUFFER_SIZE,
        .interrupt_priority  = configLIBRARY_LOWEST_INTERRUPT_PRIORITY - 1,
        .operation_mode          = USART_RS232,
        .options_flags           = WAIT_RX_COMPLETE | WAIT_TX_COMPLETE
    };

    // BlueTooth UART
    configASSERT(bt_periph_opt.receive_buffer = (uint8_t*)pvPortMalloc(CONF_NMEA_MUX_BT_UART_RX_BUFFER_SIZE));
    configASSERT(freertos_usart_serial_init(CONF_UART_BT, &bt_usart_opt, &bt_periph_opt));
#endif

    char* nmea_str;
    nmea_str = (char*)pvPortMalloc(FILE_MAX_SIZE);

    printf("Reading NMEA configuration from NVRAM\r\n");
    for (int i = 0; i < NMEA_PORT_TASK_NR_NMEA_PORTS; i++) {
        if (storage_read_file(i, nmea_str) > 0) {
            printf("Reading tree %d: %s\r\n", i, nmea_str);
            nmea_search_trees[i] = nmea_tree_init(nmea_str);
        } else {
            nmea_search_trees[i] = nmea_tree_init("\0");
        }
    }        
    vPortFree(nmea_str);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Create tasks
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

#ifdef CONF_NMEA_MUX_MISC_TASK_ENABLE
    /* Create Misc task */
    create_misc_task();
#endif


#ifdef CONF_NMEA_MUX_USB_CDC_TASK_ENABLE
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    // Initialize USB
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    // For Flash erase and write commands
    storage_init();
    
    /* Register the default CLI commands. */
    vRegisterCLICommands();

    usb_init();
    
    /* Create USB CDC tasks */
    create_usb_cdc_tasks();
#endif


    printf("Starting all RTOS tasks\r\n");
    vTaskStartScheduler();  // This function call should never return

    printf("###ERROR: vTaskStartScheduler() failed\r\n");
    while(1) {
        LED_Toggle(LED_PORT_1_ERR_PIN);
        LED_Toggle(LED_PORT_2_ERR_PIN);
        LED_Toggle(LED_PORT_3_ERR_PIN);
        LED_Toggle(LED_PORT_4_ERR_PIN);
        LED_Toggle(LED_PORT_5_ERR_PIN);
        delay_ms(250);
    }
    for(;;);
}

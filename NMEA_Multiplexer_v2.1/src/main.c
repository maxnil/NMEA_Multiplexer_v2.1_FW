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
        .baudrate   = CONF_UART_CONSOLE_BAUDRATE,
        .paritytype = CONF_UART_CONSOLE_PARITY,
        .charlength = CONF_UART_CONSOLE_CHAR_LENGTH,
        .stopbits   = CONF_UART_CONSOLE_STOP_BITS
    };

    stdio_serial_init(CONF_UART_CONSOLE, &console_uart_options);
    printf(DBG_WELCOME_HEADER);         // Print welcome message on Debug UART
    
    uint32_t reset_cause;
    reset_cause = rstc_get_reset_cause(RSTC);
    printf("### INFO: Reset cause: ");
    switch (reset_cause) {
        case RSTC_GENERAL_RESET:
            printf("GENERAL_RESET\n\r");
            break;
        case RSTC_BACKUP_RESET:
            printf("BACKUP_RESET\n\r");
            break;
        case RSTC_WATCHDOG_RESET:
            printf("WATCHDOG_RESET\n\r");
            break;
        case RSTC_SOFTWARE_RESET:
            printf("SOFTWARE_RESET\n\r");
            break;
        case RSTC_USER_RESET:
            printf("USER_RESET\n\r");
            break;
        default:
            printf("Unknown reset cause\n\r");
    }

#endif

    char* file_str;
    file_str = (char*)pvPortMalloc(FILE_MAX_SIZE);

    printf("### INFO: Reading NMEA-Mux configuration from NVRAM\r\n");
    for (int i = 0; i < NMEA_PORT_TASK_NR_NMEA_PORTS; i++) {
        if (storage_read_file(i, file_str) > 0) {
            printf(" Reading tree %d: %s\r\n", i, file_str);
            nmea_search_trees[i] = nmea_tree_init(file_str);
            } else {
            nmea_search_trees[i] = nmea_tree_init("\0");
        }
    }

    storage_read_file(7, file_str);
    printf(" Reading NMEA UART baud rates\n\r");
    update_usart_baudrate_from_str(file_str);
    
    vPortFree(file_str);

#ifdef CONF_NMEA_MUX_NMEA_UARTS_ENABLE
    printf("### INFO: Enabling NMEA ports\n\r");
    for (int i = 0; i < CONF_NMEA_MUX_NR_NMEA_UART_PORTS; i++) {
        printf(" NMEA UART Port %d baud rate = %d\n\r", i + 1, usart_baudrate[i]);
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


    /* Initialize Watch Dog */
    uint32_t timeout_value;
    uint32_t wdt_mode;
    wdt_mode = WDT_MR_WDRSTEN; // | WDT_MR_WDRPROC;
    timeout_value = wdt_get_timeout_value(CONF_NMEA_MUX_WATCHDOG_TIMEOUT_US, BOARD_FREQ_SLCK_XTAL);
    wdt_init(WDT, wdt_mode, timeout_value, timeout_value);
    
    printf("### INFO: Starting all RTOS tasks\r\n");
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

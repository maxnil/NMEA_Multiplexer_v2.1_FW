/*
 * usart_setting.c
 *
 * Created: 2021-10-11 22:43:35
 *  Author: max
 */ 

#include <asf.h>
#include "config/conf_nmea_mux.h"
#include "usart_setting.h"

int usart_baudrate[CONF_NMEA_MUX_NR_NMEA_UART_PORTS] = {4800, 4800, 4800, 4800, 4800};
Usart *p_usart_list[CONF_NMEA_MUX_NR_NMEA_UART_PORTS] = {CONF_UART_NMEA_PORT_1, CONF_UART_NMEA_PORT_2, CONF_UART_NMEA_PORT_3, CONF_UART_NMEA_PORT_4, CONF_UART_NMEA_PORT_5};
    
void init_nmea_port_usart(Usart *p_usart, int baudrate) {
    // Same settings for all NMEA ports
    sam_usart_opt_t nmea_usart_opt = {
        .baudrate       = CONF_NMEA_MUX_NMEA_UARTS_BAUDRATE,
        .char_length    = CONF_UART_NMEA_PORT_CHAR_LENGTH,
        .parity_type    = CONF_UART_NMEA_PORT_PARITY,
        .stop_bits      = CONF_UART_NMEA_PORT_STOP_BITS,
        .channel_mode   = US_MR_CHMODE_NORMAL,  //  US_MR_USART_MODE_HW_HANDSHAKING
        .irda_filter    = 0
    };

    // Peripheral settings
    freertos_peripheral_options_t nmea_periph_opt = {
        .receive_buffer      = NULL,  // Will be set later
        .receive_buffer_size = CONF_NMEA_MUX_NMEA_UARTS_RX_BUFFER_SIZE,
        .interrupt_priority  = configLIBRARY_LOWEST_INTERRUPT_PRIORITY - 1,
        .operation_mode      = USART_RS232,
        .options_flags       = WAIT_RX_COMPLETE | WAIT_TX_COMPLETE
    };

    // NMEA Port #1
    nmea_usart_opt.baudrate = baudrate;
    configASSERT(nmea_periph_opt.receive_buffer = (uint8_t*)pvPortMalloc(CONF_NMEA_MUX_NMEA_UARTS_RX_BUFFER_SIZE));
    configASSERT(freertos_usart_serial_init(p_usart, &nmea_usart_opt, &nmea_periph_opt));
}    

void update_usart_baudrate_from_str(char* str) {
    /* Default settings */
    for (int i = 0; i < CONF_NMEA_MUX_NR_NMEA_UART_PORTS; i++) {
        usart_baudrate[i] = CONF_NMEA_MUX_NMEA_UARTS_BAUDRATE;
    }        

    for (int i = 0; i < CONF_NMEA_MUX_NR_NMEA_UART_PORTS; i++) {
        if (*str < '0' || *str > '9') {
            printf("### ERR: Baud rate for NMEA UART port %d is not valid\n\r", i + 1);
            break;
        }
        usart_baudrate[i] = strtol(str, &str, 10);
        str++;  // Get rid of ','
    }
}

void get_usart_baudrate_str(char* buf) {
    int length;
    for (int i = 0; i < CONF_NMEA_MUX_NR_NMEA_UART_PORTS; i++) {
        length = sprintf(buf, "%d,", usart_baudrate[i]);
        buf += length;
    }
}
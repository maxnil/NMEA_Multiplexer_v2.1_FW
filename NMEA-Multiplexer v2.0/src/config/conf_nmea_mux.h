/*
 * conf_nmea_mux.h
 *
 * Created: 2017-08-07 16:58:21
 *  Author: Max
 */ 


#ifndef CONF_NMEA_MUX_H_
#define CONF_NMEA_MUX_H_

#define SW_VERSION "v0.1.2"

#define DISK_SIZE 8192
#define DISK_ADDR (0x480000 - DISK_SIZE)

#define DEBUG_L1

/* Initial LED test delay (in ms, set to 0 to disable) */
#define CONF_NMEA_MUX_LED_TEST_DELAY_MS 500

/* UART baud rates */
#define CONF_NMEA_MUX_BT_UART_BAUDRATE		9600
#define CONF_NMEA_MUX_CONSOLE_UART_BAUDRATE	115200
#define CONF_NMEA_MUX_NMEA_UART_BAUDRATE	4800

/* UART receive buffer sizes */
#define CONF_NMEA_MUX_BT_RX_BUFFER_SIZE 256
#define CONF_NMEA_MUX_NMEA_RX_BUFFER_SIZE 128

/* Enabled peripheral functions */
#define CONF_NMEA_MUX_ENABLE_DEBUG_CONSOLE
#define CONF_NMEA_MUX_ENABLE_FREERTOS_BT_USART
#define CONF_NMEA_MUX_ENABLE_FREERTOS_NMEA_USART

/* Enable tasks */
#define CONF_NMEA_MUX_ENABLE_BT_TASK
#define CONF_NMEA_MUX_ENABLE_NMEA_PORT_TASKS
#define CONF_NMEA_MUX_ENABLE_TIMER_TASK
#define CONF_NMEA_MUX_ENABLE_USB_CDC_TASK

#endif /* CONF_NMEA_MUX_H_ */
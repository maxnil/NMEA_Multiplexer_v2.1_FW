/*
 * conf_nmea_mux.h
 *
 * Created: 2017-08-07 16:58:21
 *  Author: Max
 */ 


#ifndef CONF_NMEA_MUX_H_
#define CONF_NMEA_MUX_H_

#define SW_VERSION "v0.1.0"

#define DEBUG_L1

/* Initial LED test delay (in ms, set to 0 to disable) */
#define CONF_NMEA_MUX_LED_TEST_DELAY_MS 500

/* UART baud rates */
#define CONF_NMEA_MUX_CONSOLE_UART_BAUDRATE	115200
#define CONF_NMEA_MUX_NMEA_UART_BAUDRATE	4800
#define CONF_NMEA_MUX_BT_UART_BAUDRATE		9600

/* UART's receive buffer size */
#define CONF_NMEA_MUX_NMEA_RX_BUFFER_SIZE 100

/* Enabled peripheral functions */
#define CONF_NMEA_MUX_ENABLE_DEBUG_CONSOLE
#define CONF_NMEA_MUX_ENABLE_FREERTOS_NMEA_USART

/* Enable tasks */
//#define CONF_NMEA_MUX_ENABLE_BT_TASKS
#define CONF_NMEA_MUX_ENABLE_NMEA_PORT_1_TASKS
//#define CONF_NMEA_MUX_ENABLE_NMEA_PORT_2_TASKS
//#define CONF_NMEA_MUX_ENABLE_NMEA_PORT_3_TASKS
//#define CONF_NMEA_MUX_ENABLE_NMEA_PORT_4_TASKS
//#define CONF_NMEA_MUX_ENABLE_NMEA_PORT_5_TASKS
//#define CONF_NMEA_MUX_ENABLE_TIMER_TASK
//#define CONF_NMEA_MUX_ENABLE_USB_CDC_TASK

#endif /* CONF_NMEA_MUX_H_ */
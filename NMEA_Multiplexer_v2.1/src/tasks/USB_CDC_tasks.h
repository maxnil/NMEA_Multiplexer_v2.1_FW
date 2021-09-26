/*
 * USB_CDC_CLI_task.h
 *
 * Created: 2015-06-26 21:22:02
 *  Author: Max
 */


#ifndef USB_CDC_CLI_TASK_H_
#define USB_CDC_CLI_TASK_H_

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ INCLUDES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "task.h"
#include "compiler.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DEFINES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* Task priorities */
#define CDC_CLI_TASK_PRIORITY		(tskIDLE_PRIORITY + 1)
#define CDC_NMEA_TASK_PRIORITY		(tskIDLE_PRIORITY + 1)

/* Task stack sizes */
#define CDC_CLI_TASK_STACK_SIZE		(configMINIMAL_STACK_SIZE * 10)
#define CDC_NMEA_TASK_STACK_SIZE	(configMINIMAL_STACK_SIZE * 4)

extern bool usb_power;
extern bool usb_connected;

/* USB CDC Task creator */
void create_usb_cdc_tasks(void);

void cdc_cli_output(const uint8_t const *message_string);

//void usb_vbus_event_callback(bool b_vbus_high);
//void usb_cdc_suspend_callback();
//void usb_cdc_resume_callback();
//bool usb_cdc_set_dtr_callback(uint8_t port, bool set);
//bool usb_cdc_set_rts_callback(uint8_t port, bool set);
//bool usb_cdc_rx_notify_callback(uint8_t port);
//bool usb_cdc_enable_callback(uint8_t port);
//bool usb_cdc_disable_callback(uint8_t port);

#endif /* USB_CDC_CLI_TASK_H_ */
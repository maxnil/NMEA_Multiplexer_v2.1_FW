/*
 * usb.h
 *
 * Created: 2021-10-01 22:40:17
 *  Author: max
 */ 


#ifndef USB_H_
#define USB_H_

#include "semphr.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DEFINES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* USB CDC ports */
#define USB_CLI_PORT 0
#define USB_NMEA_PORT 1

/* Flags used to indicated whether or not the CDC port is connected or not */
extern volatile bool usb_power;
extern volatile bool cdc_connected;
extern volatile bool cdc_cli_dtr;
extern volatile bool cdc_cli_rts;
extern volatile bool cdc_nmea_dtr;
extern volatile bool cdc_nmea_rts;

// Global Semaphore used by USB Rx interrupt to signal to USB CLI and Data Tasks that new data is available
extern SemaphoreHandle_t cdc_new_rx_data_semaphores[UDI_CDC_PORT_NB];

void usb_init(void);

//void usb_vbus_event_callback(bool b_vbus_high);
//void usb_cdc_suspend_callback();
//void usb_cdc_resume_callback();
//bool usb_cdc_set_dtr_callback(uint8_t port, bool set);
//bool usb_cdc_set_rts_callback(uint8_t port, bool set);
//bool usb_cdc_rx_notify_callback(uint8_t port);
//bool usb_cdc_enable_callback(uint8_t port);
//bool usb_cdc_disable_callback(uint8_t port);

#endif /* USB_H_ */
/**
 * \file
 *
 * \brief USB configuration file for CDC application
 *
 * Copyright (c) 2009-2018 Microchip Technology Inc. and its subsidiaries.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Subject to your compliance with these terms, you may use Microchip
 * software and any derivatives exclusively with Microchip products.
 * It is your responsibility to comply with third party license terms applicable
 * to your use of third party software (including open source software) that
 * may accompany Microchip software.
 *
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES,
 * WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE,
 * INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY,
 * AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT WILL MICROCHIP BE
 * LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
 * LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE
 * SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED OF THE
 * POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT
 * ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY
 * RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
 * THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */

#ifndef _CONF_USB_H_
#define _CONF_USB_H_

#include "compiler.h"
#include "FreeRTOS.h"

//#warning You must refill the following definitions with a correct values

/**
 * USB Device Configuration
 * @{
 */

#define USB_VBUS_FLAGS (PIO_INPUT | PIO_DEBOUNCE | PIO_IT_EDGE)
#define USB_VBUS_PIN_IRQn (PIOA_IRQn)
#define USB_VBUS_PIO_ID (ID_PIOA)
#define USB_VBUS_PIO_MASK (PIO_PA15)

//! Device definition (mandatory)
#define  USB_DEVICE_VENDOR_ID             USB_VID_ATMEL
#define  USB_DEVICE_PRODUCT_ID            USB_PID_ATMEL_ASF_CDC
#define  USB_DEVICE_MAJOR_VERSION         1
#define  USB_DEVICE_MINOR_VERSION         0
#define  USB_DEVICE_POWER                 100 // Consumption on Vbus line (mA)
#define  USB_DEVICE_ATTR                  USB_CONFIG_ATTR_SELF_POWERED

//! USB Device string definitions (Optional)
#define  USB_DEVICE_MANUFACTURE_NAME      "DML"
#define  USB_DEVICE_PRODUCT_NAME          "NMEA Multiplexer"
#define  USB_DEVICE_SERIAL_NAME           "0001"


/**
 * USB Device Callbacks definitions (Optional)
 * @{
 */
#define  UDC_VBUS_EVENT(b_vbus_high)      usb_vbus_event_callback(b_vbus_high)
extern void usb_vbus_event_callback(bool b_vbus_high);
// #define  UDC_SOF_EVENT()                  user_callback_sof_action()
// extern void user_callback_sof_action(void);
#define  UDC_SUSPEND_EVENT()              usb_cdc_suspend_callback()
extern void usb_cdc_suspend_callback(void);
#define  UDC_RESUME_EVENT()               usb_cdc_resume_callback()
extern void usb_cdc_resume_callback(void);
// Mandatory when USB_DEVICE_ATTR authorizes remote wakeup feature
// #define  UDC_REMOTEWAKEUP_ENABLE()        user_callback_remotewakeup_enable()
// extern void user_callback_remotewakeup_enable(void);
// #define  UDC_REMOTEWAKEUP_DISABLE()       user_callback_remotewakeup_disable()
// extern void user_callback_remotewakeup_disable(void);
// When a extra string descriptor must be supported
// other than manufacturer, product and serial string
// #define  UDC_GET_EXTRA_STRING()
//@}

//@}


/**
 * USB Interface Configuration
 * @{
 */
/**
 * Configuration of CDC interface
 * @{
 */

//! Number of communication port used (1 to 3)
#define  UDI_CDC_PORT_NB 1
//#define  UDI_CDC_PORT_NB 1

//! Interface callback definition
extern bool usb_cdc_enable_callback(uint8_t port);
extern bool usb_cdc_disable_callback(uint8_t port);
extern bool usb_cdc_rx_notify_callback(uint8_t port);
extern bool usb_cdc_set_dtr_callback(uint8_t port, bool set);
extern bool usb_cdc_set_rts_callback(uint8_t port, bool set);

#define  UDI_CDC_ENABLE_EXT(port)				usb_cdc_enable_callback(port)
#define  UDI_CDC_DISABLE_EXT(port)				usb_cdc_disable_callback(port)
#define  UDI_CDC_RX_NOTIFY(port)				usb_cdc_rx_notify_callback(port)
#define  UDI_CDC_SET_CODING_EXT(port,cfg)
#define  UDI_CDC_SET_DTR_EXT(port,set)          usb_cdc_set_dtr_callback(port, set)
#define  UDI_CDC_SET_RTS_EXT(port,set)          usb_cdc_set_rts_callback(port, set)
#define  UDI_CDC_TX_EMPTY_NOTIFY(port)

// #define UDI_CDC_ENABLE_EXT(port) my_callback_cdc_enable()
// extern bool my_callback_cdc_enable(void);
// #define UDI_CDC_DISABLE_EXT(port) my_callback_cdc_disable()
// extern void my_callback_cdc_disable(void);
// #define  UDI_CDC_RX_NOTIFY(port) my_callback_rx_notify(port)
// extern void my_callback_rx_notify(uint8_t port);
// #define  UDI_CDC_TX_EMPTY_NOTIFY(port) my_callback_tx_empty_notify(port)
// extern void my_callback_tx_empty_notify(uint8_t port);
// #define  UDI_CDC_SET_CODING_EXT(port,cfg) my_callback_config(port,cfg)
// extern void my_callback_config(uint8_t port, usb_cdc_line_coding_t * cfg);
// #define  UDI_CDC_SET_DTR_EXT(port,set) my_callback_cdc_set_dtr(port,set)
// extern void my_callback_cdc_set_dtr(uint8_t port, bool b_enable);
// #define  UDI_CDC_SET_RTS_EXT(port,set) my_callback_cdc_set_rts(port,set)
// extern void my_callback_cdc_set_rts(uint8_t port, bool b_enable);

//! Define it when the transfer CDC Device to Host is a low rate (<512000 bauds)
//! to reduce CDC buffers size
#define  UDI_CDC_LOW_RATE

#define  UDD_USB_INT_LEVEL configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY

//! Default configuration of communication port
#define  UDI_CDC_DEFAULT_RATE             115200
#define  UDI_CDC_DEFAULT_STOPBITS         CDC_STOP_BITS_1
#define  UDI_CDC_DEFAULT_PARITY           CDC_PAR_NONE
#define  UDI_CDC_DEFAULT_DATABITS         8
//@}
//@}


/**
 * USB Device Driver Configuration
 * @{
 */
//@}

//! The includes of classes and other headers must be done at the end of this file to avoid compile error
#include "udi_cdc_conf.h"

#endif // _CONF_USB_H_

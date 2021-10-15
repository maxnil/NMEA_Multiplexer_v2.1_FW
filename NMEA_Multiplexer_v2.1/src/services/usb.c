/*
 * usb.c
 *
 * Created: 2021-10-01 22:40:36
 *  Author: max
 */ 

/* Atmel library includes. */
#include <udi_cdc.h>
#include <udc.h>

#include "semphr.h"
#include "services/usb.h"
#include "string.h"

volatile bool usb_power = false;
volatile bool cdc_connected = false;
volatile bool cdc_cli_dtr = false;
volatile bool cdc_cli_rts = false;
volatile bool cdc_nmea_dtr = false;
volatile bool cdc_nmea_rts = false;

// Global Semaphore used by USB Rx interrupt to signal to USB CLI and Data Tasks that new data is available
SemaphoreHandle_t cdc_new_rx_data_semaphores[UDI_CDC_PORT_NB];

void usb_init() {
       
    /* Enable VBUS (if needed) */
    if (!udc_include_vbus_monitoring()) {
        /* If VBUS monitoring is not available on this product.  Assume VBUS is present. */
        printf("VBUS monitoring not available on this product\r\n");
        UDC_VBUS_EVENT(true);
    }
    
    for (int i = 0; i < UDI_CDC_PORT_NB; i++) {
        cdc_new_rx_data_semaphores[i] = xSemaphoreCreateBinary();
        configASSERT(cdc_new_rx_data_semaphores[i]);
    }

    /* Start USB */
    udc_start();
}

/*******************************************************************************
 * USB VBUS Event callback function
 */
void usb_vbus_event_callback(bool b_vbus_high) {
    if (b_vbus_high == true) {
        printf("VBUS is high\r\n");
        usb_power = true;
        udc_attach();
    } else {
        printf("VBUS is low\r\n");
        usb_power = false;
        udc_detach();
    }
}

void usb_cdc_suspend_callback() {
    printf("usb_cdc_suspend_callback()\r\n");
}

void usb_cdc_resume_callback() {
    printf("usb_cdc_resume_callback()\r\n");
}


bool usb_cdc_set_dtr_callback(uint8_t port, bool set) {
    char welcome_str[] = "\r\n\nWelcome to the NMEA Multiplexer CLI\r\n=> ";
    
    printf("usb_cdc_set_dtr_callback(%d, %d)\r\n", port, set);
    switch (port) {
        case USB_CLI_PORT:
            cdc_cli_dtr = set;
            break;
        case USB_NMEA_PORT:
            cdc_nmea_dtr = set;
            break;
    }
    if (port == USB_CLI_PORT) {
        udi_cdc_multi_write_buf(USB_CLI_PORT, welcome_str, strlen((char *) welcome_str));
        cdc_connected = set;
    }
    return true;
}

bool usb_cdc_set_rts_callback(uint8_t port, bool set) {
    printf("usb_cdc_set_rts_callback(%d, %d)\r\n", port, set);
    switch (port) {
        case USB_CLI_PORT:
            cdc_cli_rts = set;
            break;
        case USB_NMEA_PORT:
            cdc_nmea_rts = set;
            break;
    }
    return true;
}


/*******************************************************************************
 * USB CDC Received Notify callback function
 * Signals whenever there is new data received on any of the two USB interfaces
 */
bool usb_cdc_rx_notify_callback(uint8_t port) {
    portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

//    printf("usb_cdc_rx_notify_callback(%d)\r\n", port);

    if (port > UDI_CDC_PORT_NB) {
        printf("### ERR: usb_cdc_rx_notify_callback(%d) port number (%d) is greater that the number of supported ports (%d)\r\n", port, port, UDI_CDC_PORT_NB);
        return false;
    }

    // printf("  before xSemaphoreGiveFromISR(%) 0x%.8X\r\n", port, cdc_new_rx_data_semaph[port]);
    xSemaphoreGiveFromISR(cdc_new_rx_data_semaphores[port], &xHigherPriorityTaskWoken);
    // printf("  after xSemaphoreGiveFromISR(%d)\r\n", port);

    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    // printf("after portEND_SWITCHING_ISR()\r\n");
    return true;
}


/*******************************************************************************
 * USB CDC Enable callback function
 */
bool usb_cdc_enable_callback(uint8_t port) {
    (void) port;
    printf("usb_cdc_enable_callback(%d)\r\n", port);
    cdc_connected = true;
    return true;
}


/*******************************************************************************
 * USB CDC Disable callback function
 */
bool usb_cdc_disable_callback(uint8_t port) {
    (void) port;
    printf("usb_cdc_disable_callback(%d)\r\n", port);
    cdc_connected = false;
    return true;
}
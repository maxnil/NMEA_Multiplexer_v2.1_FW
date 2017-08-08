/**
 * \file
 *
 * \brief NMEA Multiplexer v2.0 board definition template
 *
 */


#ifndef USER_BOARD_H
#define USER_BOARD_H

#include <conf_board.h>

// External oscillator settings.
// Uncomment and set correct values if external oscillator is used.

// External oscillator frequency
//#define BOARD_XOSC_HZ          32768

// External oscillator type.
//!< External clock signal
//#define BOARD_XOSC_TYPE        XOSC_TYPE_EXTERNAL
//!< 32.768 kHz resonator on TOSC
//#define BOARD_XOSC_TYPE        XOSC_TYPE_32KHZ
//!< 0.4 to 16 MHz resonator on XTALS
//#define BOARD_XOSC_TYPE        XOSC_TYPE_XTAL

// External oscillator startup time
#define BOARD_XOSC_STARTUP_US  500000

// External oscillator type.
#define BOARD_FREQ_SLCK_XTAL      (32768U)
#define BOARD_FREQ_SLCK_BYPASS    (0U)	// Not available
#define BOARD_FREQ_MAINCK_XTAL    (0U)	// Not available
#define BOARD_FREQ_MAINCK_BYPASS  (0U)	// Not available
#define BOARD_MCK                 CHIP_FREQ_CPU_MAX
#define BOARD_OSC_STARTUP_US      15625

// LED's
#define BT_TX_LED			IOPORT_CREATE_PIN(PIOB,  9)
#define BT_RX_LED			IOPORT_CREATE_PIN(PIOA,  2)
#define PORT_1_ERR_LED		IOPORT_CREATE_PIN(PIOA, 24)
#define PORT_1_RX_LED		IOPORT_CREATE_PIN(PIOB, 13)
#define PORT_1_TX_LED		IOPORT_CREATE_PIN(PIOB,  8)
#define PORT_2_ERR_LED		IOPORT_CREATE_PIN(PIOA, 14)
#define PORT_2_RX_LED		IOPORT_CREATE_PIN(PIOA, 20)
#define PORT_2_TX_LED		IOPORT_CREATE_PIN(PIOA, 11)
#define PORT_3_ERR_LED		IOPORT_CREATE_PIN(PIOA, 18)
#define PORT_3_RX_LED		IOPORT_CREATE_PIN(PIOA, 19)
#define PORT_3_TX_LED		IOPORT_CREATE_PIN(PIOA, 17)
#define PORT_4_ERR_LED		IOPORT_CREATE_PIN(PIOA, 30)
#define PORT_4_RX_LED		IOPORT_CREATE_PIN(PIOA, 29)
#define PORT_4_TX_LED		IOPORT_CREATE_PIN(PIOA, 16)
#define PORT_5_ERR_LED		IOPORT_CREATE_PIN(PIOB, 14)
#define PORT_5_RX_LED		IOPORT_CREATE_PIN(PIOB, 15)
#define PORT_5_TX_LED		IOPORT_CREATE_PIN(PIOA, 31)

#endif // USER_BOARD_H

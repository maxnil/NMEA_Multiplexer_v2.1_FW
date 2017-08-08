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
#define BT_TX_LED_PIN			IOPORT_CREATE_PIN(PIOB,  9)
#define BT_RX_LED_PIN			IOPORT_CREATE_PIN(PIOA,  2)
#define PORT_1_ERR_LED_PIN		IOPORT_CREATE_PIN(PIOA, 24)
#define PORT_1_RX_LED_PIN		IOPORT_CREATE_PIN(PIOB, 13)
#define PORT_1_TX_LED_PIN		IOPORT_CREATE_PIN(PIOB,  8)
#define PORT_2_ERR_LED_PIN		IOPORT_CREATE_PIN(PIOA, 14)
#define PORT_2_RX_LED_PIN		IOPORT_CREATE_PIN(PIOA, 20)
#define PORT_2_TX_LED_PIN		IOPORT_CREATE_PIN(PIOA, 11)
#define PORT_3_ERR_LED_PIN		IOPORT_CREATE_PIN(PIOA, 18)
#define PORT_3_RX_LED_PIN		IOPORT_CREATE_PIN(PIOA, 19)
#define PORT_3_TX_LED_PIN		IOPORT_CREATE_PIN(PIOA, 17)
#define PORT_4_ERR_LED_PIN		IOPORT_CREATE_PIN(PIOA, 30)
#define PORT_4_RX_LED_PIN		IOPORT_CREATE_PIN(PIOA, 29)
#define PORT_4_TX_LED_PIN		IOPORT_CREATE_PIN(PIOA, 16)
#define PORT_5_ERR_LED_PIN		IOPORT_CREATE_PIN(PIOB, 14)
#define PORT_5_RX_LED_PIN		IOPORT_CREATE_PIN(PIOB, 15)
#define PORT_5_TX_LED_PIN		IOPORT_CREATE_PIN(PIOA, 31)

/* BT and USB GPIO */
#define BT_KEY_PIN				IOPORT_CREATE_PIN(PIOA, 23)
#define BT_RESET_N_PIN			IOPORT_CREATE_PIN(PIOA, 29)
#define USB_VBUS_PIN			IOPORT_CREATE_PIN(PIOA, 15)

/* UART 0 (BT Module) pins */
#define PINS_USART0				(PIO_PA9A_RXD0 | PIO_PA10A_TXD0 | PIO_PA25A_CTS0 | PIO_PA26A_RTS0)
#define PINS_USART0_FLAGS		(IOPORT_MODE_MUX_A)
#define PINS_USART0_PORT		IOPORT_PIOA

/* UART 1 (NMEA Port 2) pins */
#define PINS_USART1				(PIO_PB2A_RXD1 | PIO_PB3A_TXD1)
#define PINS_USART1_FLAGS		(IOPORT_MODE_MUX_A)
#define PINS_USART1_PORT		IOPORT_PIOB

/* UART 2 (NMEA Port 5) pins */
#define PINS_USART2				(PIO_PA5A_RXD2 | PIO_PA6A_TXD2)
#define PINS_USART2_FLAGS		(IOPORT_MODE_MUX_A)
#define PINS_USART2_PORT		IOPORT_PIOA

/* UART 3 (NMEA Port 4) pins */
#define PINS_USART3				(PIO_PA4A_RXD3 | PIO_PA3A_TXD3)
#define PINS_USART3_FLAGS		(IOPORT_MODE_MUX_A)
#define PINS_USART3_PORT		IOPORT_PIOA

/* UART 4 (Debug Console) pins */
#define PINS_USART4				(PIO_PB11A_RXD4 | PIO_PB10A_TXD4)
#define PINS_USART4_FLAGS		(IOPORT_MODE_MUX_A)
#define PINS_USART4_PORT		IOPORT_PIOB

/* UART 5 (NMEA Port 1) pins */
#define PINS_USART5				(PIO_PA12A_RXD5 | PIO_PA13A_TXD5)
#define PINS_USART5_FLAGS		(IOPORT_MODE_MUX_A)
#define PINS_USART5_PORT		IOPORT_PIOA

/* UART 6 (NMEA Port 3) pins */
#define PINS_USART6				(PIO_PB1B_RXD6 | PIO_PB0B_TXD6)
#define PINS_USART6_FLAGS		(IOPORT_MODE_MUX_B)
#define PINS_USART6_PORT		IOPORT_PIOB

#endif // USER_BOARD_H

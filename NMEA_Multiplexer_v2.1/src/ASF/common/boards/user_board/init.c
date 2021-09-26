/**
 * \file
 *
 * \brief NMEA Multiplexer v2.0 board initialization
 *
 */

#include <asf.h>
#include <board.h>
#include <conf_board.h>

/**
 * \brief Set peripheral mode for IOPORT pins.
 * It will configure port mode and disable pin mode (but enable peripheral).
 * \param port IOPORT port to configure
 * \param masks IOPORT pin masks to configure
 * \param mode Mode masks to configure for the specified pin (\ref ioport_modes)
 */
#define ioport_set_port_peripheral_mode(port, masks, mode) \
	do {\
		ioport_set_port_mode(port, masks, mode);\
		ioport_disable_port(port, masks);\
	} while (0)


void board_init(void)
{
	WDT->WDT_MR = WDT_MR_WDDIS;                         // disable watchdog

	ioport_init();                                      // call before using IOPORT service
	delay_init(BOARD_MCK);

	/** Configure LED pins */
	ioport_set_pin_dir(LED_BT_RX_PIN,	 	  IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(LED_BT_TX_PIN,		  IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(LED_PORT_1_ERR_PIN,	IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(LED_PORT_1_RX_PIN,	IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(LED_PORT_1_TX_PIN,	IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(LED_PORT_2_ERR_PIN,	IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(LED_PORT_2_RX_PIN,	IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(LED_PORT_2_TX_PIN,	IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(LED_PORT_3_ERR_PIN,	IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(LED_PORT_3_RX_PIN,	IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(LED_PORT_3_TX_PIN,	IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(LED_PORT_4_ERR_PIN,	IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(LED_PORT_4_RX_PIN,	IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(LED_PORT_4_TX_PIN,	IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(LED_PORT_5_ERR_PIN,	IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(LED_PORT_5_RX_PIN,	IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(LED_PORT_5_TX_PIN,	IOPORT_DIR_OUTPUT);

	/** Turn LED's off */
	ioport_set_pin_level(LED_BT_TX_PIN,			IOPORT_PIN_LEVEL_LOW);
	ioport_set_pin_level(LED_BT_RX_PIN,			IOPORT_PIN_LEVEL_LOW);
	ioport_set_pin_level(LED_PORT_1_ERR_PIN,	IOPORT_PIN_LEVEL_LOW);
	ioport_set_pin_level(LED_PORT_1_RX_PIN,		IOPORT_PIN_LEVEL_LOW);
	ioport_set_pin_level(LED_PORT_1_TX_PIN,		IOPORT_PIN_LEVEL_LOW);
	ioport_set_pin_level(LED_PORT_2_ERR_PIN,	IOPORT_PIN_LEVEL_LOW);
	ioport_set_pin_level(LED_PORT_2_RX_PIN,		IOPORT_PIN_LEVEL_LOW);
	ioport_set_pin_level(LED_PORT_2_TX_PIN,		IOPORT_PIN_LEVEL_LOW);
	ioport_set_pin_level(LED_PORT_3_ERR_PIN,	IOPORT_PIN_LEVEL_LOW);
	ioport_set_pin_level(LED_PORT_3_RX_PIN,		IOPORT_PIN_LEVEL_LOW);
	ioport_set_pin_level(LED_PORT_3_TX_PIN,		IOPORT_PIN_LEVEL_LOW);
	ioport_set_pin_level(LED_PORT_4_ERR_PIN,	IOPORT_PIN_LEVEL_LOW);
	ioport_set_pin_level(LED_PORT_4_RX_PIN,		IOPORT_PIN_LEVEL_LOW);
	ioport_set_pin_level(LED_PORT_4_TX_PIN,		IOPORT_PIN_LEVEL_LOW);
	ioport_set_pin_level(LED_PORT_5_ERR_PIN,	IOPORT_PIN_LEVEL_LOW);
	ioport_set_pin_level(LED_PORT_5_RX_PIN,		IOPORT_PIN_LEVEL_LOW);
	ioport_set_pin_level(LED_PORT_5_TX_PIN,		IOPORT_PIN_LEVEL_LOW);

	/** Configure BT and USB pins */
	ioport_set_pin_dir(BT_KEY_PIN,			IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(BT_RESET_N_PIN,		IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(USB_VBUS_PIN,		IOPORT_DIR_INPUT);

	/** Disable BT */
	ioport_set_pin_level(BT_KEY_PIN,		IOPORT_PIN_LEVEL_HIGH);
	ioport_set_pin_level(BT_RESET_N_PIN,	IOPORT_PIN_LEVEL_LOW);

	/** Configure USART0 (BT Module) pins */
	ioport_set_port_peripheral_mode(PINS_USART0_PORT, PINS_USART0, PINS_USART0_FLAGS);

	/** Configure USART1 (NMEA Port 2) pins */
	ioport_set_port_peripheral_mode(PINS_USART1_PORT, PINS_USART1, PINS_USART1_FLAGS);

	/** Configure USART2 (NMEA Port 5) pins */
	ioport_set_port_peripheral_mode(PINS_USART2_PORT, PINS_USART2, PINS_USART2_FLAGS);

	/** Configure USART3 (NMEA Port 4) pins */
	ioport_set_port_peripheral_mode(PINS_USART3_PORT, PINS_USART3, PINS_USART3_FLAGS);

	/** Configure USART4 (Debug Console) pins */
	ioport_set_port_peripheral_mode(PINS_USART4_PORT, PINS_USART4, PINS_USART4_FLAGS);

	/** Configure USART5 (NMEA Port 1) pins */
	ioport_set_port_peripheral_mode(PINS_USART5_PORT, PINS_USART5, PINS_USART5_FLAGS);

	/** Configure USART6 (NMEA Port 3) pins */
	ioport_set_port_peripheral_mode(PINS_USART6_PORT, PINS_USART6, PINS_USART6_FLAGS);
}

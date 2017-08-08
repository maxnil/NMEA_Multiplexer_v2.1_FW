/**
 * \file
 *
 * \brief NMEA Multiplexer v2.0 board initialization
 *
 */

#include <asf.h>
#include <board.h>
#include <conf_board.h>

void board_init(void)
{
	WDT->WDT_MR = WDT_MR_WDDIS;                         // disable watchdog
	
	ioport_init();                                      // call before using IOPORT service
	delay_init(BOARD_MCK);
	
	/** Configure LED pins */
	ioport_set_pin_dir(BT_RX_LED,      IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(BT_TX_LED,      IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(PORT_1_ERR_LED, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(PORT_1_RX_LED,  IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(PORT_1_TX_LED,  IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(PORT_2_ERR_LED, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(PORT_2_RX_LED,  IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(PORT_2_TX_LED,  IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(PORT_3_ERR_LED, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(PORT_3_RX_LED,  IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(PORT_3_TX_LED,  IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(PORT_4_ERR_LED, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(PORT_4_RX_LED,  IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(PORT_4_TX_LED,  IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(PORT_5_ERR_LED, IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(PORT_5_RX_LED,  IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(PORT_5_TX_LED,  IOPORT_DIR_OUTPUT);
	
	/** Turn LED's off */
	ioport_set_pin_level(BT_TX_LED,      IOPORT_PIN_LEVEL_LOW);
	ioport_set_pin_level(BT_RX_LED,      IOPORT_PIN_LEVEL_LOW);
	ioport_set_pin_level(PORT_1_ERR_LED, IOPORT_PIN_LEVEL_LOW);
	ioport_set_pin_level(PORT_1_RX_LED,  IOPORT_PIN_LEVEL_LOW);
	ioport_set_pin_level(PORT_1_TX_LED,  IOPORT_PIN_LEVEL_LOW);
	ioport_set_pin_level(PORT_2_ERR_LED, IOPORT_PIN_LEVEL_LOW);
	ioport_set_pin_level(PORT_2_RX_LED,  IOPORT_PIN_LEVEL_LOW);
	ioport_set_pin_level(PORT_2_TX_LED,  IOPORT_PIN_LEVEL_LOW);
	ioport_set_pin_level(PORT_3_ERR_LED, IOPORT_PIN_LEVEL_LOW);
	ioport_set_pin_level(PORT_3_RX_LED,  IOPORT_PIN_LEVEL_LOW);
	ioport_set_pin_level(PORT_3_TX_LED,  IOPORT_PIN_LEVEL_LOW);
	ioport_set_pin_level(PORT_4_ERR_LED, IOPORT_PIN_LEVEL_LOW);
	ioport_set_pin_level(PORT_4_RX_LED,  IOPORT_PIN_LEVEL_LOW);
	ioport_set_pin_level(PORT_4_TX_LED,  IOPORT_PIN_LEVEL_LOW);
	ioport_set_pin_level(PORT_5_ERR_LED, IOPORT_PIN_LEVEL_LOW);
	ioport_set_pin_level(PORT_5_RX_LED,  IOPORT_PIN_LEVEL_LOW);
	ioport_set_pin_level(PORT_5_TX_LED,  IOPORT_PIN_LEVEL_LOW);
}

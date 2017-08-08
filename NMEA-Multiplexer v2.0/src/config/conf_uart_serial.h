/**
 * \file
 *
 * \brief Serial USART service configuration.
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */
/*
 * Support and FAQ: visit <a href="http://www.atmel.com/design-support/">Atmel Support</a>
 */

#ifndef CONF_USART_SERIAL_H
#define CONF_USART_SERIAL_H

/** Debug console USART Interface */
#define CONF_UART_CONSOLE				USART4
#define CONF_UART_CONSOLE_BAUDRATE		CONF_NMEA_MUX_CONSOLE_UART_BAUDRATE
#define CONF_UART_CONSOLE_CHAR_LENGTH	US_MR_CHRL_8_BIT
#define CONF_UART_CONSOLE_PARITY		US_MR_PAR_NO
#define CONF_UART_CONSOLE_STOP_BITS		US_MR_NBSTOP_1_BIT

/** NMEA USART Interfaces */
#define CONF_UART_NMEA_PORT_1			USART5
#define CONF_UART_NMEA_PORT_2			USART1
#define CONF_UART_NMEA_PORT_3			USART6
#define CONF_UART_NMEA_PORT_4			USART3
#define CONF_UART_NMEA_PORT_5			USART2
#define CONF_UART_NMEA_PORT_BAUDRATE	CONF_NMEA_MUX_NMEA_UART_BAUDRATE
#define CONF_UART_NMEA_PORT_CHAR_LENGTH	US_MR_CHRL_8_BIT
#define CONF_UART_NMEA_PORT_PARITY		US_MR_PAR_NO
#define CONF_UART_NMEA_PORT_STOP_BITS	US_MR_NBSTOP_1_BIT

/** BlueThoot USART Interface */
#define CONF_UART_BT					USART0
#define CONF_UART_BT_BAUDRATE			CONF_NMEA_MUX_BT_UART_BAUDRATE
#define CONF_UART_BT_CHAR_LENGTH		US_MR_CHRL_8_BIT
#define CONF_UART_BT_PARITY				US_MR_PAR_NO
#define CONF_UART_BT_STOP_BITS			US_MR_NBSTOP_1_BIT

#endif/* CONF_USART_SERIAL_H_INCLUDED */

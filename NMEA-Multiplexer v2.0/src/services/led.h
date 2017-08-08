/*
 * led.h
 *
 * Created: 2017-08-07 16:21:41
 *  Author: Max
 */ 


#ifndef LED_H_
#define LED_H_

#include <asf.h>
//#include "compiler.h"
//#include "ioport.h"

/**
 * Turns LED On
 */
void LED_On(ioport_pin_t led_gpio);

/**
 * Turns LED Off
 */
void LED_Off(ioport_pin_t led_gpio);

/**
 * Toggles LED
 */
void LED_Toggle(ioport_pin_t led_gpio);

/**
 * LED test
 */
void LED_Test(int delay);

#endif /* LED_H_ */
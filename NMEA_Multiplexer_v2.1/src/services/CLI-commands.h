/*
 * CLI_commands.h
 *
 * Created: 2015-06-26 21:23:57
 *  Author: Max
 */ 


#ifndef CLI_COMMANDS_H_
#define CLI_COMMANDS_H_

//#include "services/nmea_func.h"

#define RESPONSE_TIMEOUT_MS 1000

extern bool disable_usb_nmea;

void vRegisterCLICommands(void);

#endif /* CLI_COMMANDS_H_ */
/*
 * CLI-commands.c
 * NMEA Multiplexer CLI Commands
 *
 * Created: 2017-08-09 17:07:04
 *  Author: Max
 */

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ INCLUDES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

/* Standard includes. */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* FreeRTOS+CLI includes. */
#include "FreeRTOS_CLI.h"

#include "CLI-commands.h"

#include "queue.h"
#include "tasks/task_queues.h"
#include "tasks/nmea_port_tasks.h"

#include "config/conf_nmea_mux.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DEFINES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* Macro to make the lines shorter */
#define CLI_DEF_T static const CLI_Command_Definition_t

//~~~~~~~~~~~~~~~~~~~~~~~~ LOCAL FUNCTION DECLARATIONS ~~~~~~~~~~~~~~~~~~~~~~~~~

static portBASE_TYPE get_version_cmd    (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE mem_cmd            (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE ps_cmd             (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE task_stats_cmd     (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE get_portmasks_cmd  (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE set_portmask_cmd   (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE save_settings_cmd  (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE load_settings_cmd  (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ LOCAL VARIABLES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* CLI command definitions */
CLI_DEF_T get_version_cmd_def   = {"get_version",   "get_version\r\n",                 get_version_cmd,   0};
CLI_DEF_T mem_cmd_def           = {"mem",           "mem\r\n",                         mem_cmd,           0};
CLI_DEF_T ps_cmd_def            = {"ps",            "ps\r\n",                          ps_cmd,            0};
CLI_DEF_T task_stats_cmd_def    = {"task-stats",    "task-stats\r\n",                  task_stats_cmd,    0};
CLI_DEF_T get_portmasks_cmd_def = {"get_portmasks", "get_portmasks port\r\n",          get_portmasks_cmd, 1};
CLI_DEF_T set_portmask_cmd_def  = {"set_portmask",  "set_portmask port\r\n",           set_portmask_cmd,  3};
CLI_DEF_T save_settings_cmd_def = {"save_settings", "save_settings\r\n",               save_settings_cmd, 0};
CLI_DEF_T load_settings_cmd_def = {"load_settings", "load_settings\r\n",               load_settings_cmd, 0};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*******************************************************************************
 * Registers all CLI commands
 */
void vRegisterCLICommands(void) {
	FreeRTOS_CLIRegisterCommand(&get_version_cmd_def);
	FreeRTOS_CLIRegisterCommand(&mem_cmd_def);
	FreeRTOS_CLIRegisterCommand(&ps_cmd_def);
	FreeRTOS_CLIRegisterCommand(&task_stats_cmd_def);
	FreeRTOS_CLIRegisterCommand(&get_portmasks_cmd_def);
	FreeRTOS_CLIRegisterCommand(&set_portmask_cmd_def);
	FreeRTOS_CLIRegisterCommand(&save_settings_cmd_def);
	FreeRTOS_CLIRegisterCommand(&load_settings_cmd_def);
}


/*******************************************************************************
 * "Get Version" command
 * Returns SW version
 */
static portBASE_TYPE get_version_cmd(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	configASSERT(pcWriteBuffer);

	sprintf(pcWriteBuffer, "NMEA Multiplexer %s (%s, %s)\r\n", SW_VERSION, __DATE__, __TIME__);

	return pdFALSE;
}


/*******************************************************************************
 * "mem" command
 * Returns memory status
 */
static portBASE_TYPE mem_cmd(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	configASSERT(pcWriteBuffer);

	/* Return current Heap memory status */
	sprintf(pcWriteBuffer, "\r\nAllocated memory: %d/%d (%d%c free)\r\n", configTOTAL_HEAP_SIZE-xPortGetFreeHeapSize(), configTOTAL_HEAP_SIZE, (100*xPortGetFreeHeapSize())/configTOTAL_HEAP_SIZE, '%');

	return pdFALSE;
}


/*******************************************************************************
 * "ps" command
 * Returns task run-time status
 */
static portBASE_TYPE ps_cmd(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	const int8_t *const stats_table_header = (int8_t *) "\r\nTask     \tAbs Time\t% Time\r\n*******************************************\r\n";

	configASSERT(pcWriteBuffer);

	/* Generate a table of task stats. */
	strcpy((char *) pcWriteBuffer, (char *)stats_table_header);
// Not implemented /Max	vTaskGetRunTimeStats(pcWriteBuffer + strlen((char *)stats_table_header));

	return pdFALSE;
}


/*******************************************************************************
 * "Task Status" command
 */
static portBASE_TYPE task_stats_cmd(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	const int8_t *const task_table_header = (int8_t *) "\r\nTask     \tState\tPri\tStack\t#\r\n*******************************************\r\n";

	configASSERT(pcWriteBuffer);

	/* Generate a table of task stats. */
	strcpy((char *) pcWriteBuffer, (char *) task_table_header);
//Not implemented /Max	vTaskList(pcWriteBuffer + strlen((char *) task_table_header));

	/* There is no more data to return after this single string, so return
	pdFALSE. */
	return pdFALSE;
}


/*******************************************************************************
 * "Get NMEA sentence list" command
 */
static portBASE_TYPE get_portmasks_cmd(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	const char *parameter_string;
	portBASE_TYPE parameter_string_length;
	static char* nmea_str = NULL;
	static char* nmea_str_bak = NULL;
	char nmea_type_str[10];
	char portmask_str[3];
	uint8_t portmask;
	int index = 0;
	int i;

	int port;

	configASSERT(pcWriteBuffer);

	if (nmea_str == NULL) {
		/* Obtain the parameter string. */
		parameter_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &parameter_string_length);

		/* Check parameter */
		if (parameter_string == NULL || parameter_string[0] < '0' || parameter_string[0] > '4') {
			sprintf(pcWriteBuffer, "Unknown port (ports 0->4)\n\r");
			return pdFALSE;
		}

		port = parameter_string[0] - '0';

		// Allocate NMEA string buffer
		nmea_str = (char*)pvPortMalloc(1024);
//		printf("get_portmasks: nmea_str %p\n\r", nmea_str);
		nmea_str_bak = nmea_str;  // Save start of buffer so that we can free it later
		nmea_tree_get_string(nmea_search_tree[port], nmea_str);

//		printf("get_portmasks_cmd first run\n\r");
//		printf("nmea_str_node = %p\n\r", nmea_str_node);
		sprintf(pcWriteBuffer, "\n\rNMEA Port %d:\n\r", port);
//		cnt = 0;
		return pdPASS;
	}


	// Copy string up to '='
	while (*nmea_str != '=') {
		nmea_type_str[index++] = *nmea_str;
		nmea_str++;
	}

	nmea_type_str[index] = 0x00;		// End of NMEA string

	nmea_str++;		// Skip '='

	// Copy string up to '='
	for (i = 0; i < 2; i++) {
		portmask_str[i] = (char)*nmea_str;
		nmea_str++;
	}

	portmask_str[2] = "\0";

	portmask = strtol(portmask_str, NULL, 16);

	printf("pcWriteBuffer: %s\n\r", nmea_type_str);
	sprintf(pcWriteBuffer, "%s 0x%.2X\n\r", nmea_type_str, portmask);

	if (*nmea_str == 0x00) {
		printf("Last\n\r");
		vPortFree(nmea_str_bak);
		nmea_str_bak = NULL;
		nmea_str = NULL;
		return pdFALSE;
	} else {
		printf("More\n\r");
		return pdPASS;
	}
}


/*******************************************************************************
 * "Set NMEA portmask" command
 */
static portBASE_TYPE set_portmask_cmd(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	const char *parameter_string;
	portBASE_TYPE parameter_1_string_length;
	portBASE_TYPE parameter_2_string_length;
	portBASE_TYPE parameter_3_string_length;
	//static char* nmea_str = NULL;
	//static char* nmea_str_bak = NULL;
	//char temp_str[10];
	uint8_t portmask;
	//int index = 0;

	int port;
	char* nmea_type_str;

	configASSERT(pcWriteBuffer);

	/* Obtain the first parameter string. */
	parameter_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &parameter_1_string_length);

	/* Check parameter */
	if (parameter_string == NULL || parameter_string[0] < '0' || parameter_string[0] > '4') {
		sprintf(pcWriteBuffer, "Unknown port (ports 0->4)\n\r");
		return pdFALSE;
	}

	port = parameter_string[0] - '0';

	/* Obtain the second parameter string. */
	nmea_type_str = FreeRTOS_CLIGetParameter(pcCommandString, 2, &parameter_2_string_length);

	printf("nmea_type_str %s (%d)\n\r", nmea_type_str, parameter_2_string_length);

	/* Check parameter */
	if (nmea_type_str == NULL) {
		sprintf(pcWriteBuffer, "NMEA type missing\n\r");
		return pdFALSE;
	}

	if (nmea_type_str[0] != '$') {
		sprintf(pcWriteBuffer, "NMEA type does not start with $\n\r");
		return pdFALSE;
	}

	/* Obtain the third parameter string. */
	parameter_string = FreeRTOS_CLIGetParameter(pcCommandString, 3, &parameter_3_string_length);

	/* Check parameter */
	if (parameter_string == NULL) {
		sprintf(pcWriteBuffer, "Port mask missing\n\r");
		return pdFALSE;
	}

	nmea_type_str[parameter_2_string_length] = 0x00; // Terminate string

	portmask = strtol(parameter_string, NULL, 16);

	printf("set_portmask: %d, %s, 0x%.2X\n\r", port, nmea_type_str, portmask);

	nmea_tree_add(nmea_search_tree[port], nmea_type_str, portmask);

	sprintf(pcWriteBuffer, "\n\rNMEA port mask set\n\r");

//	printf("pcWriteBuffer: %s\n\r", temp_str);
//	sprintf(pcWriteBuffer, "%s 0x%.2X\n\r", temp_str, portmask);

	return pdFALSE;
}


/*******************************************************************************
 * "save settings" command
 */
static portBASE_TYPE save_settings_cmd(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	char* nmea_str;
	int i;
	int* flash_ptr;
#if 0
	configASSERT(pcWriteBuffer);

	nmea_str = (char*)pvPortMalloc(1024);
	flash_ptr = DISK_ADDR;

	if (flash_erase_page(DISK_ADDR, IFLASH_ERASE_PAGES_16) != FLASH_RC_OK) {
		printf("flash_erase_page failed\n\r");
		sprintf(pcWriteBuffer, "Flash erase failed\r\n");
		return pdFALSE;
	}

	for (i = 0; i < 5; i++) {
		nmea_tree_get_string(nmea_search_tree[i], nmea_str);
		flash_ptr = DISK_ADDR + (i * 1024);
		printf("Saving %p: %s\n\r", flash_ptr, nmea_str);
		if (flash_write(flash_ptr, nmea_str, strlen(nmea_str), 0) != FLASH_RC_OK) {
			sprintf(pcWriteBuffer, "Saving port %d settings failed\r\n", i);
			return pdFALSE;
		}
	}
	sprintf(pcWriteBuffer, "Saved settings to Flash successful\r\n");
#endif
	return pdFALSE;
}



/*******************************************************************************
 * "load settings" command
 */
static portBASE_TYPE load_settings_cmd(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	int* flash_ptr;
	int i, j;
#if 0
	configASSERT(pcWriteBuffer);


	for (j = 0; j < 5; j++) {
		flash_ptr = DISK_ADDR + (j * 1024);
		printf("Port %d\n\r", j);
		for (i = 0; i < 10; i++) {
			printf("Flash %.8p = 0x%.8X\n\r", flash_ptr, *flash_ptr);
			flash_ptr++;
		}
	}

	sprintf(pcWriteBuffer, "Load settings from Flash successful\r\n");
#endif
	return pdFALSE;
}

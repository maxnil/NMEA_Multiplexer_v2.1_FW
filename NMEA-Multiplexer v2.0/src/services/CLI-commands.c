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

static portBASE_TYPE get_version_cmd   (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE mem_cmd           (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE ps_cmd            (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE task_stats_cmd    (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE get_portmask_cmd  (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ LOCAL VARIABLES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* CLI command definitions */
CLI_DEF_T get_version_cmd_def   = {"get_version",   "get_version\r\n",                 get_version_cmd,   0};
CLI_DEF_T mem_cmd_def           = {"mem",           "mem\r\n",                         mem_cmd,           0};
CLI_DEF_T ps_cmd_def            = {"ps",            "ps\r\n",                          ps_cmd,            0};
CLI_DEF_T task_stats_cmd_def    = {"task-stats",    "task-stats\r\n",                  task_stats_cmd,    0};
CLI_DEF_T get_portmask_cmd_def  = {"get_portmask",  "get_portmask port\r\n",           get_portmask_cmd, 1};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*******************************************************************************
 * Registers all CLI commands
 */
void vRegisterCLICommands(void) {
	FreeRTOS_CLIRegisterCommand(&get_version_cmd_def);
	FreeRTOS_CLIRegisterCommand(&mem_cmd_def);
	FreeRTOS_CLIRegisterCommand(&ps_cmd_def);
	FreeRTOS_CLIRegisterCommand(&task_stats_cmd_def);
	FreeRTOS_CLIRegisterCommand(&get_portmask_cmd_def);
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
	vTaskGetRunTimeStats(pcWriteBuffer + strlen((char *)stats_table_header));

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
	vTaskList(pcWriteBuffer + strlen((char *) task_table_header));

	/* There is no more data to return after this single string, so return
	pdFALSE. */
	return pdFALSE;
}


/*******************************************************************************
 * "Get NMEA sentence list" command
 */
static portBASE_TYPE get_portmask_cmd(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
	const char *parameter_string;	
	portBASE_TYPE parameter_string_length;
	static nmea_str_node_t* nmea_str_node = NULL;
//	static int cnt = 0;
	int port;

	configASSERT(pcWriteBuffer);
	
	if (nmea_str_node == NULL) {
		/* Obtain the parameter string. */
		parameter_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &parameter_string_length);
	
		/* Check parameter */
		if (parameter_string != NULL && (parameter_string[0] >= '0'  && parameter_string[0] <= '4')) {
			port = parameter_string[0] - '0';
		} else {
			sprintf(pcWriteBuffer, "Unknown port (ports 0->4)\n\r");
			return pdFALSE;
		}
		nmea_str_node = nmea_tree_get_list(nmea_search_tree[port]);
		nmea_tree_get_string(nmea_search_tree[port]);
		
//		printf("get_portmask_cmd first run\n\r");
//		printf("nmea_str_node = %p\n\r", nmea_str_node);
		sprintf(pcWriteBuffer, "\n\rNMEA Port %d:\n\r", port);
//		cnt = 0;
		return pdPASS;
	}

//	printf("nmea_str_node = %p\n\r", nmea_str_node);
//	printf("get_portmask_cmd #%d run\n\r", cnt);
//	cnt = cnt + 1;

	sprintf(pcWriteBuffer, "%s 0x%.2X\n\r", nmea_str_node->str, nmea_str_node->port_mask);

	// Get next NMEA sentence
	nmea_str_node = nmea_str_node->next;
	
	if (nmea_str_node == NULL) {
//		printf("Last\n\r");
		return pdFALSE;
	} else {
//		printf("More\n\r");
		return pdPASS;
	}
}

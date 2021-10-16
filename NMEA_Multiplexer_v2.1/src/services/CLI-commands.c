/*
 * CLI-commands.c
 * NMEA Multiplexer CLI Commands
 *
 * Created: 2017-08-09 17:07:04
 *  Author: Max
 */

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ INCLUDES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include <asf.h>

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

#include "config/conf_nmea_mux.h"
#include "services/nmea_func.h"
#include "services/storage.h"
#include "services/usart_setting.h"
#include "tasks/nmea_port_tasks.h"

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ DEFINES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//~~~~~~~~~~~~~~~~~~~~~~~~ LOCAL FUNCTION DECLARATIONS ~~~~~~~~~~~~~~~~~~~~~~~~~

static portBASE_TYPE get_version_cmd    (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE mem_cmd            (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE cli_cmd            (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE ps_cmd             (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE task_stats_cmd     (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE get_portmask_cmd   (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE set_portmask_cmd   (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE save_settings_cmd  (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE erase_flash_cmd    (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE reset_cmd          (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE set_baudrate_cmd   (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);
static portBASE_TYPE get_baudrate_cmd   (char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString);

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ LOCAL VARIABLES ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/* CLI command definitions */
CLI_Command_Definition_t get_version_cmd_def   = {"get_version",   "get_version\r\n",                 get_version_cmd,   0};
CLI_Command_Definition_t mem_cmd_def           = {"mem",           "mem\r\n",                         mem_cmd,           0};
CLI_Command_Definition_t cli_cmd_def           = {"cli",           "cli\r\n",                         cli_cmd,           0};
CLI_Command_Definition_t ps_cmd_def            = {"ps",            "ps\r\n",                          ps_cmd,            0};
CLI_Command_Definition_t task_stats_cmd_def    = {"task_stats",    "task_stats\r\n",                  task_stats_cmd,    0};
CLI_Command_Definition_t get_portmask_cmd_def  = {"get_portmask",  "get_portmask port\r\n",           get_portmask_cmd,  1};
CLI_Command_Definition_t set_portmask_cmd_def  = {"set_portmask",  "set_portmask port\r\n",           set_portmask_cmd,  3};
CLI_Command_Definition_t save_settings_cmd_def = {"save_settings", "save_settings\r\n",               save_settings_cmd, 0};
CLI_Command_Definition_t erase_flash_cmd_def   = {"erase_flash",   "erase_flash\r\n",                 erase_flash_cmd,   0};
CLI_Command_Definition_t reset_cmd_def         = {"reset",         "reset\r\n",                       reset_cmd,         0};
CLI_Command_Definition_t set_baudrate_cmd_def  = {"set_baudrate",  "set_baudrate\r\n",                set_baudrate_cmd,  2};
CLI_Command_Definition_t get_baudrate_cmd_def  = {"get_baudrate",  "get_baudrate\r\n",                get_baudrate_cmd,  0};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ FUNCTIONS ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool disable_usb_nmea = false;

/*******************************************************************************
 * Registers all CLI commands
 */
void vRegisterCLICommands() {
    FreeRTOS_CLIRegisterCommand(&get_version_cmd_def);
    FreeRTOS_CLIRegisterCommand(&mem_cmd_def);
    FreeRTOS_CLIRegisterCommand(&cli_cmd_def);
    FreeRTOS_CLIRegisterCommand(&ps_cmd_def);
    FreeRTOS_CLIRegisterCommand(&task_stats_cmd_def);
    FreeRTOS_CLIRegisterCommand(&get_portmask_cmd_def);
    FreeRTOS_CLIRegisterCommand(&set_portmask_cmd_def);
    FreeRTOS_CLIRegisterCommand(&save_settings_cmd_def);
    FreeRTOS_CLIRegisterCommand(&erase_flash_cmd_def);
    FreeRTOS_CLIRegisterCommand(&reset_cmd_def);
    FreeRTOS_CLIRegisterCommand(&set_baudrate_cmd_def);
    FreeRTOS_CLIRegisterCommand(&get_baudrate_cmd_def);
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
 * "CLI" command
 * Disables any NMEA output on USB
 */
static portBASE_TYPE cli_cmd(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    configASSERT(pcWriteBuffer);

    disable_usb_nmea = true;
    sprintf(pcWriteBuffer, "NMEA output on USB is disabled until NMEA Multiplexer is reset\r\n");
    
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
    const int8_t *const task_table_header = (int8_t *) "\r\nTask     \tState\tPri\tstack\t#\r\n\t\t\t\t(free)\r\n*******************************************\r\n";

    configASSERT(pcWriteBuffer);

    /* Generate a table of task stats. */
    strcpy((char *) pcWriteBuffer, (char *) task_table_header);
    vTaskList(pcWriteBuffer + strlen((char *) task_table_header));

    /* There is no more data to return after this single string, so return pdFALSE. */
    return pdFALSE;
}


/*******************************************************************************
 * "Get NMEA sentence list" command
 */
static portBASE_TYPE get_portmask_cmd(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    const char *parameter_string;
    portBASE_TYPE parameter_string_length;
    static char* nmea_str = NULL;
    static char* nmea_str_start = NULL;
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
        if (parameter_string != NULL) {
            port = atoi(parameter_string) - 1;
            if (port < 0 || port >= NMEA_PORT_TASK_NR_NMEA_PORTS) {
                sprintf(pcWriteBuffer, "Unknown port (ports 1->%d)\r\n", NMEA_PORT_TASK_NR_NMEA_PORTS);
                return pdFALSE;
            }
        }

        // Allocate NMEA string buffer
        nmea_str_start = (char*)pvPortMalloc(1024);
        nmea_str = nmea_str_start;
        nmea_tree_get_string(nmea_search_trees[port], nmea_str);

        if (strlen(nmea_str) == 0) {
            if (port == 5) {
                sprintf(pcWriteBuffer, "\n\rNMEA Port %d (BT) is not configured\r\n", port+1);
            } else if (port == 6) {
                sprintf(pcWriteBuffer, "\n\rNMEA Port %d (USB) is not configured\r\n", port+1);
            } else {
                sprintf(pcWriteBuffer, "\n\rNMEA Port %d (UART) is not configured\r\n", port+1);
            }            
            vPortFree(nmea_str_start);
            nmea_str_start = NULL;
            nmea_str = NULL;
            return pdFALSE;
        } else {
            if (port == 5) {
                sprintf(pcWriteBuffer, "\n\rNMEA Port %d (BT):\r\n", port+1);
            } else if (port == 6) {
                sprintf(pcWriteBuffer, "\n\rNMEA Port %d (USB):\r\n", port+1);
            } else {
                sprintf(pcWriteBuffer, "\n\rNMEA Port %d (UART):\r\n", port+1);
            }                
    //      cnt = 0;
            return pdTRUE;
        }           
    }

    // Copy string up to '='
    while (*nmea_str != '=') {
        nmea_type_str[index++] = *nmea_str;
        nmea_str++;
    }

    nmea_type_str[index] = 0x00;        // End of NMEA string

    nmea_str++;     // Skip '='

    // Copy string up to '='
    for (i = 0; i < 2; i++) {
        portmask_str[i] = (char)*nmea_str;
        nmea_str++;
    }
    nmea_str++;     // Skip ','
    portmask_str[2] = 0x00;
    portmask = strtol(portmask_str, NULL, 16);

    sprintf(pcWriteBuffer, "%s 0x%.2X\r\n", nmea_type_str, portmask);

    if (*nmea_str == 0x00) {
        vPortFree(nmea_str_start);
        nmea_str_start = NULL;
        nmea_str = NULL;
        return pdFALSE;
    } else {
        return pdPASS;
    }
}


/*******************************************************************************
 * "Set NMEA portmask" command
 */
static portBASE_TYPE set_portmask_cmd(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    const char *parameter_string;
    portBASE_TYPE parameter_string_len;
    uint8_t portmask;

    int port;
    const char* nmea_type_str_ptr;
    char nmea_type_str[10];

    configASSERT(pcWriteBuffer);

    /* Obtain the first parameter string. */
    parameter_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &parameter_string_len);

    /* Check parameter */
    if (parameter_string != NULL) {
        port = atoi(parameter_string) - 1;
//        printf("Port %d\r\n", port);
        if (port < 0 || port >= NMEA_PORT_TASK_NR_NMEA_PORTS) {
            sprintf(pcWriteBuffer, "Unknown port (ports 1->%d)\r\n", NMEA_PORT_TASK_NR_NMEA_PORTS);
            return pdFALSE;
        }
    } else {
        sprintf(pcWriteBuffer, "Port missing (ports 1->%d)\r\n", NMEA_PORT_TASK_NR_NMEA_PORTS);
        return pdFALSE;
    }

    /* Obtain the second parameter string. */
    nmea_type_str_ptr = FreeRTOS_CLIGetParameter(pcCommandString, 2, &parameter_string_len);
    strncpy(nmea_type_str, nmea_type_str_ptr, parameter_string_len);
    nmea_type_str[parameter_string_len] = 0x00;
    
//    printf("nmea_type_str %s (%d)\r\n", nmea_type_str, parameter_string_len);

    /* Check parameter */
    if (nmea_type_str == NULL) {
        sprintf(pcWriteBuffer, "\r\nNMEA sentence missing\r\n");
        return pdFALSE;
    }

    if (nmea_type_str[0] != '$') {
        sprintf(pcWriteBuffer, "\r\nNMEA sentence does not start with \'$\'\r\n");
        return pdFALSE;
    }

    /* Obtain the third parameter string. */
    parameter_string = FreeRTOS_CLIGetParameter(pcCommandString, 3, &parameter_string_len);

    /* Check parameter */
    if (parameter_string == NULL) {
        sprintf(pcWriteBuffer, "\r\nPort mask missing\r\n");
        return pdFALSE;
    }

    portmask = strtol(parameter_string, NULL, 16);

//    printf("set_portmask: %d, %s, 0x%.2X\r\n", port, nmea_type_str, portmask);

	nmea_tree_add(nmea_search_trees[port], (char*)nmea_type_str, portmask);

    sprintf(pcWriteBuffer, "\n\r\nNMEA port mask set\r\n");

//  printf("pcWriteBuffer: %s\r\n", temp_str);
//  sprintf(pcWriteBuffer, "%s 0x%.2X\r\n", temp_str, portmask);

    return pdFALSE;
}


/*******************************************************************************
 * "Get NMEA baudrate" command
 */
static portBASE_TYPE get_baudrate_cmd(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    static int port = 0;

    configASSERT(pcWriteBuffer);

    sprintf(pcWriteBuffer, "NMEA Port %d baudrate: %d\r\n", port+1, usart_baudrate[port]);

    if (port < CONF_NMEA_MUX_NR_NMEA_UART_PORTS-1) {
        port++;
        return pdTRUE;
    } else {
        port = 0;
        return pdFALSE;
    }
}


/*******************************************************************************
 * "Set NMEA UART baudrate" command
 */
static portBASE_TYPE set_baudrate_cmd(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    const char *parameter_string;
    portBASE_TYPE parameter_string_len;
    int baudrate;
     int port;
    const char* nmea_type_str_ptr;
    char nmea_type_str[10];

    configASSERT(pcWriteBuffer);

    /* Obtain the first parameter string. */
    parameter_string = FreeRTOS_CLIGetParameter(pcCommandString, 1, &parameter_string_len);

    /* Check parameter */
    if (parameter_string != NULL) {
        port = atoi(parameter_string) - 1;
//        printf("Port %d\r\n", port);
        if (port < 0 || port >= CONF_NMEA_MUX_NR_NMEA_UART_PORTS) {
            sprintf(pcWriteBuffer, "Unknown port (ports 1->%d)\r\n", CONF_NMEA_MUX_NR_NMEA_UART_PORTS);
            return pdFALSE;
        }
    } else {
        sprintf(pcWriteBuffer, "Port missing (ports 1->%d)\r\n", CONF_NMEA_MUX_NR_NMEA_UART_PORTS);
        return pdFALSE;
    }

    /* Obtain the second parameter string. */
    parameter_string = FreeRTOS_CLIGetParameter(pcCommandString, 2, &parameter_string_len);

    /* Check parameter */
    if (parameter_string == NULL) {
        sprintf(pcWriteBuffer, "\r\Baudrate missing\r\n");
        return pdFALSE;
    }

    baudrate = strtol(parameter_string, NULL, 10);
    usart_baudrate[port] = baudrate;

    printf("Setting baudrate on the fly\n\r");
    taskENTER_CRITICAL();
    usart_set_async_baudrate(p_usart_list[port], baudrate, sysclk_get_cpu_hz());
    taskEXIT_CRITICAL();
 
    sprintf(pcWriteBuffer, "\n\r\nNMEA Port %d baudrate set to %d\r\n", port+1, baudrate);

    printf("NMEA Port %d baudrate %d\n\r", port, baudrate);
//  sprintf(pcWriteBuffer, "%s 0x%.2X\r\n", temp_str, portmask);

    return pdFALSE;
}


/*******************************************************************************
 * "save settings" command
 */
static portBASE_TYPE save_settings_cmd(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    char* nmea_str;
    static int stage = 0;
    
    switch (stage) {
        case 0:
            sprintf(pcWriteBuffer, "Erasing Flash storage...");
            storage_erase_all();
            stage = 1;
            return pdTRUE;
        case 1:
            sprintf(pcWriteBuffer, "done\r\n");
            stage = 2;
            return pdTRUE;
        case 2:
            sprintf(pcWriteBuffer, "Writing NMEA config to Flash...");
            nmea_str = pvPortMalloc(FILE_MAX_SIZE);
            for (int i = 0; i < NMEA_PORT_TASK_NR_NMEA_PORTS; i++) {
                nmea_tree_get_string(nmea_search_trees[i], nmea_str);
                printf("Writing tree %d\r\n", i);
                storage_write_file(i, nmea_str);
            }
            vPortFree(nmea_str);
            stage = 3;
            return pdTRUE;
        case 3:
            sprintf(pcWriteBuffer, "done\r\n");
            stage = 4;
            return pdTRUE;
        case 4:
            sprintf(pcWriteBuffer, "Writing NMEA USART config to Flash...");
            nmea_str = pvPortMalloc(FILE_MAX_SIZE);
            get_usart_baudrate_str(nmea_str);
            printf("Writing NMEA USART config to Flash\r\n");
            storage_write_file(7, nmea_str);
            vPortFree(nmea_str);
            stage = 5;
            return pdTRUE;
        case 5:
            sprintf(pcWriteBuffer, "done\r\n");
            stage = 0;
        return pdFALSE;
    }

    return pdFALSE;
}


/*******************************************************************************
 * Erase Flash command
 */
static portBASE_TYPE erase_flash_cmd(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    static int stage = 0;
    
    switch (stage) {
        case 0:
            sprintf(pcWriteBuffer, "Erasing Flash storage...");
            storage_erase_all();
            stage++;
            return pdTRUE;
        case 1:
            sprintf(pcWriteBuffer, "done\r\n");
            stage = 0;
        return pdFALSE;
    }

    return pdFALSE;
}


/*******************************************************************************
 * Reset command
 */
static portBASE_TYPE reset_cmd(char *pcWriteBuffer, size_t xWriteBufferLen, const char *pcCommandString) {
    rstc_start_software_reset(RSTC);
    return pdFALSE;
}
/*
 * nmea_func.c
 *
 * Created: 2017-09-02 15:17:21
 *  Author: Max
 */

#include <asf.h>
#include <string.h>

/* FreeRTOS */
#include "FreeRTOS.h"
#include "queue.h"
#include "services/nmea_func.h"
#include "tasks/nmea_port_tasks.h"

// Global NMEA sentence search trees, one for each of the 7 NMEA Rx tasks
nmea_node_t* nmea_search_trees[NMEA_PORT_TASK_NR_NMEA_PORTS];


/************************************************************************************
 * nmea_create_node
 * Create an NMEA node for the search tree
 ************************************************************************************/
nmea_node_t* nmea_node_create(char character, uint8_t port_mask) {
  nmea_node_t* nmea_node;

  nmea_node = (nmea_node_t*)pvPortMalloc(sizeof(nmea_node_t));

  nmea_node->right     = NULL;
  nmea_node->down      = NULL;
  nmea_node->character = character;
  nmea_node->port_mask = port_mask;

  return nmea_node;
}


 /************************************************************************************
 * nmea_get_port_mask
 * Get Port Mask for a NMEA sentence in the search tree
 ************************************************************************************/
uint8_t nmea_get_port_mask(nmea_node_t* nmea_node, char* nmea_sentence) {
  while(nmea_node != NULL) {
    if (nmea_node->character == (char)*nmea_sentence) {
      // Check if it was the last valid character
      nmea_sentence++;
      if ((char)*nmea_sentence == 0x00 || (char)*nmea_sentence == ',') {
          return nmea_node->port_mask;
      } else {
          nmea_node = nmea_node->right;
      }
    } else {
      nmea_node = nmea_node->down;
    }
  }

  return 0xFF;
}


/************************************************************************************
 * nmea_del_list
 * Delete (free memory) the linked list of all NMEA sentences and Port Masks
 ************************************************************************************/
void nmea_del_list(nmea_str_node_t** nmea_str_node_ref) {
  nmea_str_node_t* curr_node;
  nmea_str_node_t* next_node;

  curr_node = *nmea_str_node_ref;
//  printf("curr_node 0x%.8X\n", curr_node);

  while (curr_node != NULL) {
    next_node = curr_node->next;
    vPortFree(curr_node->str);      // De-allocate string memory
    vPortFree(curr_node);           // De-allocate node memory
    curr_node = next_node;
  }

  // Set reference to NULL
  *nmea_str_node_ref = (nmea_str_node_t*)NULL;
}

void nmea_tree_get_string_rec(nmea_node_t* nmea_node, char* nmea_str, char* nmea_type_str);

/************************************************************************************
 * nmea_tree_get_string
 * Get a string of all NMEA sentences and Port Masks from the NMEA search tree
 ************************************************************************************/
int nmea_tree_get_string(nmea_node_t* nmea_node, char* nmea_str) {
  char nmea_type_str[10];

  nmea_type_str[0] = 0x00;  // Make empty string
  nmea_str[0] = 0x00;       // Make empty string

  taskENTER_CRITICAL();
  nmea_tree_get_string_rec(nmea_node, nmea_str, nmea_type_str);
  taskEXIT_CRITICAL();
  return 1;
}



/* Recursive help function for nmea_tree_get_string */
void nmea_tree_get_string_rec(nmea_node_t* nmea_node, char* nmea_str, char* nmea_type_str) {
    int nmea_type_str_len;

    // Get current length of NMEA type string (so far)
    nmea_type_str_len = strlen(nmea_type_str);

    while (nmea_node != NULL) {
        // Check if last character in NMEA type string is found
        if (nmea_node->right == NULL) {
            // Append NMEA type string
            sprintf(nmea_type_str + nmea_type_str_len, "%c=%.2X,", nmea_node->character, nmea_node->port_mask);
            strcat(nmea_str, nmea_type_str);
//          printf("after append: %s\r\n", nmea_str);
        } else {
            // Add character to the temporary NMEA type string
            sprintf(nmea_type_str + nmea_type_str_len, "%c", nmea_node->character);
            nmea_tree_get_string_rec(nmea_node->right, nmea_str, nmea_type_str);
        }

        // Next NMEA type string
        nmea_node = nmea_node->down;
    }
}


/************************************************************************************
 * nmea_tree_init
 * Initialize NMEA Search Tree
 ************************************************************************************/
nmea_node_t* nmea_tree_init(const char* nmea_tree_string) {
    char nmea_type[6];
    uint8_t port_mask;
    int index = 0;
    nmea_node_t* first_node;
        
    // Create first 'dummy' node
    first_node = nmea_node_create('$', 0x00);
    
    while((char)*nmea_tree_string != 0x00) {
        index = 0;
        while((char)*nmea_tree_string != '=') {
            nmea_type[index++] = (char)*nmea_tree_string;
//            printf("type %c\r\n", (char)*nmea_tree_string);
            nmea_tree_string++;
        }
        nmea_tree_string++;
        
        nmea_type[index] = 0x00;
        
//        printf("nmea_type: %s ", nmea_type);
        
        port_mask = 0x00;
        for (int i = 0; i < 2; i++) {
            if ((char)*nmea_tree_string >= '0' && (char)*nmea_tree_string <= '9') {
                port_mask = (port_mask << 4) | ((char)*nmea_tree_string - '0');
//                printf("0-9: %c\n\r", (char)*nmea_tree_string);
            } else {
                port_mask = (port_mask << 4) | ((char)*nmea_tree_string - 'F' + 15);
//                printf("A-F: \'%c\' %d (0x%x)\n\r", (char)*nmea_tree_string, (char)*nmea_tree_string, ((char)*nmea_tree_string - 'F' + 15));
            }        
            nmea_tree_string++;
       }                
       nmea_tree_string++;  // Get rid of ';'
            
//       printf("Port_mask 0x%X\r\n", port_mask);
       nmea_tree_add(first_node, nmea_type, port_mask);
    }
    return first_node;
}


/************************************************************************************
 * nmea_tree_add
 * Add a NMEA sentence and Port Mask to the NMEA search tree
 ************************************************************************************/
int nmea_tree_add(nmea_node_t* curr_node, char* nmea_string, uint8_t port_mask) {
    nmea_node_t* new_node;

    printf("Insert NMEA \'%s\' 0x%.2X %p\r\n", nmea_string, port_mask, curr_node);

    taskENTER_CRITICAL();

    // Loop through all characters
    for (;;) {
        // If character is the same, the step to the right
//      printf("curr_node->character %c, (char)*nmea_string %c\r\n", curr_node->character, (char)*nmea_string);
        if (curr_node->character == (char)*nmea_string) {
            nmea_string++;
            // If the complete NMEA sentence already exists then replace port-mask
            if ((char)*nmea_string == 0x00 || (char)*nmea_string == ',') {
                printf("Exists\n\r");
                curr_node->port_mask = port_mask;
                taskEXIT_CRITICAL();
                return 0;
            } else {
                if (curr_node->right != NULL && curr_node->right->character > (char)*nmea_string) {
                    // Insert current NMEA character
                    new_node = nmea_node_create((char)*(nmea_string), 0x00);
                    new_node->down = curr_node->right;
                    curr_node->right = new_node;
                    curr_node = new_node;
                    nmea_string++;
                    break;
                } else if (curr_node->right == NULL) {
                    new_node = nmea_node_create((char)*(nmea_string), 0x00);
                    curr_node->right = new_node;
                    curr_node = new_node;
                    nmea_string++;
                    break;       
                } else {
//                  printf("go right\r\n");
                    curr_node = curr_node->right;
                    continue;  // Next character to the right
                }
            }
        }

        // If the character in the next node is 'smaller' than
        //  the NMEA character, then continue down
        if (curr_node->down == NULL || curr_node->down->character > (char)*nmea_string) {
            // Insert current NMEA character
            new_node = nmea_node_create((char)*(nmea_string), 0x00);
            new_node->down = curr_node->down;
            curr_node->down = new_node;
            curr_node = new_node;
            nmea_string++;

            break;  // Reached the end
        }

        curr_node = curr_node->down;  // Next character down
    }

    // Add the rest of the NMEA sentence
    while ((char)*nmea_string != 0x00 && (char)*nmea_string != ',') {
        new_node = nmea_node_create((char)*(nmea_string), 0x00);
        curr_node->right = new_node;
        curr_node = new_node;
        nmea_string++;
    }

    // Set port_mask in leaf node
    curr_node->port_mask = port_mask;
    taskEXIT_CRITICAL();

    return 1;
}

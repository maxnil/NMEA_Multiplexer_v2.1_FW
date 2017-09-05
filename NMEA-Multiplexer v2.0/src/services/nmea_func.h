/*
 * nmea_func.h
 *
 * Created: 2017-09-02 15:17:10
 *  Author: Max
 */ 


#ifndef NMEA_FUNC_H_
#define NMEA_FUNC_H_

/************************************************************************************
 * nmea_node struct
 * NMEA sentence search tree node
 ************************************************************************************/
typedef struct _nmea_node_t {
  struct _nmea_node_t* right;
  struct _nmea_node_t* down;    
  char    character;
  uint8_t port_mask;
} nmea_node_t;


/************************************************************************************
 * nmea_str_node struct
 * NMEA sentence list node
 ************************************************************************************/
typedef struct _nmea_str_node_t {
  struct _nmea_str_node_t* next;
  char* str;
  uint8_t port_mask;
} nmea_str_node_t;


/************************************************************************************
 * nmea_create_node
 * Create an NMEA node for the search tree
 ************************************************************************************/
nmea_node_t* nmea_node_create(char character, uint8_t port_mask);


/************************************************************************************
 * nmea_get_port_mask
 * Get Port Mask for a NMEA sentence in the search tree
 ************************************************************************************/
uint8_t nmea_get_port_mask(nmea_node_t* nmea_node, char* nmea_sentence);


/************************************************************************************
 * nmea_del_list
 * Delete (free memory) the linked list of all NMEA sentences and Port Masks
 ************************************************************************************/
void nmea_del_list(nmea_str_node_t** nmea_str_node_ref);


/************************************************************************************
 * nmea_tree_get_list
 * Get a linked list of all NMEA sentences and Port Masks from the NMEA search tree
 ************************************************************************************/
nmea_str_node_t* nmea_tree_get_list(nmea_node_t* nmea_node);


/************************************************************************************
 * nmea_tree_init
 * Initialize NMEA Search Tree
 ************************************************************************************/
nmea_node_t* nmea_tree_init(void);


/************************************************************************************
 * nmea_tree_add
 * Add a NMEA sentence and Port Mask to the NMEA search tree
 ************************************************************************************/
int nmea_tree_add(nmea_node_t* curr_node, char* nmea_string, uint8_t port_mask);

#endif /* NMEA_FUNC_H_ */
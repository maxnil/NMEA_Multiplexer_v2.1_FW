/*
 * usart_setting.h
 *
 * Created: 2021-10-11 22:43:22
 *  Author: max
 */ 


#ifndef USART_SETTING_H_
#define USART_SETTING_H_

extern int usart_baudrate[5];

extern Usart *p_usart_list[5];

void init_nmea_port_usart(Usart *p_usart, int baudrate);

void update_usart_baudrate_from_str(char* str);

void get_usart_baudrate_str(char* buf);

#endif /* USART_SETTING_H_ */
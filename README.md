# NMEA_Multiplexer_v2.1_FW
<b>NMEA Multiplexer version 2.1</b>

This repository contains the firmware for the NMEA Multiplexer v2.1

Features:\
5x NMEA ports (programmable baud-rate, RS232)\
1x Debug serial port (115200 baud)\
1x BlueTooth module (HM-11)\
1x USB port (can funciton both as an additional NMEA Tx port, or as an Comman Line Interface for configuration of the NMEA-Multiplexer

It captures incomming NMEA-0183 messages on any of the 5 in-port (NMEA RS232 Port 1-5) and sends each message to one or more of the 7 out-ports (NMEA RS232 Port 1-5, BlueTooth or USB port). The message can also be ignore.

Configuration can be saved to internal Flash memory.

The microcontroller used is an Atmel SAMG55J.

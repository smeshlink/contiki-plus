/*
 * Copyright (c) 2012, SmeshLink Technology Ltd.
 * All rights reserved.
 *
 */

#ifndef __CONTIKI_MAIN_H__
#define __CONTIKI_MAIN_H__

extern unsigned char arduino_node_id[8];
extern uint8_t arduino_channel_id;
extern uint8_t arduino_power_id;

int start_contiki(void (*setup) (void), void (*run) (void));

#endif /* __CONTIKI_MAIN_H__ */

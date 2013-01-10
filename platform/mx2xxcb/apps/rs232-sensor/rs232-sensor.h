/*
 * Copyright (c) 2011-2012 SmeshLink Technology Corporation.
 * All rights reserved.
 *
 * $Id: rs232-sensor.h $
 */

/**
 * \file
 *         Provides support methods for sensors using RS232.
 * \author
 *         SmeshLink
 */

#ifndef __RS232_SENSOR_H__
#define __RS232_SENSOR_H__

#include "dev/rs232.h"

#define RS232_SENSOR_FRAME_LENGTH 32

struct rs232_frame_t {
  u16_t ndx;
  u8_t frame[RS232_SENSOR_FRAME_LENGTH];
  u8_t done;
};

extern struct rs232_frame_t rs232_frame;

void rs232_sensor_init(uint8_t port, uint8_t bd, uint8_t ffmt);
void rs232_sensor_set_input(int (*f)(unsigned char));
void rs232_sensor_set_frame_length(uint16_t len);
void rs232_sensor_reset();
void rs232_sensor_send(unsigned char c);
void rs232_sensor_print(char *buf);
int rs232_sensor_debug(char* buf);

#endif /* __RS232_SENSOR_H__ */

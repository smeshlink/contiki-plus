/*
 * Copyright (c) 2011-2012 SmeshLink Technology Corporation.
 * All rights reserved.
 *
 * $Id: rs232-sensor.c $
 */

/**
 * \file
 *         Provides support methods for sensors using RS232.
 * \author
 *         SmeshLink
 */

#include "rs232-sensor.h"

struct rs232_frame_t rs232_frame;

static uint8_t rs232_sensor_port;
static uint16_t rs232_sensor_frame_length;

static int
rs232_sensor_serial_input(unsigned char ch)
{
  if (rs232_frame.ndx >= rs232_sensor_frame_length) {
    rs232_frame.ndx = 0;
  }
  rs232_frame.frame[rs232_frame.ndx++] = ch;
  if (rs232_frame.ndx >= rs232_sensor_frame_length) {
    rs232_frame.done = 1;
  }
  return 0;
}

void
rs232_sensor_init(uint8_t port, uint8_t bd, uint8_t ffmt)
{
  rs232_sensor_port = port;
  rs232_init(port, bd, ffmt);
  rs232_set_input(port, rs232_sensor_serial_input);
}

void
rs232_sensor_set_input(int (*f)(unsigned char))
{
  rs232_set_input(rs232_sensor_port, f);
}

void
rs232_sensor_set_frame_length(uint16_t len)
{
  rs232_sensor_frame_length = len;
}

void
rs232_sensor_reset()
{
  rs232_frame.done = 0;
}

void
rs232_sensor_send(unsigned char c)
{
  rs232_send(rs232_sensor_port, c);
}

void
rs232_sensor_print(char *buf)
{
  while(*buf) {
    rs232_send(rs232_sensor_port, *buf++);
  }
}

int
rs232_sensor_debug(char* buf)
{
  int index = 0, i = 0;
  for (i = 0; i < rs232_sensor_frame_length; i++) {
    index += sprintf(buf + index, " %02x", rs232_frame.frame[i]);
  }
  return index;
}

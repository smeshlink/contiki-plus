/*
 * Copyright (c) 2011-2012 SmeshLink Technology Corporation.
 * All rights reserved.
 *
 * $Id: co2.h $
 */

/**
 * \file
 *         A custom CO2 sensor using RS232.
 * \author
 *         SmeshLink
 */

#ifndef __CO2_H__
#define __CO2_H__

#include "rs232-sensor.h"

void co2_init();
uint16_t co2_get();
int co2_debug(char* buf);

#endif /* __CO2_H__ */

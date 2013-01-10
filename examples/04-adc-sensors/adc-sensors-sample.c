/*
 * Copyright (c) 2011-2012 SmeshLink Technology Corporation.
 * All rights reserved.
 *
 * $Id: adc-sensors-sample.c $
 */

/**
 * \file
 *         This application shows how read data of temperature and light
 *         using ADC sensors interfaces.
 * \author
 *         SmeshLink
 */

#include "contiki.h"

#include "dev/adc-sensors.h"

#include <stdio.h>

/*---------------------------------------------------------------------------*/
PROCESS(sensors_sample_process, "Sensors sample process");
AUTOSTART_PROCESSES(&sensors_sample_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(sensors_sample_process, ev, data)
{
  /* Variables are declared static to ensure their values are kept between kernel calls. */
  static struct etimer timer;

  /* Any process must start with this. */
  PROCESS_BEGIN();

  /* Set the etimer to generate an event in one second. */
  etimer_set(&timer, CLOCK_CONF_SECOND);

  while(1) {
    /* Wait for an event. */
    PROCESS_WAIT_EVENT();

    /* Got the timer's event~ */
    if (ev == PROCESS_EVENT_TIMER) {
      /* Read raw temperature value. */
      int16_t temp_raw = sensor_temp_get_raw();
      /* Read temperature value in specified unit (C/F). */
      int16_t temp = sensor_temp_get(TEMP_UNIT_CELCIUS);
      /* Read raw light value. */
      int16_t light_raw = sensor_light_get_raw();

      printf("temp_raw:%d\ntemp:%d\nlight_raw:%d", temp_raw, temp, light_raw);

      /* Reset the etimer so it will generate another event after the exact same time. */
      etimer_reset(&timer);
    }
  } // while (1)

  /* Any process must end with this, even if it is never reached. */
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

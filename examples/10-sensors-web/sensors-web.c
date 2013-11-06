/*
 * Copyright (c) 2011-2012 SmeshLink Technology Corporation.
 * All rights reserved.
 *
 * $Id: sensors-web.c $
 */

/**
 * \file
 *         A web interface for accessing sensor data including light, temperature,
 *         voltage, ds2411x, etc.
 * \author
 *         SmeshLink
 */

#include "contiki.h"
#include "contiki-net.h"
#include "rest.h"

#include "dev/leds.h"
#include "dev/adc-sensors.h"
#include "dev/ds2401.h"

#include <stdio.h>

/*---------------------------------------------------------------------------*/
PROCESS(rest_example, "Sensors Web");
AUTOSTART_PROCESSES(&rest_example);
/*---------------------------------------------------------------------------*/
char temp[100];

RESOURCE(sensors, METHOD_GET, "sensors");
void
sensors_handler(REQUEST* request, RESPONSE* response)
{
  int index = 0, ii;
  int16_t light, temperature;
  double voltage;

  light = sensor_light_get();
  temperature = sensor_temp_get(TEMP_UNIT_CELCIUS);
  voltage = voltage_get();

  index += sprintf(temp + index, "Light: %d\n", light);
  index += sprintf(temp + index, "Temperature: %d C\n", temperature);
  index += sprintf(temp + index, "Voltage: %d mv\n", (int)(voltage * (double)1000));

  ds2401_init();
  index += sprintf(temp + index, "ds2401:");
  for (ii = 0; ii < 8; ii++) {
    index += sprintf(temp + index, " %02x", ds2401_id[ii]);
  }

  rest_set_header_content_type(response, TEXT_PLAIN);
  rest_set_response_payload(response, temp, strlen(temp));

}

RESOURCE(discover, METHOD_GET, ".well-known/core");
void
discover_handler(REQUEST* request, RESPONSE* response)
{
  int index = 0;

  index += sprintf(temp + index, "%s,", "</sensors>;n=\"sensors\"");

  rest_set_response_payload(response, temp, strlen(temp));
  rest_set_header_content_type(response, APPLICATION_LINK_FORMAT);
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(rest_example, ev, data)
{
  PROCESS_BEGIN();

  rest_init();
  rest_activate_resource(&resource_sensors);
  rest_activate_resource(&resource_discover);

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

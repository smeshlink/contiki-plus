/*
 * Copyright (c) 2011-2012 SmeshLink Technology Corporation.
 * All rights reserved.
 *
 * $Id: co2-sensor.c $
 */

/**
 * \file
 *         This application demonstrates a custom CO2 sensor.
 * \author
 *         SmeshLink
 */

#include "contiki.h"
#include "contiki-net.h"

#include "erbium.h"

#include "co2.h"

#include <stdio.h>
#include <string.h>

/* For CoAP-specific example: not required for normal RESTful Web service. */
#if WITH_COAP == 3
#include "er-coap-03.h"
#define CONTENT_2_05 OK_200
#elif WITH_COAP == 7
#include "er-coap-07.h"
#else
#warning "Erbium example without CoAP-specifc functionality"
#endif /* CoAP-specific example */

/*---------------------------------------------------------------------------*/
char temp[100];

PERIODIC_RESOURCE(co2, METHOD_GET, "co2", "title=\"Periodic temperature\";rt=\"Observable\"", 2*CLOCK_SECOND);
void
co2_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  char buf[32];
  uint16_t co2 = co2_get();
  sprintf(buf, "%u", co2);
  REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
  REST.set_response_payload(response, (uint8_t *)buf, strlen(buf));
}
void
co2_periodic_handler(resource_t *r)
{
  static uint32_t obs_counter = 0;
  char content[32];

  obs_counter++;

  uint16_t co2 = co2_get();

  /* Build notification. */
  coap_packet_t notification[1]; /* This way the packet can be treated as pointer as usual. */
  coap_init_message(notification, COAP_TYPE_NON, CONTENT_2_05, 0 );
  coap_set_payload(notification, content, snprintf(content, sizeof(content), "%u", co2));

  /* Notify the registered observers with the given message type, observe option, and payload. */
  REST.notify_subscribers(r, obs_counter, notification);
}
/*---------------------------------------------------------------------------*/
PROCESS(coap_sample, "CoAP Sample");
AUTOSTART_PROCESSES(&coap_sample);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(coap_sample, ev, data)
{
  PROCESS_BEGIN();
  //rs232_redirect_stdout(RS232_PORT_1);

  co2_init();

  /* Initialize the REST engine. */
  rest_init_engine();

  /* Activate the application-specific resources. */
  rest_activate_periodic_resource(&periodic_resource_co2);

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

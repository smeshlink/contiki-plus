/*
 * Copyright (c) 2011-2012 SmeshLink Technology Corporation.
 * All rights reserved.
 *
 * $Id: camera-observe.c $
 */

/**
 * \file
 *         A RESTful server example showing how to use the REST layer
 *         to develop server-side applications.
 *
 * \author
 *         SmeshLink
 */

#include "contiki.h"
#include "contiki-net.h"

#include "erbium.h"

#include "dev/leds.h"
#include "dev/adc-sensors.h"
#include "dev/interrupt.h"
#include "camera.h"

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

PERIODIC_RESOURCE(temperature, METHOD_GET, "temperature", "title=\"Periodic temperature\";rt=\"Observable\"", 2*CLOCK_SECOND);
void
temperature_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  char buf[32];
  int16_t temp = sensor_temp_get(TEMP_UNIT_CELCIUS);
  sprintf(buf, "%d", temp);
  REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
  REST.set_response_payload(response, (uint8_t *)buf, strlen(buf));
}
void
temperature_periodic_handler(resource_t *r)
{
  static uint32_t obs_counter = 0;
  char content[32];

  obs_counter++;

  int16_t temp = sensor_temp_get(TEMP_UNIT_CELCIUS);

  /* Build notification. */
  coap_packet_t notification[1]; /* This way the packet can be treated as pointer as usual. */
  coap_init_message(notification, COAP_TYPE_NON, CONTENT_2_05, 0 );
  coap_set_payload(notification, content, snprintf(content, sizeof(content), "%u", temp));

  /* Notify the registered observers with the given message type, observe option, and payload. */
  REST.notify_subscribers(r, obs_counter, notification);
}

/*---------------------------------------------------------------------------*/
PERIODIC_RESOURCE(light, METHOD_GET, "light", "title=\"Periodic light\";rt=\"Observable\"", 1*CLOCK_SECOND);
void
light_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  char buf[32];
  int16_t light = sensor_light_get();
  sprintf(buf, "%d", light);
  REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
  REST.set_response_payload(response, (uint8_t *)buf, strlen(buf));
}
void
light_periodic_handler(resource_t *r)
{
  static uint32_t obs_counter = 0;
  char content[32];

  obs_counter++;

  int16_t light = sensor_light_get();

  /* Build notification. */
  coap_packet_t notification[1]; /* This way the packet can be treated as pointer as usual. */
  coap_init_message(notification, COAP_TYPE_NON, CONTENT_2_05, 0 );
  coap_set_payload(notification, content, snprintf(content, sizeof(content), "%u", light));

  /* Notify the registered observers with the given message type, observe option, and payload. */
  REST.notify_subscribers(r, obs_counter, notification);
}

/*---------------------------------------------------------------------------*/
PERIODIC_RESOURCE(voltage, METHOD_GET, "voltage", "title=\"Periodic voltage\";rt=\"Observable\"", 1*CLOCK_SECOND);
void
voltage_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  char buf[32];
  double voltage = voltage_get();
  sprintf(buf, "%d", (int)(voltage * (double)1000));
  REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
  REST.set_response_payload(response, (uint8_t *)buf, strlen(buf));
}
void
voltage_periodic_handler(resource_t *r)
{
  static uint32_t obs_counter = 0;
  char content[32];

  obs_counter++;

  double voltage = voltage_get();

  /* Build notification. */
  coap_packet_t notification[1]; /* This way the packet can be treated as pointer as usual. */
  coap_init_message(notification, COAP_TYPE_NON, CONTENT_2_05, 0 );
  coap_set_payload(notification, content, snprintf(content, sizeof(content), "%d", (int)(voltage * (double)1000)));

  /* Notify the registered observers with the given message type, observe option, and payload. */
  REST.notify_subscribers(r, obs_counter, notification);
}

/*---------------------------------------------------------------------------*/
RESOURCE(take, METHOD_GET | METHOD_POST, "take", "take");
/*
 * Take a picture.
 */
void
take_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  const char *temp;
  char buf[64] = "";
  camera_size_t camera_size = CAMERA_SIZE_1;
  uint16_t count;
  uint32_t size;

  /* Determine the size of picture. 1 for the smallest and 3 for the largest. */
  if (!REST.get_query_variable(request, "size", &temp)) {
    REST.get_post_variable(request, "size", &temp);
  }

  if (temp) {
    camera_size = camera_parse_size(temp[0]);
  }

  camera_take_picture(camera_size, DEFAULT_CAMERA_PACKET_SIZE, &size, &count);
  sprintf(buf, "{'t':'OK','s':%lu,'n':%u}", size, count);

  REST.set_header_content_type(response, TEXT_PLAIN);
  REST.set_response_payload(response, buf, strlen(buf));
}

RESOURCE(size, METHOD_GET, "size", "size");
/*
 * Get the size of the taken picture.
 */
void
size_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  char temp[64] = "";

  uint32_t size = camera_get_picture_size();
  sprintf(temp, "%lu", size);

  REST.set_header_content_type(response, TEXT_PLAIN);
  REST.set_response_payload(response, temp, strlen(temp));
}

RESOURCE(count, METHOD_GET, "count", "count");
/*
 * Get the total count of the picture's packets.
 */
void
count_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  char temp[64] = "";

  uint16_t count = camera_get_packet_count();
  sprintf(temp, "%u", count);

  REST.set_header_content_type(response, TEXT_PLAIN);
  REST.set_response_payload(response, temp, strlen(temp));
}

RESOURCE(packet, METHOD_GET | METHOD_PUT | METHOD_POST, "packet", "packet");
/*
 * Get one packet of the picture.
 */
void
packet_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  const char *temp;
  char buf[1024];
  int no = 0, len = 0, tries = 0;

  /* Get the index of the specified packet. */
  len = REST.get_query_variable(request, "no", &temp);
  if (!len) {
    len = REST.get_post_variable(request, "no", &temp);
  }
  memcpy(buf, temp, len);
  buf[len] = '\0';
  no = atoi(buf);

  /* Get count of tries. Unused right now. */
  len = REST.get_query_variable(request, "try", &temp);
  if (!len) {
    len = REST.get_post_variable(request, "try", &temp);
  }
  if (len) {
    memcpy(buf, temp, len);
    buf[len] = '\0';
    tries = atoi(buf);
  }
  if (tries <= 0) {
    tries = 10;
  }
  len = camera_try_get_packet(no, buf, tries);
  REST.set_response_payload(response, buf, len);
  REST.set_header_content_type(response, TEXT_PLAIN);
}

RESOURCE(camera, METHOD_GET | METHOD_POST, "camera", "title=\"camera\"");
void
camera_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  const char *temp;
  uint8_t buff[1024];
  camera_size_t camera_size = CAMERA_SIZE_1;
  u16_t count;
  u32_t size;
  int index = 0, len = 0, tries = 10;

  if (0 == *offset) {
    /* Determine the size of picture. 1 for the smallest and 3 for the largest. */
    if (!REST.get_query_variable(request, "size", &temp)) {
      REST.get_post_variable(request, "size", &temp);
    }
    if (temp) {
      camera_size = camera_parse_size(temp[0]);
    }
    leds_toggle(LEDS_GREEN);
    camera_take_picture(camera_size, preferred_size, &size, &count);
  }

  index = *offset / preferred_size + 1;
  count = camera_get_packet_count();
  len = camera_try_get_packet(index, buff, tries);
  *offset += len;
  if (index >= count) {
    *offset = -1;
  }
  REST.set_header_content_type(response, REST.type.IMAGE_JPEG);
  REST.set_response_payload(response, buff, len);
}

EVENT_RESOURCE(camera_trigger, METHOD_GET | METHOD_POST, "camera/trigger", "title=\"camera trigger\";obs");
void
camera_trigger_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
  /* Usually, a CoAP server would response with the current resource representation. */
  const char *msg = "It's eventful!";
  REST.set_response_payload(response, (uint8_t *)msg, strlen(msg));
}
void
camera_trigger_event_handler(resource_t *r)
{
  static uint16_t event_counter = 0;
  static char content[12];

  ++event_counter;
  /* Build notification. */
  coap_packet_t notification[1]; /* This way the packet can be treated as pointer as usual. */
  coap_init_message(notification, COAP_TYPE_CON, CONTENT_2_05, 0 );
  coap_set_payload(notification, content, snprintf(content, sizeof(content), "EVENT %u", event_counter));

  /* Notify the registered observers with the given message type, observe option, and payload. */
  REST.notify_subscribers(r, event_counter, notification);
}

/*---------------------------------------------------------------------------*/
#if UIP_CONF_IPV6_RPL
#include <avr/pgmspace.h>
#include "rpl.h"

extern uip_ds6_nbr_t uip_ds6_nbr_cache[];
extern uip_ds6_route_t uip_ds6_routing_table[];

static char buf[512];
static uint8_t blen;
#define ADD(FORMAT,args...) do {                                                 \
    blen += snprintf_P(&buf[blen], sizeof(buf) - blen, PSTR(FORMAT),##args);      \
  } while(0)

static void
ipaddr_add(const uip_ipaddr_t *addr)
{
  uint16_t a;
  int i, f;
  for(i = 0, f = 0; i < sizeof(uip_ipaddr_t); i += 2) {
    a = (addr->u8[i] << 8) + addr->u8[i + 1];
    if(a == 0 && f >= 0) {
      if(f++ == 0 && sizeof(buf) - blen >= 2) {
        buf[blen++] = ':';
        buf[blen++] = ':';
      }
    } else {
      if(f > 0) {
        f = -1;
      } else if(i > 0 && blen < sizeof(buf)) {
        buf[blen++] = ':';
      }
      ADD("%x", a);
    }
  }
}

RESOURCE(route, METHOD_GET | METHOD_POST, "route", "route");
void
route_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  int i;
  blen = 0;
  ADD("Routes [%u max]\n\r", UIP_DS6_ROUTE_NB);
  for(i = 0; i < UIP_DS6_ROUTE_NB; i++) {
    if(uip_ds6_routing_table[i].isused) {
      ipaddr_add(&uip_ds6_routing_table[i].ipaddr);
      ADD("/%u (via ", uip_ds6_routing_table[i].length);
      ipaddr_add(&uip_ds6_routing_table[i].nexthop);
      if(uip_ds6_routing_table[i].state.lifetime < 600) {
        ADD(") %lus\n\r", uip_ds6_routing_table[i].state.lifetime);
      } else {
        ADD(")\n\r");
      }
    }
  }
  REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
  REST.set_response_payload(response, (uint8_t *)buf, strlen(buf));
}

#endif

/*---------------------------------------------------------------------------*/
PROCESS(coap_sample, "CoAP Sample");
AUTOSTART_PROCESSES(&coap_sample);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(coap_sample, ev, data)
{
  PROCESS_BEGIN();

  /* Initialize the REST engine. */
  rest_init_engine();

  /* Activate the application-specific resources. */
  rest_activate_resource(&resource_camera);
  rest_activate_event_resource(&resource_camera_trigger);
  rest_activate_resource(&resource_take);
  rest_activate_resource(&resource_size);
  rest_activate_resource(&resource_count);
  rest_activate_resource(&resource_packet);
#if UIP_CONF_IPV6_RPL
  rest_activate_resource(&resource_route);
#endif
  rest_activate_periodic_resource(&periodic_resource_temperature);
  rest_activate_periodic_resource(&periodic_resource_light);
  rest_activate_periodic_resource(&periodic_resource_voltage);

  interrupt_init(1, 1, 1, 1);
  interrupt_enable(INT0);
  interrupt_register(INT0);

  while(1) {
     /* Wait for an event. */
     PROCESS_WAIT_EVENT();

     /* Got the interrupt event~ */
     if (ev == PROCESS_EVENT_INTERRUPT) {
       /* Check for the int_vect. */
       if (INT0 == ((struct interrupt *)data)->int_vect) {
         /* Got an INT0 interrupt. */
         leds_toggle(LEDS_GREEN);
         camera_trigger_event_handler(&resource_camera_trigger);
       }
     }
   } // while (1)

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

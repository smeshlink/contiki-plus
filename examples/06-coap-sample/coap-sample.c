/*
 * Copyright (c) 2011-2012 SmeshLink Technology Corporation.
 * All rights reserved.
 *
 * $Id: coap-sample.c $
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
#include "camera.h"

#include <stdio.h>
#include <stdlib.h>
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
//char temp[100];

PERIODIC_RESOURCE(temperature, METHOD_GET, "temperature", "title=\"Periodic temperature\";rt=\"Observable\"", 2*CLOCK_SECOND);
void
temperature_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
	char tbuf[32];
	int16_t temp = sensor_temp_get(TEMP_UNIT_CELCIUS);
	sprintf(tbuf, "%d", temp);
	REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
	REST.set_response_payload(response, (uint8_t *)tbuf, strlen(tbuf));
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
	char tbuf[32];
	int16_t light = sensor_light_get();
	sprintf(tbuf, "%d", light);
	REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
	REST.set_response_payload(response, (uint8_t *)tbuf, strlen(tbuf));
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
	char tbuf[32];
	double voltage = voltage_get();
	sprintf(tbuf, "%d", (int)(voltage * (double)1000));
	REST.set_header_content_type(response, REST.type.TEXT_PLAIN);
	REST.set_response_payload(response, (uint8_t *)tbuf, strlen(tbuf));
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
	char tbuf[32] = "";
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
	sprintf(tbuf, "{'t':'OK','s':%lu,'n':%u}", size, count);

	REST.set_header_content_type(response, TEXT_PLAIN);
	REST.set_response_payload(response, tbuf, strlen(tbuf));
}

RESOURCE(size, METHOD_GET, "size", "size");
/*
 * Get the size of the taken picture.
 */
void
size_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
	char tbuf[32] = "";

	uint32_t size = camera_get_picture_size();
	sprintf(tbuf, "%lu", size);

	REST.set_header_content_type(response, TEXT_PLAIN);
	REST.set_response_payload(response, tbuf, strlen(tbuf));
}

RESOURCE(count, METHOD_GET, "count", "count");
/*
 * Get the total count of the picture's packets.
 */
void
count_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
	char tbuf[32] = "";

	uint16_t count = camera_get_packet_count();
	sprintf(tbuf, "%u", count);

	REST.set_header_content_type(response, TEXT_PLAIN);
	REST.set_response_payload(response, tbuf, strlen(tbuf));
}

RESOURCE(packet, METHOD_GET | METHOD_PUT | METHOD_POST, "packet", "packet");
/*
 * Get one packet of the picture.
 */
void
packet_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
	const char *temp;
	char tbuf[128];
	int no = 0, len = 0, tries = 0;

	/* Get the index of the specified packet. */
	len = REST.get_query_variable(request, "no", &temp);
	if (!len) {
		len = REST.get_post_variable(request, "no", &temp);
	}
	memcpy(tbuf, temp, len);
	tbuf[len] = '\0';
	no = atoi(tbuf);

	/* Get count of tries. Unused right now. */
	len = REST.get_query_variable(request, "try", &temp);
	if (!len) {
		len = REST.get_post_variable(request, "try", &temp);
	}
	if (len) {
		memcpy(tbuf, temp, len);
		tbuf[len] = '\0';
		tries = atoi(tbuf);
	}
	if (tries <= 0) {
		tries = 10;
	}
	len = camera_try_get_packet(no, tbuf, tries);
	REST.set_response_payload(response, tbuf, len);
	REST.set_header_content_type(response, TEXT_PLAIN);
}

RESOURCE(camera, METHOD_GET | METHOD_POST, "camera", "camera");
void
camera_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
	const char *temp;
	uint8_t tbuf[128];
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
	len = camera_try_get_packet(index, tbuf, tries);
	*offset += len;
	if (index >= count) {
		*offset = -1;
	}
	REST.set_header_content_type(response, REST.type.IMAGE_JPEG);
	REST.set_response_payload(response, tbuf, len);
}

/*---------------------------------------------------------------------------*/
#if UIP_CONF_IPV6_RPL && 0
#include <avr/pgmspace.h>
#include "rpl.h"

//extern uip_ds6_nbr_t uip_ds6_nbr_cache[];
//extern uip_ds6_route_t uip_ds6_routing_table[];

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
	//int i;
	blen = 0;
	ADD("Routes [%u max]\n\r", UIP_DS6_ROUTE_NB);

	{
		uip_ds6_route_t *r;
		for(r = uip_ds6_route_list_head();
				r != NULL;
				r = list_item_next(r)) {
			ipaddr_add(&r->ipaddr);
			ADD("/%u (via ", r->length);
			ipaddr_add(&r->nexthop);
			if(r->state.lifetime < 600) {
			  ADD(") %lus<br>", r->state.lifetime);
			} else {
			  PRINTF(")\n");
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
	rest_activate_resource(&resource_take);
	rest_activate_resource(&resource_size);
	rest_activate_resource(&resource_count);
	rest_activate_resource(&resource_packet);
#if UIP_CONF_IPV6_RPL && 0
	rest_activate_resource(&resource_route);
#endif
	rest_activate_periodic_resource(&periodic_resource_temperature);
	rest_activate_periodic_resource(&periodic_resource_light);
	rest_activate_periodic_resource(&periodic_resource_voltage);

	PROCESS_END();
}
/*---------------------------------------------------------------------------*/

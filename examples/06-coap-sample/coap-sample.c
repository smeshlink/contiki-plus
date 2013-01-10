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
#if UIP_CONF_IPV6_RPL
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
#if UIP_CONF_IPV6_RPL && 0
	rest_activate_resource(&resource_route);
#endif
	rest_activate_periodic_resource(&periodic_resource_voltage);

	PROCESS_END();
}
/*---------------------------------------------------------------------------*/

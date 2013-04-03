/*
 * Copyright (c) 2011-2012 SmeshLink Technology Corporation.
 * All rights reserved.
 *
 * $Id: hello-world.c $
 */

/**
 * \file
 *         A very simple Contiki application showing how Contiki programs look.
 * \author
 *         SmeshLink
 */

#include "contiki.h"
#include "dev/leds.h"
#include "lib/random.h"
#include <string.h>
#include <stdio.h>

/*---------------------------------------------------------------------------*/
PROCESS(hello_world_process, "Hello world process");
PROCESS(leds_blink_process, "LEDs blink process");
AUTOSTART_PROCESSES(&hello_world_process,&leds_blink_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(hello_world_process, ev, data)
{
	/* Variables are declared static to ensure their values are kept between kernel calls. */
	static struct etimer timer;
	static int count = 0;

	/* Any process must start with this. */
	PROCESS_BEGIN();

	/* Set the etimer to generate an event in one second. */
	etimer_set(&timer, CLOCK_CONF_SECOND);

	while(1) {
		/* Wait for an event. */
		PROCESS_WAIT_EVENT();

		/* Got the timer's event~ */
		if (ev == PROCESS_EVENT_TIMER) {
			int i;
			printf("Hello, world #%u\n", count);
			count++;

			/* Reset the etimer so it will generate another event after the exact same time. */
			etimer_reset(&timer);
		}
	} // while (1)

	/* Any process must end with this, even if it is never reached. */
	PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(leds_blink_process, ev, data)
{
	static struct etimer timer;
	static uint8_t leds_state = 0;

	/* Any process must start with this. */
	PROCESS_BEGIN();

	while(1) {
		/* Set the etimer every time. */
		etimer_set(&timer, CLOCK_CONF_SECOND*1);
		/* And wait until the specific event. */
		PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);

		/* Change the state of leds. */
		leds_off(LEDS_ALL);
#ifndef CONF_LOWPOWER
		leds_on(leds_state);
		leds_state += 1;
#endif
	}

	/* Any process must end with this, even if it is never reached. */
	PROCESS_END();
}
/*---------------------------------------------------------------------------*/

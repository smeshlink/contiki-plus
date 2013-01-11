/*
 * Copyright (c) 2012-2013, SmeshLink Technology Ltd.
 * All rights reserved.
 *
 */

#include "contiki.h"
#include "contiki-arduino.h"

unsigned char arduino_node_id[8] = { 0x02, 0x11, 0x22, 0xff, 0xfe, 0x33, 0x44, 0x33 };
unsigned char arduino_channel = MXCHANNEL;
unsigned char arduino_power = RF230_MAX_TX_POWER;

PROCESS(contiki_arduino, "contiki arduino");
AUTOSTART_PROCESSES(&contiki_arduino);

PROCESS_THREAD(contiki_arduino, ev, data)
{
	PROCESS_BEGIN();
	setup();
	PROCESS_END();
}

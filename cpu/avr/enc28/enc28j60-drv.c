/*-----------------------------------------------------------------------------------*/
/*
 * Copyright (c) 2001-2004, Adam Dunkels.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This file is part of the uIP TCP/IP stack.
 *
 * This is a modified contiki driver example.
 * Author: Maciej Wasilak (wasilak@gmail.com)
 *
 * $Id: enc28j60-drv.c,v 1.2 2007/05/26 23:05:36 oliverschmidt Exp $
 *
 */

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/fuse.h>
#include <avr/eeprom.h>
#include "enc28/enc28j60.h"
#include "enc28/enc28j60-drv.h"
#include "contiki-net.h"
#include "net/uip-neighbor.h"
#include <string.h>

#define DEBUG DEBUG_NONE
#include "net/uip-debug.h"

#define ENC28_INTERFACE_ID IF_FALLBACK

#define BUF ((struct uip_eth_hdr *)&ll_header[0])
#define IPBUF ((struct uip_tcpip_hdr *)&uip_buf[UIP_LLH_LEN])

PROCESS(enc28j60_process, "ENC28J60 driver");

uint8_t eth_mac_addr[6] EEMEM = {0x02, 0xaa, 0xbb, 0xcc, 0xdd, 0xee};

static void (* input_callback)(void) = NULL;
/*---------------------------------------------------------------------------*/
void
enc28j60_set_input_callback(void (*c)(void))
{
  input_callback = c;
}

/*---------------------------------------------------------------------------*/
u8_t
enc28j60_output(uip_lladdr_t *lladdr)
{
	  PRINTF("ENC28 send: %d bytes\n",uip_len);
	if (lladdr == NULL) {
		(&BUF->dest)->addr[0] = 0x33;
		(&BUF->dest)->addr[1] = 0x33;
		(&BUF->dest)->addr[2] = IPBUF->destipaddr.u8[12];
		(&BUF->dest)->addr[3] = IPBUF->destipaddr.u8[13];
		(&BUF->dest)->addr[4] = IPBUF->destipaddr.u8[14];
		(&BUF->dest)->addr[5] = IPBUF->destipaddr.u8[15];
	}
	else {
		memcpy(&BUF->dest, lladdr, uip_ds6_if[ENC28_INTERFACE_ID].lladdr_len);
	}
	memcpy(&BUF->src, &uip_lladdr[ENC28_INTERFACE_ID].addr, uip_ds6_if[ENC28_INTERFACE_ID].lladdr_len);
	BUF->type = UIP_HTONS(UIP_ETHTYPE_IPV6);
	uip_len += sizeof(struct uip_eth_hdr);
	cli();
	enc28j60PacketSend(uip_len, uip_buf);
	sei();

  return 0;
}

/*
 * Placeholder - switching off enc28 chip wasn't yet considered
 */
void enc28j60_exit(void)
{}

/*
 * Wrapper for lowlevel enc28j60 init code
 * in current configuration it reads the Ethernet driver MAC address
 * from EEPROM memory
 */
void enc28j60_init()
{
	uip_ds6_if[ENC28_INTERFACE_ID].lladdr_len = 6;
	eeprom_read_block ((void *)&uip_lladdr[ENC28_INTERFACE_ID].addr,  &eth_mac_addr, uip_ds6_if[ENC28_INTERFACE_ID].lladdr_len);
	enc28j60Init(uip_lladdr[ENC28_INTERFACE_ID].addr);
	PRINTF("FALLBACK MAC %x:%x:%x:%x:%x:%x\n",uip_lladdr[ENC28_INTERFACE_ID].addr[0],uip_lladdr[ENC28_INTERFACE_ID].addr[1],uip_lladdr[ENC28_INTERFACE_ID].addr[2],uip_lladdr[ENC28_INTERFACE_ID].addr[3],uip_lladdr[ENC28_INTERFACE_ID].addr[4],uip_lladdr[ENC28_INTERFACE_ID].addr[5]);
}


/*---------------------------------------------------------------------------*/
static void
pollhandler(void)
{
  process_poll(&enc28j60_process);
  cli();
  uip_len = enc28j60PacketReceive(UIP_BUFSIZE,uip_buf );
  sei();
  if (uip_len > 0) {
	  PRINTF("ENC28 receive: %d bytes\n",uip_len);
  		if (BUF->type == uip_htons(UIP_ETHTYPE_IPV6))
  		{
  			uip_active_interface = ENC28_INTERFACE_ID;
  			tcpip_input();
  		}
  		else
  		{
  		      uip_len = 0;
  		}
  	}
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(enc28j60_process, ev, data)
{
  PROCESS_POLLHANDLER(pollhandler());

  PROCESS_BEGIN();

  tcpip_set_outputfunc(ENC28_INTERFACE_ID, enc28j60_output);

  process_poll(&enc28j60_process);
  
  PROCESS_WAIT_UNTIL(ev == PROCESS_EVENT_EXIT);

  enc28j60_exit();

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

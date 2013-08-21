/*
 * Copyright (c) 2011-2013, SmeshLink Technology Ltd.
 * All rights reserved.
 *
 * $Id: mx-shell.c $
 */

/**
 * \file
 *         A shell back-end for the serial port.
 * \author
 *         SmeshLink
 */

#include "mx-shell.h"
#include "serial-shell.h"

#include "radio.h"
#if RF230BB || RF212BB
#include "rf230bb.h"
#endif

#include "params.h"
#include "dev/rs232.h"
#include "dev/serial-line.h"

static char buf[64];
/*---------------------------------------------------------------------------*/
PROCESS(shell_txpower_process, "txpower");
SHELL_COMMAND(txpower_command,
          "txpower",
          "txpower <power>: show or change RF transmission power (0 - 15)",
          &shell_txpower_process);
PROCESS(shell_rfchannel_process, "rfchannel");
SHELL_COMMAND(rfchannel_command,
          "rfchannel",
          "rfchannel <channel>: show or change RF channel (11 - 26)",
          &shell_rfchannel_process);
PROCESS(shell_debug_process, "debug");
SHELL_COMMAND(debug_command,
          "debug",
          "debug <state>: toggle debug output (on / off)",
          &shell_debug_process);
PROCESS(shell_net_process, "net");
SHELL_COMMAND(net_command,
          "net",
          "net: show addresses, neighbors, and routes (if RPL is enabled)",
          &shell_net_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(shell_txpower_process, ev, data)
{
  uint16_t txpower;
  const char *newptr;

  PROCESS_BEGIN();

  txpower = shell_strtolong(data, &newptr);

  /* If no transmission power was given on the command line, we print
     out the current txpower. */
  if(newptr == data) {
    txpower = rf230_get_txpower();
    sprintf(buf, "%d", txpower);
    shell_output_str(&txpower_command, "Current TX power (0=+3dBm, 15=-17.2dBm) is ", buf);
  } else if ((txpower < 0) || (txpower > 15)) {
    shell_output_str(&txpower_command, "Invalid TX power", "");
  } else {
    rf230_set_txpower(txpower);
    sprintf(buf, "Transmit power changed to %d", txpower);
#if CONTIKI_CONF_SETTINGS_MANAGER
    if(settings_set_uint8(SETTINGS_KEY_TXPOWER, txpower) == SETTINGS_STATUS_OK) {
      shell_output_str(&txpower_command, buf, " and stored in EEPROM.");
    } else {
      shell_output_str(&txpower_command, buf, ", but unable to store in EEPROM!");
    }
#else
    shell_output_str(&txpower_command, buf, "");
#endif
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(shell_rfchannel_process, ev, data)
{
  uint16_t channel;
  const char *newptr;

  PROCESS_BEGIN();

  channel = shell_strtolong(data, &newptr);

  /* If no channel was given on the command line, we print out the
     current channel. */
  if(newptr == data) {
    channel = rf230_get_channel();
    sprintf(buf, "%d", channel);
    shell_output_str(&rfchannel_command, "Current channel is ", buf);
  } else if ((channel < 11) || (channel > 26)) {
    shell_output_str(&rfchannel_command, "Invalid channel", "");
  } else {
    rf230_set_channel(channel);
    sprintf(buf, "Channel changed to %d", channel);
#if CONTIKI_CONF_SETTINGS_MANAGER
    if(settings_set_uint8(SETTINGS_KEY_CHANNEL, channel) == SETTINGS_STATUS_OK) {
      shell_output_str(&rfchannel_command, buf, " and stored in EEPROM.");
    } else {
      shell_output_str(&rfchannel_command, buf, ", but unable to store in EEPROM!");
    }
#else
    shell_output_str(&rfchannel_command, buf, "");
#endif
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(shell_debug_process, ev, data)
{
  PROCESS_BEGIN();

  if (strncasecmp(data, "on", 2) == 0) {
    shell_mode.debugOn = 1;
  } else if (strncasecmp(data, "off", 3) == 0) {
    shell_mode.debugOn = 0;
  }

  shell_output_str(&debug_command, "Debug strings are ",
      shell_mode.debugOn ? "on" : "off");

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
#include "uip.h"

static uint16_t
ipaddr_add(char *buff, const uip_ipaddr_t *addr)
{
  uint16_t a, offset = 0;
  int8_t i, f;
  for(i = 0, f = 0; i < sizeof(uip_ipaddr_t); i += 2) {
    a = (addr->u8[i] << 8) + addr->u8[i + 1];
    if(a == 0 && f >= 0) {
      if(f++ == 0)
        offset += sprintf(buff + offset, "::");
    } else {
      if(f > 0) {
        f = -1;
      } else if(i > 0) {
        offset += sprintf(buff + offset, ":");
      }
      offset += sprintf(buff + offset, "%x", a);
    }
  }
  return offset;
}

#include "net/uip-ds6.h"
extern uip_ds6_nbr_t uip_ds6_nbr_cache[];
extern uip_ds6_netif_t uip_ds6_if;
#if UIP_CONF_IPV6_RPL
#include "rpl.h"
extern uip_ds6_route_t uip_ds6_routing_table[];
#endif

PROCESS_THREAD(shell_net_process, ev, data)
{
  uint8_t i,j;
  PROCESS_BEGIN();

  shell_output_str(&net_command, "", "");
  sprintf(buf, "Addresses [%u max]", UIP_DS6_ADDR_NB);
  shell_output_str(&net_command, buf, "");
  for (i = 0;i < UIP_DS6_ADDR_NB; i++) {
    if (uip_ds6_if.addr_list[i].isused) {
      ipaddr_add(buf, &uip_ds6_if.addr_list[i].ipaddr);
      shell_output_str(&net_command, buf, "");
    }
  }

  shell_output_str(&net_command, "", "");
  sprintf(buf, "Neighbors [%u max]", UIP_DS6_NBR_NB);
  shell_output_str(&net_command, buf, "");
  for(i = 0, j = 1; i < UIP_DS6_NBR_NB; i++) {
    if(uip_ds6_nbr_cache[i].isused) {
      ipaddr_add(buf, &uip_ds6_nbr_cache[i].ipaddr);
      shell_output_str(&net_command, buf, "");
      j = 0;
    }
  }
  if (j)
    shell_output_str(&net_command, "  <none>", "");

#if UIP_CONF_IPV6_RPL
  shell_output_str(&net_command, "", "");
  sprintf(buf, "Routes [%u max]", UIP_DS6_ROUTE_NB);
  shell_output_str(&net_command, buf, "");
  {
    uip_ds6_route_t *r;
    j = 1;
    for(r = uip_ds6_route_list_head();
        r != NULL;
        r = list_item_next(r)) {
      uint16_t offset = ipaddr_add(buf, &r->ipaddr);
      offset += sprintf(buf + offset, "/%u (via ", r->length);
      ipaddr_add(buf + offset, &r->nexthop);
      if(r->state.lifetime < 600) {
        sprintf(buf + offset, ") %lus", r->state.lifetime);
      } else {
        sprintf(buf + offset, ")");
      }
      shell_output_str(&net_command, buf, "");
      j = 0;
    }
  }
  if (j)
    shell_output_str(&net_command, "  <none>", "");
#endif

  shell_output_str(&net_command, "", "");

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
static int
serial_input_byte(unsigned char c)
{
  int ret = 0;

  stdout = shell_mode.serial_stdout;
  printf("%c", c);

  if (c == 0x0d) {
    c = 0x0a;
    printf("\r\n");
  } else if (c == 0x0a) {
    c = 0x0d;
  }
  ret = serial_line_input_byte(c);

  if (!shell_mode.debugOn) {
    stdout = NULL;
  }

  return ret;
}
/*---------------------------------------------------------------------------*/
void
mx_shell_init(uint8_t port)
{
  rs232_init(port, USART_BAUD_38400, USART_PARITY_NONE | USART_STOP_BITS_1 | USART_DATA_BITS_8);
  rs232_set_input(port, serial_input_byte);

  shell_mode.serial_stdout = stdout;
  shell_mode.debugOn = 0;

  serial_line_init();
  serial_shell_init();

  shell_blink_init();
  shell_ps_init();
  shell_reboot_init();
  shell_power_init();
  shell_time_init();
  shell_ping_init();
  shell_register_command(&txpower_command);
  shell_register_command(&rfchannel_command);
  shell_register_command(&debug_command);
  shell_register_command(&net_command);
}

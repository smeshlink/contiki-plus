/*
 * Copyright (c) 2011-2013, SmeshLink Technology Ltd.
 * All rights reserved.
 *
 * $Id: mx-console.c $
 */

/**
 * \file
 *         A console for the serial port.
 * \author
 *         SmeshLink
 */

#include "contiki.h"
#include "contiki-net.h"
#include "uip.h"
#include "mx-console.h"
#include "params.h"
#include "dev/leds.h"
#include "dev/rs232.h"
#include "dev/watchdog.h"

#include "radio.h"
#if RF230BB || RF212BB
#include "rf230bb.h"
#endif

#include <stdio.h>

#include <util/delay.h>

#define PRINTF printf
#define PRINTF_P printf_P
#define Max(a, b)            ( (a)>(b) ? (a) : (b) )       // Take the max between a and b
#define Min(a, b)            ( (a)<(b) ? (a) : (b) )
#define bzero(ptr,size) memset(ptr,0,size)

typedef struct {
    uint8_t debugOn     :1;
} mx_console_mode_t;

mx_console_mode_t mx_console_mode;

void
menu_print(void)
{
  PRINTF_P(PSTR("\n\r*********** MX Console menu **********\n\r"));
  PRINTF_P(PSTR("*      [Built "__DATE__"]       *\n\r"));
  PRINTF_P(PSTR("*                                 *\n\r"));
  PRINTF_P(PSTR("*  m        Print current mode    *\n\r"));
  PRINTF_P(PSTR("*  c        Set RF channel        *\n\r"));
  PRINTF_P(PSTR("*  p        Set RF power          *\n\r"));
  PRINTF_P(PSTR("*  d        Toggle RS232 output   *\n\r"));
#if UIP_CONF_IPV6_RPL
  PRINTF_P(PSTR("*  N        RPL Neighbors         *\n\r"));
  PRINTF_P(PSTR("*  G        RPL Global Repair     *\n\r"));
#endif
  PRINTF_P(PSTR("*  e        Energy Scan           *\n\r"));
  PRINTF_P(PSTR("*  R        Reset (via WDT)       *\n\r"));
  PRINTF_P(PSTR("*  h,?      Print this menu       *\n\r"));
  PRINTF_P(PSTR("*                                 *\n\r"));
  PRINTF_P(PSTR("* Make selection at any time by   *\n\r"));
  PRINTF_P(PSTR("* pressing your choice on keyboard*\n\r"));
  PRINTF_P(PSTR("***********************************\n\r"));
}

#if UIP_CONF_IPV6_RPL
static void
ipaddr_add(const uip_ipaddr_t *addr)
{
  uint16_t a;
  int8_t i, f;
  for(i = 0, f = 0; i < sizeof(uip_ipaddr_t); i += 2) {
    a = (addr->u8[i] << 8) + addr->u8[i + 1];
    if(a == 0 && f >= 0) {
      if(f++ == 0) PRINTF_P(PSTR("::"));
    } else {
      if(f > 0) {
        f = -1;
      } else if(i > 0) {
        PRINTF_P(PSTR(":"));
      }
      PRINTF_P(PSTR("%x"),a);
    }
  }
}
#endif

void menu_process(char c)
{
  static enum menustate_enum            /* Defines an enumeration type    */
  {
      normal,
      channel,
      txpower
  } menustate = normal;

  static char channel_string[3];
  static uint8_t channel_string_i;// = 0;

  int tempchannel;

  if (menustate == channel) {

    switch(c) {
    case '\r':
    case '\n':
      if (channel_string_i) {
        channel_string[channel_string_i] = 0;
        tempchannel = atoi(channel_string);

        if ((tempchannel < 11) || (tempchannel > 26)) {
          PRINTF_P(PSTR("\n\rInvalid input\n\r"));
        } else {
          rf230_set_channel(tempchannel);

#if CONTIKI_CONF_SETTINGS_MANAGER
          if(settings_set_uint8(SETTINGS_KEY_CHANNEL, tempchannel) == SETTINGS_STATUS_OK) {
            PRINTF_P(PSTR("\n\rChannel changed to %d and stored in EEPROM.\n\r"), tempchannel);
          } else {
            PRINTF_P(PSTR("\n\rChannel changed to %d, but unable to store in EEPROM!\n\r"), tempchannel);
          }
#else
          PRINTF_P(PSTR("\n\rChannel changed to %d.\n\r"), tempchannel);
#endif
        }
      } else {
        PRINTF_P(PSTR("\n\rChannel unchanged.\n\r"));
      }

      menustate = normal;
      break;

    case '\b':
      if (channel_string_i) {
        channel_string_i--;
        PRINTF_P(PSTR("\b \b"));
      }
      break;

    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      if (channel_string_i > 1) {
        // This time the user has gone too far.
        // Beep at them.
        putc('\a', stdout);
        break;
      }
      putc(c, stdout);
      channel_string[channel_string_i] = c;
      channel_string_i++;
      break;

    default:
      break;
    }

  } else if (menustate == txpower) {

    switch(c) {
    case '\r':
    case '\n':
      if (channel_string_i) {
        channel_string[channel_string_i] = 0;
        tempchannel = atoi(channel_string);

        if ((tempchannel < 0) || (tempchannel > 15))  {
          PRINTF_P(PSTR("\n\rInvalid input\n\r"));
        } else {
          PRINTF_P(PSTR(" ")); //for some reason needs a print here to clear the string input...
          rf230_set_txpower(tempchannel);

#if CONTIKI_CONF_SETTINGS_MANAGER
          if(settings_set_uint8(SETTINGS_KEY_TXPOWER, tempchannel) == SETTINGS_STATUS_OK) {
            PRINTF_P(PSTR("\n\rTransmit power changed to %d, and stored in EEPROM.\n\r"), tempchannel);
          } else {
            PRINTF_P(PSTR("\n\rTransmit power changed to %d, but unable to store in EEPROM!\n\r"), tempchannel);
          }
#else
          PRINTF_P(PSTR("\n\rTransmit power changed to %d.\n\r"), tempchannel);
#endif
        }
      } else {
        PRINTF_P(PSTR("\n\rTransmit power unchanged.\n\r"));
      }

      menustate = normal;
      break;

    case '\b':
      if (channel_string_i) {
        channel_string_i--;
        PRINTF_P(PSTR("\b \b"));
      }
      break;

    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      if (channel_string_i > 1) {
        // This time the user has gone too far.
        // Beep at them.
        putc('\a', stdout);
        break;
      }
      putc(c, stdout);
      channel_string[channel_string_i] = c;
      channel_string_i++;
      break;

    default:
      break;
    }

  } else {
    uint8_t i;

    switch(c) {
    case '\r':
    case '\n':
      break;

    case 'h':
    case '?':
      menu_print();
      break;

    case 'd':
      if (mx_console_mode.debugOn) {
        PRINTF_P(PSTR("Node does not output debug strings\n\r"));
        mx_console_mode.debugOn = 0;
      } else {
        PRINTF_P(PSTR("Node now outputs debug strings\n\r"));
        mx_console_mode.debugOn = 1;
      }
      break;

    case 'c':
      PRINTF_P(PSTR("\nSelect 802.15.4 Channel in range 11-26 [%d]: "), rf230_get_channel());
      menustate = channel;
      channel_string_i = 0;
      break;
    case 'p':
      PRINTF_P(PSTR("\nSelect transmit power (0=+3dBm 15=-17.2dBm) [%d]: "), rf230_get_txpower());
      menustate = txpower;
      channel_string_i = 0;
      break;

#if UIP_CONF_IPV6_RPL
#include "rpl.h"
extern uip_ds6_nbr_t uip_ds6_nbr_cache[];
extern uip_ds6_netif_t uip_ds6_if;

    case 'N':
    {
      uint8_t i,j;
      PRINTF_P(PSTR("\n\rAddresses [%u max]\n\r"),UIP_DS6_ADDR_NB);
      for (i=0;i<UIP_DS6_ADDR_NB;i++) {
        if (uip_ds6_if.addr_list[i].isused) {
          ipaddr_add(&uip_ds6_if.addr_list[i].ipaddr);
          PRINTF_P(PSTR("\n\r"));
        }
      }
      PRINTF_P(PSTR("\n\rNeighbors [%u max]\n\r"),UIP_DS6_NBR_NB);
      for(i = 0,j=1; i < UIP_DS6_NBR_NB; i++) {
        if(uip_ds6_nbr_cache[i].isused) {
          ipaddr_add(&uip_ds6_nbr_cache[i].ipaddr);
          PRINTF_P(PSTR("\n\r"));
          j=0;
        }
      }
      if (j) PRINTF_P(PSTR("  <none>"));
      PRINTF_P(PSTR("\n\rRoutes [%u max]\n\r"),UIP_DS6_ROUTE_NB);
      {
        uip_ds6_route_t *r;
        j = 1;
        for(r = uip_ds6_route_list_head();
            r != NULL;
            r = list_item_next(r)) {
          ipaddr_add(&r->ipaddr);
          PRINTF_P(PSTR("/%u (via "), r->length);
          ipaddr_add(&r->nexthop);
          if(r->state.lifetime < 600) {
            PRINTF_P(PSTR(") %lus\n\r"), r->state.lifetime);
          } else {
            PRINTF_P(PSTR(")\n\r"));
          }
          j = 0;
        }
      }

      if (j) PRINTF_P(PSTR("  <none>"));
      PRINTF_P(PSTR("\n\r---------\n\r"));
      break;
    }

    case 'G':
      PRINTF_P(PSTR("Global repair returns %d\n\r"),rpl_repair_root(RPL_DEFAULT_INSTANCE));
      break;

    case 'L':
      rpl_local_repair(rpl_get_any_dag());
      PRINTF_P(PSTR("Local repair initiated\n\r"));
      break;

#endif

    case 'm':
      PRINTF_P(PSTR("Currently running on\n\r"));
      PRINTF_P(PSTR("  * %s\n\r"), CONTIKI_VERSION_STRING);
      PRINTF_P(PSTR("  * NETSTACK_MAC: %s, NETSTACK_RDC: %s\n\r"), NETSTACK_MAC.name, NETSTACK_RDC.name);
#if 1
      {
        int i;
        PRINTF_P(PSTR("  * Address: "));
        for (i = 0; i < 6; i += 2) {
          PRINTF_P(PSTR("%02x%02x:"), uip_lladdr.addr[i], uip_lladdr.addr[i + 1]);
        }
        PRINTF_P(PSTR("%02x%02x\n\r"), uip_lladdr.addr[6], uip_lladdr.addr[7]);
      }
#endif
#if UIP_CONF_IPV6_RPL
      PRINTF_P(PSTR("  * RPL Enabled\n\r"));
#endif
#if UIP_CONF_ROUTER
      PRINTF_P(PSTR("  * Routing Enabled\n\r"));
#endif
#if CONVERTTXPOWER
      PRINTF_P(PSTR("  * Operates on channel %d with TX power "),rf230_get_channel());
      printtxpower();
      PRINTF_P(PSTR("\n\r"));
#else  //just show the raw value
      PRINTF_P(PSTR("  * Operates on channel %d\n\r"), rf230_get_channel());
      PRINTF_P(PSTR("  * TX Power(0=+3dBm, 15=-17.2dBm): %d\n\r"), rf230_get_txpower());
#endif
      if (rf230_smallest_rssi) {
        PRINTF_P(PSTR("  * Current/Last/Smallest RSSI: %d/%d/%ddBm\n\r"), -91+(rf230_rssi()-1), -91+(rf230_last_rssi-1),-91+(rf230_smallest_rssi-1));
        rf230_smallest_rssi=0;
      } else {
        PRINTF_P(PSTR("  * Current/Last/Smallest RSSI: %d/%d/--dBm\n\r"), -91+(rf230_rssi()-1), -91+(rf230_last_rssi-1));
      }

#if CONFIG_STACK_MONITOR
      /* See contiki-raven-main.c for initialization of the magic numbers */
      {
        extern uint16_t __bss_end;
        uint16_t p=(uint16_t)&__bss_end;
        do {
          if (*(uint16_t *)p != 0x4242) {
            printf_P(PSTR("  * Never-used stack > %d bytes\n\r"),p-(uint16_t)&__bss_end);
            break;
          }
          p+=100;
        } while (p<RAMEND-100);
      }
#endif

      break;

    case 'e':
      PRINTF_P(PSTR("Energy Scan:\n"));
      {
        uint8_t i;
        uint16_t j;
        uint8_t previous_channel = rf230_get_channel();
        int8_t RSSI, maxRSSI[17];
        uint16_t accRSSI[17];

        bzero((void*)accRSSI,sizeof(accRSSI));
        bzero((void*)maxRSSI,sizeof(maxRSSI));

        for(j=0;j<(1<<12);j++) {
          for(i=11;i<=26;i++) {
            rf230_listen_channel(i);

            _delay_us(3*10);
            RSSI = rf230_rssi();  //multiplies rssi register by 3 for consistency with energy-detect register
            maxRSSI[i-11]=Max(maxRSSI[i-11],RSSI);
            accRSSI[i-11]+=RSSI;
          }

          if(j&(1<<7)) {
            leds_on(LEDS_RED);
            if(!(j&((1<<7)-1))) {
              PRINTF_P(PSTR("."));
            }
          } else {
            leds_off(LEDS_RED);
          }

          watchdog_periodic();
        }

        rf230_set_channel(previous_channel);

        PRINTF_P(PSTR("\n"));
        for(i=11;i<=26;i++) {
          uint8_t activity=Min(maxRSSI[i-11],accRSSI[i-11]/(1<<7));
          PRINTF_P(PSTR(" %d: %02ddB "),i, -91+(maxRSSI[i-11]-1));
          for(;activity--;maxRSSI[i-11]--) {
            PRINTF_P(PSTR("#"));
          }
          for(;maxRSSI[i-11]>0;maxRSSI[i-11]--) {
            PRINTF_P(PSTR(":"));
          }
          PRINTF_P(PSTR("\n"));
        }
      }

      PRINTF_P(PSTR("Done.\n"));
      break;

    case 'R':
      PRINTF_P(PSTR("Resetting...\n\r"));
      leds_on(LEDS_ALL);
      for(i = 0; i < 20; i++) _delay_ms(100);
      watchdog_reboot();
      break;

    default:
      PRINTF_P(PSTR("%c is not a valid option! h for menu\n\r"), c);
      break;
    }

  }
}

static FILE *rs232_stdout;

static int
rs232_serial_input(unsigned char ch)
{
  stdout = rs232_stdout;

  menu_process(ch);

  if (!mx_console_mode.debugOn) {
    stdout = NULL;
  }

  return 0;
}

void
mx_console_init(uint8_t port)
{
  rs232_init(port, USART_BAUD_38400, USART_PARITY_NONE | USART_STOP_BITS_1 | USART_DATA_BITS_8);
  rs232_set_input(port, rs232_serial_input);
  rs232_redirect_stdout(port);

  rs232_stdout = stdout;
  mx_console_mode.debugOn = 1;
}

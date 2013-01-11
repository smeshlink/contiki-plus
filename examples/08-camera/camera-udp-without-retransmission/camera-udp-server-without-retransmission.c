/*
 * Copyright (c) 2011-2012, SmeshLink Technology Ltd.
 * All rights reserved.
 *
 * $Id: camera-udp-server-without-retransmission.c $
 */

/**
 * \file
 *         UDP server without retransmission for camera application.
 * \author
 *         SmeshLink
 */

#include "contiki.h"
#include "contiki-net.h"

#include "camera.h"

#include <stdio.h>
#include <string.h>

#define CAMERA_PORT 9000
#define UDP_HDR ((struct uip_udpip_hdr *)&uip_buf[UIP_LLH_LEN])
#define MAX_PAYLOAD_LEN 128

static struct uip_udp_conn *server_conn;
static uint8_t buffer[MAX_PAYLOAD_LEN];

static void
udp_handler(void)
{
  static uint8_t tries = 10;
  static camera_size_t camera_size;
  static uint16_t index, count, len;
  static uint32_t size;
  static uint16_t k = 0;

  if (uip_newdata()) {
    uip_ipaddr_copy(&server_conn->ripaddr, &UDP_HDR->srcipaddr);
    server_conn->rport = UDP_HDR->srcport;
    if (1 == uip_datalen()) {
      camera_size = camera_parse_size(((char*)uip_appdata)[0]);

      for (k = 0; k < 20; k++) {
        camera_take_picture(camera_size, DEFAULT_CAMERA_PACKET_SIZE, &size, &count);

        /* Send picture packet by packet. */
        for (index = 1; index <= count; index++) {
          len = camera_try_get_packet(index, buffer, tries);
          uip_udp_packet_send(server_conn, buffer, len);
        }

        buffer[0] = 'D';
        buffer[1] = 'O';
        buffer[2] = 'N';
        buffer[3] = 'E';
        uip_udp_packet_send(server_conn, buffer, 4);
      }
    } else {
      index = (uint16_t)((char*)uip_appdata)[0] & 0x00FF;
      index += ((uint16_t)((char*)uip_appdata)[1] & 0x00FF) << 8;
      len = camera_try_get_packet(index, buffer, tries);
      uip_udp_packet_send(server_conn, buffer, len);
    }

    memset(&server_conn->ripaddr, 0, sizeof(server_conn->ripaddr));
    server_conn->rport = 0;
  }
}

/*---------------------------------------------------------------------------*/
PROCESS(camera_udp_without_retransmission_server, "Camera UDP server");
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(camera_udp_without_retransmission_server, ev, data)
{
  PROCESS_BEGIN();

  server_conn = udp_new(NULL, uip_htons(0), NULL);
  udp_bind(server_conn, uip_htons(CAMERA_PORT));

  /* Loop forever, accepting new connections. */
  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(ev == tcpip_event);
    udp_handler();
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

/*
 * Copyright (c) 2011-2012, SmeshLink Technology Ltd.
 * All rights reserved.
 *
 * $Id: camera-rest-server.c $
 */

/**
 * \file
 *         Restful interface (HTTP or CoAP based) for camera application.
 * \author
 *         SmeshLink
 */

#include "contiki.h"
#include "rest.h"

#include "camera.h"

#include <stdio.h>
#include <string.h>

#define rest_set_payload rest_set_response_payload

/*---------------------------------------------------------------------------*/
RESOURCE(take, METHOD_GET, "take");
/*
 * Take a picture.
 */
void
take_handler(REQUEST* request, RESPONSE* response)
{
  char temp[64] = "";
  camera_size_t camera_size = CAMERA_SIZE_1;
  uint16_t count;
  uint32_t size;

  /* Determine the size of picture. 1 for the smallest and 3 for the largest. */
  if (rest_get_query_variable(request, "size", temp, 10)) {
    camera_size = camera_parse_size(temp[0]);
  }

  camera_take_picture(camera_size, DEFAULT_CAMERA_PACKET_SIZE, &size, &count);
  sprintf(temp, "{'t':'OK','s':%lu,'n':%u}", size, count);

  rest_set_header_content_type(response, TEXT_PLAIN);
  rest_set_payload(response, temp, strlen(temp));
}

RESOURCE(size, METHOD_GET, "size");
/*
 * Get the size of the taken picture.
 */
void
size_handler(REQUEST* request, RESPONSE* response)
{
  char temp[16] = "";

  uint32_t size = camera_get_picture_size();
  sprintf(temp, "%lu", size);

  rest_set_header_content_type(response, TEXT_PLAIN);
  rest_set_payload(response, temp, strlen(temp));
}

RESOURCE(count, METHOD_GET, "count");
/*
 * Get the total count of the picture's packets.
 */
void
count_handler(REQUEST* request, RESPONSE* response)
{
  char temp[8] = "";

  uint16_t count = camera_get_packet_count();
  sprintf(temp, "%u", count);

  rest_set_header_content_type(response, TEXT_PLAIN);
  rest_set_payload(response, temp, strlen(temp));
}

RESOURCE(packet, METHOD_GET | METHOD_PUT | METHOD_POST, "packet");
/*
 * Get one packet of the picture.
 */
void
packet_handler(REQUEST* request, RESPONSE* response)
{
  char temp[1024];
  int no = 0, len = 0, i = 0, tries = 0;
  /* Get the index of the specified packet. */
  if (rest_get_query_variable(request, "no", temp, 10)) {
    len = strlen(temp);
    for (; i < len; i++) {
      no = no * 10 + temp[i] - '0';
    }
  }
  /* Get count of tries. Unused right now. */
  if (rest_get_query_variable(request, "try", temp, 10)) {
    len = strlen(temp);
    for (; i < len; i++) {
      tries = tries * 10 + temp[i] - '0';
    }
  }
  if (tries <= 0) {
    tries = 10;
  }
  len = camera_try_get_packet(no, temp, tries);
  rest_set_payload(response, temp, len);
  rest_set_header_content_type(response, TEXT_PLAIN);
}

RESOURCE(debug, METHOD_GET, "debug");
/*
 * Get debug information.
 */
void
debug_handler(REQUEST* request, RESPONSE* response)
{
  char temp[1024];
  int len = camera_debug(temp);
  rest_set_payload(response, temp, len);
  rest_set_header_content_type(response, TEXT_PLAIN);
}

RESOURCE(discover, METHOD_GET, ".well-known/core");
void
discover_handler(REQUEST* request, RESPONSE* response)
{
  char temp[256];
  int index = 0;

  index += sprintf(temp + index, "%s,", "</take>;n=\"take\"");
  index += sprintf(temp + index, "%s,", "</size>;n=\"size\"");
  index += sprintf(temp + index, "%s,", "</count>;n=\"count\"");
  index += sprintf(temp + index, "%s,", "</packet>;n=\"packet\"");

  rest_set_payload(response, temp, strlen(temp));
  rest_set_header_content_type(response, APPLICATION_LINK_FORMAT);
}

/*---------------------------------------------------------------------------*/
PROCESS(camera_rest_server, "Camera restful server");
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(camera_rest_server, ev, data)
{
  PROCESS_BEGIN();

  rest_init();
  rest_activate_resource(&resource_take);
  rest_activate_resource(&resource_size);
  rest_activate_resource(&resource_count);
  rest_activate_resource(&resource_packet);
  rest_activate_resource(&resource_debug);
  rest_activate_resource(&resource_discover);

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

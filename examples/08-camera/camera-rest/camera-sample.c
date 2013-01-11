/*
 * Copyright (c) 2011-2012, SmeshLink Technology Ltd.
 * All rights reserved.
 *
 * $Id: camera-sample.c $
 */

/**
 * \file
 *         Camera sample application.
 * \author
 *         SmeshLink
 */

#include "camera-rest-server.h"
#include "camera-udp-server.h"
#include "camera-tcp-server.h"

/*---------------------------------------------------------------------------*/
/* Enable restful server and tcp-based server. */
AUTOSTART_PROCESSES(&camera_rest_server, &camera_tcp_server);
/*---------------------------------------------------------------------------*/

/*
 * Copyright (c) 2011-2012, SmeshLink Technology Ltd.
 * All rights reserved.
 *
 * $Id: camera-udp.c $
 */

/**
 * \file
 *         Camera UDP server.
 * \author
 *         SmeshLink
 */

#include "contiki.h"

PROCESS_NAME(camera_udp_without_retransmission_server);
AUTOSTART_PROCESSES(&camera_udp_without_retransmission_server);

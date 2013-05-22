/*
 * Copyright (c) 2001, Adam Dunkels.
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
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by Adam Dunkels.
 * 4. The name of the author may not be used to endorse or promote
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
 * $Id: enc28j60-drv.h,v 1.2 2007/05/26 23:05:36 oliverschmidt Exp $
 *
 */
#ifndef __ENC28J60_DRV_H__
#define __ENC28J60_DRV_H__

#include "contiki.h"
#include "net/uip.h"

#define UIP_8023_ADDR_LEN 6

/** This macro will protect the following code from interrupts.*/
#define AVR_ENTER_CRITICAL_REGION( ) {uint8_t volatile saved_sreg = SREG; cli( )}

/** This macro must always be used in conjunction with AVR_ENTER_CRITICAL_REGION
    so that interrupts are enabled again.*/
#define AVR_LEAVE_CRITICAL_REGION( ) {SREG = saved_sreg;}

PROCESS_NAME(enc28j60_process);

u8_t enc28j60_output(uip_lladdr_t * lladdr);
void enc28j60_exit(void);
void enc28j60_init(void);
void enc28j60_set_input_callback(void (*callback)(void));

#endif /* __ENC28J60_DRV_H__ */

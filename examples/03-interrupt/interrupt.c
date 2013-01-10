/*
 * Copyright (c) 2011-2012 SmeshLink Technology Corporation.
 * All rights reserved.
 *
 * $Id: interrupt.c $
 */

/**
 * \file
 *         A simple application showing how interrupts work.
 * \author
 *         SmeshLink
 */

#include "contiki.h"
#include "dev/leds.h"
#include "dev/interrupt.h"

/*---------------------------------------------------------------------------*/
PROCESS(interrupt_sample_process, "Interrupt sample process");
AUTOSTART_PROCESSES(&interrupt_sample_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(interrupt_sample_process, ev, data)
{
  /* Any process must start with this. */
  PROCESS_BEGIN();

  interrupt_init(1, 1, 1, 1);
  interrupt_enable(INT0);
  //interrupt_enable(INT1);

  /* Register current process with INT0 & INT1*/
  interrupt_register(INT0);
  interrupt_register(INT1);

  while(1) {
    /* Wait for an event. */
    PROCESS_WAIT_EVENT();

    /* Got the interrupt event~ */
    if (ev == PROCESS_EVENT_INTERRUPT) {
      /* Check for the int_vect. */
      if (INT0 == ((struct interrupt *)data)->int_vect) {
        /* Got an INT0 interrupt. */
        leds_toggle(LEDS_RED);
      }
      else if (INT1 == ((struct interrupt *)data)->int_vect) {
        /* Got an INT1 interrupt. */
        leds_toggle(LEDS_YELLOW);
        interrupt_disable(INT0);
      }
    }
  } // while (1)

  /* Any process must end with this, even if it is never reached. */
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

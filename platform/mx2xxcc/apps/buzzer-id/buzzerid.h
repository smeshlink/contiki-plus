/*
 * Copyright (c) 2012, SmeshLink Technology Ltd.
 * All rights reserved.
 *
 */

/**
* \file
*         Notifies the node id with a buzzer.
* \author
*         SmeshLink
*/

#ifndef __BUZZERID_H__
#define __BUZZERID_H__

void buzzer_init(void);

/**
 * Returns the current status of all leds (respects invert)
 */
extern void buzzer_on();
extern void buzzer_off();
extern void buzzer_nodeid();

void delay50us(int t);
void delayms(int t);

#endif /* __LEDS_H__ */

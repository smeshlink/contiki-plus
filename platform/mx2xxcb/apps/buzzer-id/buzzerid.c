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

#include "buzzerid.h"
#include "sys/clock.h"
#include "sys/energest.h"

/*BUZZER ports*/
#define BUZZER_PxDIR DDRC
#define BUZZER_PxOUT PORTC
#define BUZZER_CONF _BV(1)

#define SET_PIN_OUTPUT() (BUZZER_PxDIR |= BUZZER_CONF)
#define OUTP_0() (BUZZER_PxOUT &= ~BUZZER_CONF)
#define OUTP_1() (BUZZER_PxOUT |= BUZZER_CONF)

void
delay50us(int t)
{
  unsigned char j;
  for(;t>0;t--)
    for(j=0;j<70;j++)
      asm volatile("nop");
}
void
delayms(int t)
{
  unsigned short j;
    for(;t>0;t--)
        for(j=0;j<8000;j++)
          asm volatile("nop");
}
/*---------------------------------------------------------------------------*/
#define BUZZER_DRIVE() do {                    \
                     SET_PIN_OUTPUT();     \
                     OUTP_1();             \
                   } while (0)

/* Release the one wire by turning on the internal pull-up. */
#define BUZZER_RELEASE() do {                  \
                       SET_PIN_OUTPUT();    \
                       OUTP_0();           \
                     } while (0)

/*---------------------------------------------------------------------------*/
void
buzzer_init(void)
{
	BUZZER_RELEASE();
}
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
extern void
buzzer_on()
{
	BUZZER_DRIVE();
	return;
}
/*---------------------------------------------------------------------------*/
extern void
buzzer_off()
{
	BUZZER_RELEASE();
	return;
}
/*---------------------------------------------------------------------------*/

void buzzer_high(uint8_t n){

   uint8_t j;

   for(j=0;j<n;j++){

	   buzzer_on();

			delay50us(2000);

			buzzer_off();

			delay50us(2000);

   }

}

void buzzer_low(uint8_t n){

   uint8_t j;

   for(j=0;j<n;j++){

	   buzzer_on();

			delay50us(40);

			buzzer_off();

			delay50us(40);

			buzzer_on();

			   delay50us(40);

			   buzzer_off();

			delay50us(40);

   }

}

void buzzer_nodeid(uint16_t n) {

   uint16_t j,k;
   uint16_t ms=10000;

   if(((j=n/65535))>0){

			for(k=0;k<j;k++){

					 buzzer_high(1);

					 delay50us(2000);

			}

			n=n%65535;

			buzzer_low(5);

			delay50us(ms);

   }



   if((j=n/4096)>0){

			for(k=0;k<j;k++){

					 buzzer_high(1);

					 delay50us(2000);
			}

			n=n%4096;

			buzzer_low(5);

			delay50us(ms);

   }



   if((j=n/256)>0){

			for(k=0;k<j;k++){

					 buzzer_high(1);

					 delay50us(4000);

			}

			n=n%256;

			buzzer_low(5);

			delay50us(ms);

   }



   if((j=n/16)>0){

			for(k=0;k<j;k++){

					 buzzer_high(1);

					 delay50us(4000);

			}

			n=n%16;

			buzzer_low(5);

			delay50us(ms*2);

   }

   if((j=n)>0){

			for(k=0;k<j;k++){

					 buzzer_high(1);

					 delay50us(4000);

			}

			//buzzer_low(1);

   }
   //buzzer_off();
}

/*
 * Copyright (c) 2011-2012 SmeshLink Technology Corporation.
 * All rights reserved.
 *
 * $Id: sht11-sensors-sample.c $
 */

/**
 * \file
 *         This application shows how read data of temperature and humidity
 *         using sht11 sensors interfaces.
 * \author
 *         SmeshLink
 */

#include "contiki.h"

#include "dev/sht11.h"

#include <stdio.h>

/*---------------------------------------------------------------------------*/
PROCESS(sensors_sample_process, "Sensors sample process");
AUTOSTART_PROCESSES(&sensors_sample_process);

float sht11_TemperatureC(int rawdata)
{
  int _val;                // Raw value returned from sensor
  float _temperature;      // Temperature derived from raw value

  // Conversion coefficients from SHT11 datasheet
  const float D1 = -39.6;
  const float D2 =   0.01;

  // Fetch raw value
  _val = rawdata;

  // Convert raw value to degrees Celsius
  _temperature = (_val * D2) + D1;

  return (_temperature);
}

float sht11_Humidity(int temprawdata,int humidityrawdata)
{
  int _val;                    // Raw humidity value returned from sensor
  float _linearHumidity;       // Humidity with linear correction applied
  float _correctedHumidity;    // Temperature-corrected humidity
  float _temperature;          // Raw temperature value

  // Conversion coefficients from SHT15 datasheet
  const float C1 = -4.0;       // for 12 Bit
  const float C2 =  0.0405;    // for 12 Bit
  const float C3 = -0.0000028; // for 12 Bit
  const float T1 =  0.01;      // for 14 Bit @ 5V
  const float T2 =  0.00008;   // for 14 Bit @ 5V

  _val = humidityrawdata;
   _linearHumidity = C1 + C2 * _val + C3 * _val * _val;

  // Get current temperature for humidity correction
  _temperature = sht11_TemperatureC(temprawdata);

  // Correct humidity value for current temperature
  _correctedHumidity = (_temperature - 25.0 ) * (T1 + T2 * _val) + _linearHumidity;

  return (_correctedHumidity);
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(sensors_sample_process, ev, data)
{
  /* Variables are declared static to ensure their values are kept between kernel calls. */
  static struct etimer timer;

  /* Any process must start with this. */
  PROCESS_BEGIN();

  /* Set the etimer to generate an event in one second. */
  etimer_set(&timer, CLOCK_CONF_SECOND);

  sht11_init();

  while(1) {
    /* Wait for an event. */
    PROCESS_WAIT_EVENT();

    /* Got the timer's event~ */
    if (ev == PROCESS_EVENT_TIMER) {
      /* Read temperature value. */
      unsigned int temp = sht11_temp();
      /* Read humidity value. */
      unsigned int humidity = sht11_humidity();

      printf("temp raw data:%u\nhumidity raw data:%u\n", temp, humidity);
      float tc=sht11_TemperatureC(temp);
      float hc=sht11_Humidity(temp,humidity);
      printf("temp:%u.%u\nhumidity:%u.%u\n",(int)tc,((int)(tc*10))%10 , (int)hc,((int)(hc*10))%10);

      /* Reset the etimer so it will generate another event after the exact same time. */
      etimer_reset(&timer);
    }
  } // while (1)

  /* Any process must end with this, even if it is never reached. */
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/

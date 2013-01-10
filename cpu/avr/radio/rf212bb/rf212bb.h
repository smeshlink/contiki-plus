/*   Copyright (c) 2008, Swedish Institute of Computer Science
 *  All rights reserved.
 *
 *  Additional fixes for AVR contributed by:
 *
 *	Colin O'Flynn coflynn@newae.com
 *	Eric Gnoske egnoske@gmail.com
 *	Blake Leverett bleverett@gmail.com
 *	Mike Vidales mavida404@gmail.com
 *	Kevin Brown kbrown3@uccs.edu
 *	Nate Bohlmann nate@elfwerks.com
 *  David Kopf dak664@embarqmail.com
 *
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of the copyright holders nor the names of
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 *  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 *  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 *  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 *  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 *  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */
/**
 *    \addtogroup radiorf212
 *   @{
 */
/**
 *  \file
 *  \brief This file contains radio driver code.
 *
 */

#ifndef RADIO_H
#define RADIO_H
/*============================ INCLUDE =======================================*/
#include <stdint.h>
#include <stdbool.h>
#include "hal.h"
#include "at86rf212_registermap.h"



/*============================ MACROS ========================================*/
#define SUPPORTED_PART_NUMBER                   ( 7 )
#define RF212_REVA                              ( 1 )
#define RF212_REVB                              ( 2 )
#define SUPPORTED_MANUFACTURER_ID               ( 31 )


/* RF212 does not support RX_START interrupts in extended mode, but it seems harmless to always enable it. */
/* In non-extended mode this allows RX_START to sample the RF rssi at the end of the preamble */
//#define RF212_SUPPORTED_INTERRUPT_MASK        ( 0x08 )  //enable trx end only
//#define RF212_SUPPORTED_INTERRUPT_MASK          ( 0x0F ) //disable bat low, trx underrun
#define RF212_SUPPORTED_INTERRUPT_MASK          ( HAL_TRX_END_MASK | HAL_RX_START_MASK | HAL_BAT_LOW_MASK | HAL_AMI_MASK ) //( 0x0C )  //disable bat low, trx underrun, pll lock/unlock


#define RF212_MIN_CHANNEL                       ( 0 )
#define RF212_MAX_CHANNEL                       ( 3 )
#define RF212_MIN_ED_THRESHOLD                  ( 0 )
#define RF212_MAX_ED_THRESHOLD                  ( 15 )
#define RF212_MAX_TX_FRAME_LENGTH               ( 127 ) /**< 127 Byte PSDU. */
//#define rf212_MAX_PACKET_LEN                    127
//rf212
#define RF212_MAX_FRAME_RETRIES					( 2 )

#define RF212_TX_PWR_5DBM_BOOST_MODE            ( 0xe8 )
#define RF212_ENABLE_PA_BOOST					(1 << 7)
#define TRX_CTRL2_BPSK_20KB						( 0x00 )
#define TRX_CTRL2_OQPSK_100KB					( 0x08 )
#define TRX_CTRL2_OQPSK_250KB					( 0x1C )

#define CC_BAND 								4
#define CC_NUMBER 								11
#define CC_CHANNEL								11

//according to 802.15.4 one of these options is mandatory
#define CCA_ED_OR_CS							(0x00)
#define CCA_ED_AND_CS							(0x03)
#define CCA_ED_DEFAULT_THRESHOLD				(0x07)
#define CCA_ED_MAX_THRESHOLD_BPSK_20			(0x08)
#define CCA_LBT_ENABLED							(0x01)

//
#define TX_PWR_8DBM                             ( 0xE4 )
#define TX_PWR_N11DBM                          	( 0x0A )

#define TX_PWR_MAX                             TX_PWR_8DBM
#define TX_PWR_MIN                             TX_PWR_N11DBM
#define TX_PWR_UNDEFINED                       (TX_PWR_MIN+1)


#define BATTERY_MONITOR_HIGHEST_VOLTAGE         ( 15 )
#define BATTERY_MONITOR_VOLTAGE_UNDER_THRESHOLD ( 0 )
#define BATTERY_MONITOR_HIGH_VOLTAGE            ( 1 )
#define BATTERY_MONITOR_LOW_VOLTAGE             ( 0 )

#define FTN_CALIBRATION_DONE                    ( 0 )
#define PLL_DCU_CALIBRATION_DONE                ( 0 )
#define PLL_CF_CALIBRATION_DONE                 ( 0 )

#define RC_OSC_REFERENCE_COUNT_MAX  (1.005*F_CPU*31250UL/8000000UL)
#define RC_OSC_REFERENCE_COUNT_MIN  (0.995*F_CPU*31250UL/8000000UL)

#ifndef RF_CHANNEL
#define RF_CHANNEL              0
#endif
/*============================ TYPEDEFS ======================================*/

/** \brief  This macro defines the start value for the RADIO_* status constants.
 *
 *          It was chosen to have this macro so that the user can define where
 *          the status returned from the TAT starts. This can be useful in a
 *          system where numerous drivers are used, and some range of status codes
 *          are occupied.
 *
 *  \see radio_status_t
 */
#define RADIO_STATUS_START_VALUE                  ( 0x40 )

/** \brief  This enumeration defines the possible return values for the TAT API
 *          functions.
 *
 *          These values are defined so that they should not collide with the
 *          return/status codes defined in the IEEE 802.15.4 standard.
 *
 */
typedef enum{
    RADIO_SUCCESS = RADIO_STATUS_START_VALUE,  /**< The requested service was performed successfully. */
    RADIO_UNSUPPORTED_DEVICE,         /**< The connected device is not an Atmel AT86RF212. */
    RADIO_INVALID_ARGUMENT,           /**< One or more of the supplied function arguments are invalid. */
    RADIO_TIMED_OUT,                  /**< The requested service timed out. */
    RADIO_WRONG_STATE,                /**< The end-user tried to do an invalid state transition. */
    RADIO_BUSY_STATE,                 /**< The radio transceiver is busy receiving or transmitting. */
    RADIO_STATE_TRANSITION_FAILED,    /**< The requested state transition could not be completed. */
    RADIO_CCA_IDLE,                   /**< Channel is clear, available to transmit a new frame. */
    RADIO_CCA_BUSY,                   /**< Channel busy. */
    RADIO_TRX_BUSY,                   /**< Transceiver is busy receiving or transmitting data. */
    RADIO_BAT_LOW,                    /**< Measured battery voltage is lower than voltage threshold. */
    RADIO_BAT_OK,                     /**< Measured battery voltage is above the voltage threshold. */
    RADIO_CRC_FAILED,                 /**< The CRC failed for the actual frame. */
    RADIO_CHANNEL_ACCESS_FAILURE,     /**< The channel access failed during the auto mode. */
    RADIO_NO_ACK,                     /**< No acknowledge frame was received. */
}radio_status_t;


/**
 * \name Transaction status codes
 * \{
 */
#define TRAC_SUCCESS                0
#define TRAC_SUCCESS_DATA_PENDING   1
#define TRAC_SUCCESS_WAIT_FOR_ACK   2
#define TRAC_CHANNEL_ACCESS_FAILURE 3
#define TRAC_NO_ACK                 5
#define TRAC_INVALID                7
/** \} */


/** \brief  This enumeration defines the possible modes available for the
 *          Clear Channel Assessment algorithm.
 *
 *          These constants are extracted from the datasheet.
 *
 */
typedef enum{
//    CCA_ED                   = 0,    /**< Use energy detection above threshold mode. */ conflicts with atmega128rfa1 mcu definition
    CCA_ENERGY_DETECT         = 0,    /**< Use energy detection above threshold mode. */
    CCA_CARRIER_SENSE         = 1,    /**< Use carrier sense mode. */
    CCA_CARRIER_SENSE_WITH_ED = 2     /**< Use a combination of both energy detection and carrier sense. */
}radio_cca_mode_t;


/** \brief  This enumeration defines the possible CLKM speeds.
 *
 *          These constants are extracted from the RF212 datasheet.
 *
 */
typedef enum{
    CLKM_DISABLED      = 0,
    CLKM_1MHZ          = 1,
    CLKM_2MHZ          = 2,
    CLKM_4MHZ          = 3,
    CLKM_8MHZ          = 4,
    CLKM_16MHZ         = 5
}radio_clkm_speed_t;

typedef void (*radio_rx_callback) (uint16_t data);


/*	Hook Documentation 
**	
**	Sniffing Hooks:
**		RF212BB_HOOK_TX_PACKET(buffer,total_len)
**		RF212BB_HOOK_RX_PACKET(buf,len)
**
**	RF212BB_HOOK_IS_SEND_ENABLED()
**	RF212BB_HOOK_RADIO_ON()
**	RF212BB_HOOK_RADIO_OFF()
**	
*/


/*============================ PROTOTYPES ====================================*/

const struct radio_driver rf212_driver;

int rf212_init(void);
void rf212_warm_reset(void);
void rf212_start_sneeze(void);
//int rf212_on(void);
//int rf212_off(void);
void rf212_set_channel(uint8_t channel);
void rf212_listen_channel(uint8_t channel);
uint8_t rf212_get_channel(void);
void rf212_set_pan_addr(unsigned pan,unsigned addr,const uint8_t ieee_addr[8]);
void rf212_set_txpower(uint8_t power);
uint8_t rf212_get_txpower(void);

void rf212_set_promiscuous_mode(bool isPromiscuous);
bool rf212_is_ready_to_send();

extern uint8_t rf212_last_correlation,rf212_last_rssi,rf212_smallest_rssi;

uint8_t rf212_get_raw_rssi(void);

#define rf212_rssi	rf212_get_raw_rssi

#endif
/** @} */
/*EOF*/

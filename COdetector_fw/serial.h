/*
 * serial.h
 *
 * Created: 2015-10-02 21:22:03
 *  Author: Chebu
 */ 


#ifndef SERIAL_H_
#define SERIAL_H_

/*****************************************************************************************
   GLOBAL INCLUDES
*/

#include "common.h"

/*****************************************************************************************
   GLOBAL DEFINITIONS
*/

/*****************************************************************************************
   GLOBAL MACROS
*/

// LOGS:
#ifdef LOG_USARTD0
   #define LOG_TXT( txt, len )         serialSendD( (const uint8_t*)txt, (uint8_t)len )
   #define LOG_UINT( txt, len, val )   serialLogUintD ( (const uint8_t*)txt, (uint8_t)len, (uint32_t) val )
#else 
   #define LOG_TXT( txt, len )         // NULL
   #define LOG_UINT( txt, len, val )   // NULL
#endif

   
/*****************************************************************************************
   GLOBAL FUNCTIONS DECLARATIONS
*/

void serialInitD ( void );

void serialSendD ( const uint8_t* data, uint8_t len );

void serialLogUintD ( uint8_t* txt, uint8_t len, uint32_t val );


#endif /* SERIAL_H_ */



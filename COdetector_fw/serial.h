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
   #define LOG_TXT( txt )              serialSendD( (uint8_t*)txt, (sizeof(txt)-1) )
   #define LOG_TXT_WL( txt, len )      serialSendD( (uint8_t*)txt, len )
   #define LOG_UINT( txt, val )        serialLogUintD ( (uint8_t*)txt, (sizeof(txt)-1), (uint32_t) val )
#else 
   #define LOG_TXT( txt )              // NULL
   #define LOG_TXT_WL( txt, len )      // NULL
   #define LOG_UINT( txt, val )        // NULL
#endif

   
/*****************************************************************************************
   GLOBAL FUNCTIONS DECLARATIONS
*/

void serialInitD ( void );

void serialSendD ( uint8_t* data, uint8_t len );

void serialLogUintD ( uint8_t* txt, uint8_t len, uint32_t val );


#endif /* SERIAL_H_ */



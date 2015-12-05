/*
 * system.h
 *
 * Created: 2015-10-31 21:02:55
 *  Author: Chebu
 */ 


#ifndef SYSTEM_H_
#define SYSTEM_H_


/*****************************************************************************************
   GLOBAL INCLUDES
*/

#include "common.h"

/*****************************************************************************************
   GLOBAL DEFINITIONS
*/


/*****************************************************************************************
   GLOBAL TYPEDEFS
*/

typedef struct
{
   uint16_t year;
   uint8_t  month;
   uint8_t  day;
   uint8_t  hour;
   uint8_t  min;
   uint8_t  sec;
   
} timeStruct_t;


typedef uint16_t meanType_t;

typedef struct
{
   meanType_t* pStart;
   meanType_t* pHead;
   uint16_t len;
   bool firstOF;
   
} meanQueue_t;


typedef struct 
{
   timeStruct_t sysTime;
   uint16_t actVal;
   meanType_t mean1mVal;
   meanType_t mean1hVal;
   meanType_t mean8hVal;
      
} valsToDisp_t;

/*****************************************************************************************
   GLOBAL FUNCTIONS DECLARATIONS
*/

void systemInit ( void );

void systemMeasEnd ( uint16_t val );



#endif /* SYSTEM_H_ */
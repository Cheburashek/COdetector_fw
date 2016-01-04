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

#define TEMP_COMP_TAB_OFFSET_0C 40  // Offset for 0C 

/*****************************************************************************************
   GLOBAL TYPEDEFS
*/

typedef uint16_t measType_t;

typedef struct
{
   measType_t* pStart;
   measType_t* pHead;
   uint16_t len;

} meanQueue_t;

typedef struct 
{
   measType_t actSensVal;
   measType_t mean15sVal;
   measType_t mean1mVal;
   measType_t mean15mVal;
   measType_t mean1hVal;
   measType_t mean2hVal;  
   int8_t tempC;
   uint8_t battPer;   
   bool usbPlugged;
   bool lpFlag;
      
} valsToDisp_t;

/*****************************************************************************************
   GLOBAL FUNCTIONS DECLARATIONS
*/

void systemInit ( void );
void systemUSBStateChanged ( void );
void systemMeasEnd ( uint16_t val );
void systemResetMeasRes ( void );
void systemSensCodeSet ( uint16_t val );

#endif /* SYSTEM_H_ */
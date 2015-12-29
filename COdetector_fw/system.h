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
   measType_t actBattVal;
   measType_t mean1mVal;
   measType_t mean15mVal;
   measType_t mean1hVal;
   measType_t mean2hVal;
  
   bool usbPlugged;
   bool lpFlag;
      
} valsToDisp_t;

/*****************************************************************************************
   GLOBAL FUNCTIONS DECLARATIONS
*/

void systemInit ( void );
void systemUSBStateChanged ( void );
void systemMeasEnd ( uint16_t val );
void systemMeasPermFlagSet ( bool stat );
void systemResetMeasRes ( void );
#endif /* SYSTEM_H_ */
/*
 * timers.h
 *
 * Created: 2015-10-11 02:50:12
 *  Author: Chebu
 */ 


#ifndef TIMERS_H_
#define TIMERS_H_




/*****************************************************************************************
   GLOBAL INCLUDES
*/
#include "common.h"

/*****************************************************************************************
   GLOBAL DEFINITIONS
*/

typedef void (*pfnRTC) ( void ); 

/*****************************************************************************************
   GLOBAL FUNCTIONS DECLARATIONS
*/
void timerInit ( void );

void timerRegisterRtcCB ( pfnRTC cb );

#endif /* TIMERS_H_ */
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

/*****************************************************************************************
   GLOBAL FUNCTIONS DECLARATIONS
*/
void timerInit ( void );

void timerSingleUs( uint16_t us );

void timerSHARP ( void );


#endif /* TIMERS_H_ */
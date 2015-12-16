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

 // Prescaler 1024 (so 8MHz/1024-> 7812 ticks for 1s):
#define TIMER_TCC4_EN()      ( TCC4.CTRLA |= TC45_CLKSEL_DIV1024_gc )   
#define TIMER_TCC4_DIS()      ( TCC4.CTRLA |= TC45_CLKSEL_OFF_gc )   

/*****************************************************************************************
   GLOBAL TYPEDEFS
*/

typedef void (*pfnTimerCB_t) ( void ); 

// Typedef describing timer channel user configuration:
typedef struct 
{
   pfnTimerCB_t chCB;
   uint16_t period;  // Period
   bool rptFlag;      // rptFlaging flag (rptFlag timer event when set)
   
} channelStruct_t;




/*****************************************************************************************
   GLOBAL FUNCTIONS DECLARATIONS
*/
void timerInit ( void );

void timerRegisterRtcCB ( pfnTimerCB_t cb );

// Fail when couldn't find free channel:
uint8_t timerRegisterAndStart ( pfnTimerCB_t chCB, uint16_t period, bool rptFlag );

#endif /* TIMERS_H_ */
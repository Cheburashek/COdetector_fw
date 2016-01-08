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
#define TIMER_TCC5_EN()      ( TCC5.CTRLA |= TC45_CLKSEL_DIV1024_gc )   
#define TIMER_TCC5_DIS()     ( TCC5.CTRLA |= TC45_CLKSEL_OFF_gc )   

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

typedef enum
{
   CHA,
   CHB,
   CHC,
   CHD,
   TIM_ERROR = 0xFF
   
} eChanNr_t;


/*****************************************************************************************
   GLOBAL FUNCTIONS DECLARATIONS
*/
void timerInit ( uint8_t per );

void timerRegisterRtcCB ( pfnTimerCB_t cb );

// Fail when couldn't find free channel:
eChanNr_t timerRegisterAndStart ( pfnTimerCB_t chCB, uint16_t period, bool rptFlag );

void timerDeregister ( eChanNr_t ch );
void timerReset ( eChanNr_t ch ); 


#endif /* TIMERS_H_ */
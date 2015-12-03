/*
 * timers.c
 *
 * Created: 2015-10-11 02:49:22
 *  Author: Chebu
 */ 


// only for dr

/*****************************************************************************************
   LOCAL INCLUDES
*/
#include "timers.h"

/*****************************************************************************************
   MACROS
*/

/*****************************************************************************************
   LOCAL DEFINITIONS
*/


/*****************************************************************************************
   LOCAL VARIABLES
*/

static pfnRTC rtcCB = NULL;

/*****************************************************************************************
   LOCAL FUNCTIONS DECLARATIONS
*/

/*****************************************************************************************
   LOCAL FUNCTIONS DEFINITIONS
*/


/*****************************************************************************************
   GLOBAL FUNCTIONS DEFINITIONS
*/


void timerInit (void )
{
   // RTC:
   
   if ( !(OSC.CTRL & OSC_RC32KEN_bm) )          // If disabled
   {
      OSC.CTRL |= OSC_RC32KEN_bm;               // Enabling 32kHz clock
      while (!(OSC.STATUS & OSC_RC32KRDY_bm));  // Waiting for clock   
   }   
   
   while ( RTC.STATUS & RTC_SYNCBUSY_bm ){}      // Wait until SYNCBUSY is cleared
      
      
   RTC.PER = RTC_PERIOD_S * 128;                // 32ms period
   
   RTC.INTCTRL = CFG_PRIO_RTC_OVFL;     // from boardCfg.h
   
   while ( RTC.STATUS & RTC_SYNCBUSY_bm ){}  // Wait until SYNCBUSY is cleared 
   RTC.CTRL = RTC_PRESCALER_DIV256_gc;      // For 1s resolution

   
   CLK.RTCCTRL = CLK_RTCEN_bm           // RTC source enabled
               | CLK_RTCSRC_RCOSC32_gc;     // 32k ULP for RTC
   


   LOG_TXT ( ">>init<<   Timer initialized\n" );
}


//****************************************************************************************
void timerRegisterRtcCB ( pfnRTC cb )
{
   rtcCB = cb;   
}


//****************************************************************************************
ISR ( RTC_OVF_vect )
{
   DEB_2_TGL();
   
   if ( NULL != rtcCB )
   {
      rtcCB();
   }
}





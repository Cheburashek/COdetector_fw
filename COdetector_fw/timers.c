/*
 * timers.c
 *
 * Created: 2015-10-11 02:49:22
 *  Author: Chebu
 */ 

// Do opisu:
//wybralem 32,768 kHz jako zrodlo RTC, bo mala roznica poboru energii a dokladniejszy

// dynamiczne przyznawanie kana��w!!!

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

#define TIMER_NUMBER_OF_CHANS    4
#define TIMER_MULTI_MS           7.812
#define TIMER_MAX_PERIOD         7500            // 7.5s @8MHz and 1024 prescaler
        

/*****************************************************************************************
   LOCAL TYPEDEFS
*/

typedef enum
{
   CHA,
   CHB,
   CHC,
   CHD
      
} eChanNr_t;

/*****************************************************************************************
   LOCAL VARIABLES
*/

static pfnTimerCB_t rtcCB = NULL;

// Channels descriptors
static channelStruct_t chStruct[ TIMER_NUMBER_OF_CHANS ] = 
{
   { NULL, 0x00, false },
   { NULL, 0x00, false },
   { NULL, 0x00, false },
   { NULL, 0x00, false }
};


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
      
      
   RTC.PER = RTC_PERIOD_S * 1024;                // 1ms period
   
   RTC.INTCTRL = CFG_PRIO_RTC_OVFL;            // from boardCfg.h  
      
   RTC.CTRL = RTC_PRESCALER_DIV1_gc;         
   
   CLK.RTCCTRL = CLK_RTCEN_bm   |             // RTC source enabled
                 CLK_RTCSRC_RCOSC_gc;        // 1024kHz from 32k768Hz
   

   // TIMER4:
   
   TCC4.CTRLE |=  TC45_CCDMODE_COMP_gc |     // Compare mode on all channels
                  TC45_CCDMODE_COMP_gc |
                  TC45_CCDMODE_COMP_gc |
                  TC45_CCDMODE_COMP_gc;
   
   TCC4.PER = 0xFFFF;
           
}


//****************************************************************************************
void timerRegisterRtcCB ( pfnTimerCB_t cb )
{
   rtcCB = cb;   
}


//****************************************************************************************
uint8_t timerRegisterAndStart ( pfnTimerCB_t chCB, uint16_t period, bool rptFlag )
{
   if ( (NULL != chCB) && ( period < TIMER_MAX_PERIOD ) )
   {
      for ( eChanNr_t i = 0; i < TIMER_NUMBER_OF_CHANS; i++ )
      {
          if ( NULL == chStruct[i].chCB )   // Looking for free channel
          {
             chStruct[i].chCB = chCB;
             chStruct[i].rptFlag = rptFlag;
             chStruct[i].period = (uint16_t)((((float)period)*TIMER_MULTI_MS)+0.5); // Period from ms to ticks (stored for repeatable timer)
            
            
             uint32_t absPer = (uint32_t)TCC4.CNT + chStruct[i].period;
          
             if ( absPer > 0x0000FFFF )      // When sum of period and actual cnt value is greater than capacity of timer counter
             {
                absPer -= 0x0000FFFF;               
             }          

             switch (i)
             {
                case CHA:           
                  TCC4.CCA = (uint16_t)absPer;            // Setting value to compare                  
                  TCC4.INTCTRLB |= CFG_PRIO_TC4_CCALVL;      // Enabling interrupt with priority level defined in boardCfg.h      
                break;
             
                case CHB:
                  TCC4.CCB = (uint16_t)absPer;
                  TCC4.INTCTRLB |= CFG_PRIO_TC4_CCBLVL;
                break;
             
                case CHC:
                  TCC4.CCC = (uint16_t)absPer;
                  TCC4.INTCTRLB |= CFG_PRIO_TC4_CCCLVL;
                break;
             
                case CHD:
                  TCC4.CCD = (uint16_t)absPer;
                  TCC4.INTCTRLB |= CFG_PRIO_TC4_CCDLVL;
                break;                  
              }
              return SUCCESS;
          } 
      }
      LOG_TXT ( ">>ERR<<No free channel!\n");
      return FAIL;
   }
   else
   {
      LOG_TXT ( ">>ERR<<Period too long!\n"); 
      return FAIL;      
   }     
   return SUCCESS;   
}



//****************************************************************************************
// ISRs:

// RTC:
ISR ( RTC_OVF_vect )
{   
   if ( NULL != rtcCB )
   {
      rtcCB();
   }
}


// CCA:
ISR ( TCC4_CCA_vect )
{      
   if ( NULL != chStruct[CHA].chCB )
   {
      chStruct[CHA].chCB();  // Call back
   }   
   if ( false == chStruct[CHA].rptFlag )            // If non repeatable
   {       
      chStruct[CHA].period = 0; 
      chStruct[CHA].chCB = NULL;                   // Free this channel
      TCC4.INTCTRLB &= ~CFG_PRIO_TC4_CCALVL;       // Disabling interrupt
   }
   else 
   {
      uint32_t absPer = (uint32_t)TCC4.CNT + chStruct[CHA].period;
      
      if ( absPer > 0x0000FFFF )   // When sum of period and actual cnt value is greater than capacity of timer counter
      {
         absPer -= 0x0000FFFF;
      }  
      TCC4.CCA =  (uint16_t)absPer;    
   }
}

// CCB:
ISR ( TCC4_CCB_vect )
{   
   if ( NULL != chStruct[CHB].chCB )
   {
      chStruct[CHB].chCB();  // Call back
   }
   if ( false == chStruct[CHB].rptFlag )            // If non repeatable
   {
      chStruct[CHB].period = 0;
      chStruct[CHB].chCB = NULL;                   // Free this channel
      TCC4.INTCTRLB &= ~CFG_PRIO_TC4_CCBLVL;       // Disabling interrupt
   }
   else
   {
      uint32_t absPer = (uint32_t)TCC4.CNT + chStruct[CHB].period;
         
      if ( absPer > 0x0000FFFF )   // When sum of period and actual cnt value is greater than capacity of timer counter
      {
         absPer -= 0x0000FFFF;
      }
      TCC4.CCB =  (uint16_t)absPer;
   }
}

// CCC:
ISR ( TCC4_CCC_vect )
{
   if ( NULL != chStruct[CHC].chCB )
   {
      chStruct[CHC].chCB();  // Call back
   }   
   if ( false == chStruct[CHC].rptFlag )            // If non repeatable
   {
      chStruct[CHC].period = 0;
      chStruct[CHC].chCB = NULL;                   // Free this channel
      TCC4.INTCTRLB &= ~CFG_PRIO_TC4_CCCLVL;       // Disabling interrupt
   }
   else
   {
      uint32_t absPer = (uint32_t)TCC4.CNT + chStruct[CHC].period;
         
      if ( absPer > 0x0000FFFF )   // When sum of period and actual cnt value is greater than capacity of timer counter
      {
         absPer -= 0x0000FFFF;
      }
      TCC4.CCC =  (uint16_t)absPer;
   }
}

// CCD:
ISR ( TCC4_CCD_vect )
{
   if ( NULL != chStruct[CHD].chCB )
   {
      chStruct[CHD].chCB();  // Call back
   }
   if ( false == chStruct[CHD].rptFlag )            // If non repeatable
   {
      chStruct[CHD].period = 0;
      chStruct[CHD].chCB = NULL;                   // Free this channel
      TCC4.INTCTRLB &= ~CFG_PRIO_TC4_CCDLVL;       // Disabling interrupt
   }
   else
   {
      uint32_t absPer = (uint32_t)TCC4.CNT + chStruct[CHD].period;
         
      if ( absPer > 0x0000FFFF )   // When sum of period and actual cnt value is greater than capacity of timer counter
      {
         absPer -= 0x0000FFFF;
      }
      TCC4.CCD =  (uint16_t)absPer;
   }
}
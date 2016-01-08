/*
 * ADC.c
 *
 * Created: 2015-10-10 01:16:09
 *  Author: Chebu
 */ 

/*****************************************************************************************
   LOCAL INCLUDES
*/

#include "ADC.h"

/*****************************************************************************************
   MACROS
*/

/*****************************************************************************************
   LOCAL DEFINITIONS
*/

#define ADC_OFF_MAN_CORR   240      // bits in 12b
#define ADC_GAIN_MAN_CORR  0x07E1   // 357page in manual

#define ADC_GAIN_SOFT_CORR  350  // for additional, software offset error in 16b

/*****************************************************************************************
   LOCAL VARIABLES
*/

static pfnAdcEnd convEndCB = NULL;
static bool busyFlag = FALSE;

/*****************************************************************************************
   LOCAL FUNCTIONS DECLARATIONS
*/

/*****************************************************************************************
   LOCAL FUNCTIONS DEFINITIONS
*/

/*****************************************************************************************
   GLOBAL FUNCTIONS DEFINITIONS
*/

//****************************************************************************************
void adcInit ( void )
{
   // PORT:
   PORTA.DIRCLR = CFG_ADC_SENS_PIN_MASK;        // Input
   PORTA.DIRCLR = CFG_ADC_VBATT_PIN_MASK;        // Input

   
   ADCA.CH0.INTFLAGS = 0xFF;                          // Clearing int flags
   ADCA.CH0.INTCTRL = CFG_PRIO_ADC;                   // From boardCfg.h
   
   ADC_EN();
   
   ADCA.CTRLB = ADC_CURRLIMIT_HIGH_gc  |        // High current limit, max. sampling rate 75kSPS
                ADC_RESOLUTION_MT12BIT_gc;      // More than 12-bit right adjusted result, when (SAPNUM>0)
   
   ADCA.CTRLB &= ~(ADC_CONMODE_bm |             // Unsigned mode
                   ADC_FREERUN_bm);             // Single conversion
   
   ADCA.CH0.CTRL = ADC_CH_GAIN_1X_gc |           // 1x gain
                   ADC_CH_INPUTMODE_SINGLEENDED_gc;   // Singleended mode
   
   ADCA.EVCTRL = 0x00;                          // Ensuring that event system is disabled for ADC         
   
   ADCA.CH0.AVGCTRL = ADC_SAMPNUM_32X_gc ;      // Number of samples (averaging) - 16bit

   ADCA.SAMPCTRL = 0x08;   // For 8Mhz clock only!
   
   ADCA.PRESCALER = ADC_PRESCALER_DIV512_gc;
      
   ADCA.REFCTRL = ADC_REFSEL_INT1V_gc;                   // Internal 1V reference              

   ADCA.CH0.OFFSETCORR0 = ADC_OFF_MAN_CORR & 0xFF;
   ADCA.CH0.OFFSETCORR1 = ADC_OFF_MAN_CORR >> 8;
   
   ADCA.CH0.GAINCORR0 = ADC_GAIN_MAN_CORR & 0xFF;
   ADCA.CH0.GAINCORR1 = ADC_GAIN_MAN_CORR >> 8;          // Gain correction x1  
       
   ADCA.CH0.CORRCTRL = 0x01;     // Correction enabled
                               
}
 
 
//****************************************************************************************
void adcStartChannel ( eAdcChan_t ch )
{
   ADCA.CTRLA = ADC_FLUSH_bm; 
   ADC_EN();   
   
   
   ADCA.CH0.MUXCTRL = ch;
     

   ADCA.CTRLA |= ADC_START_bm;
}


//****************************************************************************************
void adcRegisterEndCb ( pfnAdcEnd cb )
{
   convEndCB = cb; 
}

//****************************************************************************************
bool adcIsBusy ( void )
{
   return busyFlag;
}


//****************************************************************************************
ISR ( ADCA_CH0_vect )
{  
   if ( NULL != convEndCB )
   {       
      if ( ADCA.CH0RES >= ADC_GAIN_SOFT_CORR ) 
      {       
         convEndCB ( ADCA.CH0RES - ADC_GAIN_SOFT_CORR ); // Software offset compensation
      }
      else
      {
         convEndCB ( 0x00 );
      }         
   }   
   busyFlag = FALSE;  
}
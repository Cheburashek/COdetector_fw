/*
 * ADC.c
 *
 * Created: 2015-10-10 01:16:09
 *  Author: Chebu
 */ 


// TODO: automatic offset calibration
// TODO: gain error calibration
// TODO: DMA ?
// TODO: eventsystem?

// niestabilny adc pomimo w miarê dobrego zasilania

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

/*****************************************************************************************
   LOCAL VARIABLES
*/

static pfnAdcEnd convEndCB = NULL;


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
   
   ADCA.CH0.CTRL = ADC_CH_GAIN_1X_gc;           // 1x gain
   
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
   
   if ( ch != TEMP )
   {   
      ADCA.CH0.CTRL = ADC_CH_INPUTMODE_SINGLEENDED_gc;   // Single ended input
      ADCA.CH0.MUXCTRL = ch;
   }   
   else 
   {
      ADCA.CH0.CTRL = ADC_CH_INPUTMODE_INTERNAL_gc;   // Internal input
      ADCA.CH0.MUXCTRL = ADC_CH_MUXINT_TEMP_gc;       // Temperature reference
   }   
   
   ADCA.CTRLA |= ADC_START_bm;
}



//****************************************************************************************
void adcRegisterEndCb ( pfnAdcEnd cb )
{
   convEndCB = cb; 
}


//****************************************************************************************
ISR ( ADCA_CH0_vect )
{  
   if ( NULL != convEndCB )
   {                  
      convEndCB (ADCA.CH0RES); // Software offset compensation
   }     
}
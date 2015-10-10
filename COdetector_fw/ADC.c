/*
 * ADC.c
 *
 * Created: 2015-10-10 01:16:09
 *  Author: Chebu
 */ 


// TODO: automatic offset calibration
// TODO: DMA ?
// TODO: eventsystem
// TODO: averaging
// TODO: API to select channel with should start conversion


/*****************************************************************************************
   LOCAL INCLUDES
*/
#include "common.h"
#include "ADC.h"

/*****************************************************************************************
   MACROS
*/

/*****************************************************************************************
   LOCAL DEFINITIONS
*/

#define ADC_START()        ( ADCA.CTRLA |= ADC_START_bm   )       // Start single covertion
#define ADC_EN()           ( ADCA.CTRLA |= ADC_ENABLE_bm  )
#define ADC_DIS()          ( ADCA.CTRLA &= ~ADC_ENABLE_bm )

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


void adcInit ( void )
{

   /* Default settings
      - unsigned mode
      - 12b resolution, right-adjusted
      - internal 1V ref
      - clock divided by 4 (maybe change for low power)
      - gain x1
      - interrupt on complete conversion

   */

//SAMPCTRL ?

   // PORT:
   PORTA.DIRCLR = 1 << CFG_ADC_PIN_NUM; // Input
   
   ADCA.CTRLB |= ADC_CURRLIMIT_HIGH_gc;        // High current limit, max. sampling rate 75kSPS
   
   ADCA.CH0.AVGCTRL = ADC_SAMPNUM_8X_gc;       // Number of samples (averaging)
   
   ADCA.CH0.INTCTRL = CFG_PRIO_ADC;            // From boardCfg.h
   
   ADC_EN();       //   Enabling ADC block
   
   LOG_TXT ( ">>init<<   ADC initialized\n", 28 );
}



void adcStartChToGnd ( ADC_CH_MUXPOS_t ch )
{
   ADCA.CH0.MUXCTRL = ch | ADC_CH_MUXNEGL_INTGND_gc;       
   ADCA.CH0.CTRL = ADC_CH_INPUTMODE_SINGLEENDED_gc;   // Single ended mode
   ADC_START();
}


void adcRegisterEndCb ( pfnAdcEnd cb )
{
   convEndCB = cb;
}




ISR ( ADCA_CH0_vect )
{
   uint16_t result = (((uint16_t)ADCA.CH0RESH)<<8) +  ADCA.CH0RESL;
   
   if ( NULL != convEndCB )
   {
      convEndCB ( result );      
   }
   
}
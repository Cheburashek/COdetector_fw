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

#define ADC_START()        ( ADCA.CTRLA |= ADC_START_bm   )       // Start single conversion
#define ADC_EN()           ( ADCA.CTRLA |= ADC_ENABLE_bm  )
#define ADC_DIS()          ( ADCA.CTRLA &= ~ADC_ENABLE_bm )

#define ADC_OFF_MAN_CORR   175   // Manually measured offset in 12b storage

/*****************************************************************************************
   LOCAL VARIABLES
*/

static pfnAdcEnd convEndCB = NULL;
static void adcOffCalibration ( void );

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
   /* Default settings
      - unsigned mode
      - 12b resolution, right-adjusted
      - internal 1V ref
      - clock divided by 4 (maybe change for low power)
      - gain x1
      - interrupt on complete conversion

   */

   // PORT:
   PORTA.DIRCLR = CFG_ADC_PIN_MASK;             // Input
   
   ADCA.CTRLB = ADC_CURRLIMIT_HIGH_gc  |        // High current limit, max. sampling rate 75kSPS
                ADC_RESOLUTION_MT12BIT_gc;      // More than 12-bit right adjusted result, when (SAPNUM>0)
                  
   ADCA.CH0.AVGCTRL = ADC_SAMPNUM_32X_gc ;       // Number of samples (averaging) - 16bit

   ADCA.SAMPCTRL = 0x08;   // For 8Mhz clock only!
   
   ADCA.PRESCALER = ADC_PRESCALER_DIV512_gc;
   
   ADC_EN();                                    // Enabling ADC block
   //adcOffCalibration ();
   
   ADCA.CH0.CTRL = ADC_CH_INPUTMODE_SINGLEENDED_gc;   // Single ended input
   ADCA.CH0.INTCTRL = CFG_PRIO_ADC;                   // From boardCfg.h
   
   
   ADCA.CH0.OFFSETCORR0 = ADC_OFF_MAN_CORR & 0xFF;
   ADCA.CH0.OFFSETCORR1 = ADC_OFF_MAN_CORR >> 8;
   
   ADCA.CH0.GAINCORR0 = 0x00;
   ADCA.CH0.GAINCORR1 = 0x08;          // Gain correction x1  
       
   ADCA.CH0.CORRCTRL = 0x01;     // Correction enabled
                               
   ADC_EN();
   
   LOG_TXT ( ">>init<<   ADC initialized\n", 28 );
}
 
 

////****************************************************************************************
//static void adcOffCalibration ( void )
//{   
   //ADCA.CH0.CTRL = ADC_CH_INPUTMODE_DIFFWGAINL_gc;    // Only for calibration
   //
   //ADCA.CH0.MUXCTRL = ADC_CH_MUXPOS_PIN0_gc |         // Connect to one pin - offset cal
                      //ADC_CH_MUXNEGL_PIN0_gc ;
                      //
   //ADC_START();  
   //while ( !ADCA.INTFLAGS ){;}
   //ADCA.INTFLAGS = 0x01;         // Clearing this flag     
   //LOG_UINT ("rr ", 3, ADCA.CH0RES>>4 );
//
   //uint16_t offTemp = ADCA.CH0RES >> 8;   // For 16b->12b
//
//
   //ADCA.CH0.OFFSETCORR0 = offTemp & 0xFF; 
   //ADCA.CH0.OFFSETCORR1 = offTemp >> 8;  
     //
   //ADCA.CH0.GAINCORR0 = 0x00;
   //ADCA.CH0.GAINCORR1 = 0x08;          // Gain correction x1
   //
   //ADCA.CH0.CORRCTRL = 0x01;     // Correction enabled
   //
   //LOG_UINT ( ">>adc<<  Offset calibration: ", 30, (((uint16_t)ADCA.CH0.OFFSETCORR1)<<8)+ADCA.CH0.OFFSETCORR0 );
//}



//****************************************************************************************
void adcStartChToGnd ( void )
{
   ADCA.CH0.MUXCTRL = CFG_ADC_MUXPOS; 
   DEB_1_SET(); 
   ADC_START();
}


//****************************************************************************************
void adcRegisterEndCb ( pfnAdcEnd cb )
{
   convEndCB = cb; 
}


//****************************************************************************************
ISR ( ADCA_CH0_vect )
{  
   DEB_1_CLR();
   if ( NULL != convEndCB )
   {       
      convEndCB (  (uint16_t)ADCA.CH0RES );      
   }  
   
}
/*
 * ADC.c
 *
 * Created: 2015-10-10 01:16:09
 *  Author: Chebu
 */ 


// TODO: automatic offset calibration
// TODO: DMA ?
// TODO: event system
// TODO: averaging in macro time
// TODO: API to select channel with should start conversion


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

#define ADC_OFFSET_RAW_VAL 170


/*****************************************************************************************
   LOCAL VARIABLES
*/

static pfnAdcEnd_t convEndCB = NULL;
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
   
   
   PORTA.DIRCLR = CFG_ADC_PIN_MASK;   
   
          
               // Enabling ADC block
   ADCA.INTFLAGS = 0x00;
   ADCA.REFCTRL = ADC_REFSEL_INT1V_gc;
   ADCA.EVCTRL = 0x00;  // No events
   ADCA.PRESCALER = ADC_PRESCALER_DIV32_gc ;
   
   ADCA.CH0.AVGCTRL = ADC_SAMPNUM_1X_gc;   // Number of samples (averaging) - 16bit
   ADCA.CTRLB = ADC_RESOLUTION_MT12BIT_gc;
 
        
   ADCA.CH0.CTRL = ADC_CH_GAIN_1X_gc | ADC_CH_INPUTMODE_SINGLEENDED_gc;   // Single ended input             
   
   ADCA.CTRLB &= ~(1<<4);   // Unsigned mode
   ADCA.CH0.INTCTRL = CFG_PRIO_ADC;                   // From boardCfg.h

   // Calibration:
  
   ADCA.CH0.OFFSETCORR0 = ADC_OFFSET_RAW_VAL & 0xFF;
   ADCA.CH0.OFFSETCORR1 = ADC_OFFSET_RAW_VAL >> 8;  

   ADCA.CH0.GAINCORR0 = 0x00;
   ADCA.CH0.GAINCORR1 = 0x08;          // Gain correction x1
   ADCA.CH0.CORRCTRL = ADC_CH_CORREN_bm;     // Correction enabled
   
   ADC_EN();
   
   //TODO: store in NV
   LOG_TXT ( ">>init<<   ADC initialized\n", 28 );
}
 
 

//****************************************************************************************
//static void adcOffCalibration ( void )
//{   
   //ADCA.CH0.CTRL = ADC_CH_INPUTMODE_SINGLEENDED_gc;    // Only for calibration
   //
   //ADCA.CH0.MUXCTRL = ADC_CH_MUXPOS_PIN0_gc |         // Connect to one pin - offset cal
                      //ADC_CH_MUXNEGL_PIN0_gc ;
         //
  //// ADCA.CTRLB |= (1<<4);   // Signed mode
   //
                      //
   //ADC_START();  
   //while ( !ADCA.INTFLAGS );
   //ADCA.INTFLAGS = 0x01;         // Clearing this flag     
//
   //uint16_t offTemp = ADCA.CH0RES>>4;
  //// offTemp >>= 4; // Because of 16b resolution
//
   //LOG_UINT ( "off= ", 5, offTemp );
   //LOG_BIN ( "off= ", 5, offTemp, 16 );
//
   //ADCA.CH0.OFFSETCORR0 = offTemp & 0xFF; 
   //ADCA.CH0.OFFSETCORR1 = offTemp >> 8;  
     //
      ////ADCA.CH0.OFFSETCORR0 = 0x1A;
      ////ADCA.CH0.OFFSETCORR1 = 0x00;
     //
   //ADCA.CH0.GAINCORR0 = 0x00;
   //ADCA.CH0.GAINCORR1 = 0x08;          // Gain correction x1
   //
   //ADCA.CH0.CORRCTRL = ADC_CH_CORREN_bm;     // Correction enabled
   //ADCA.CTRLB &= ~(1<<4);   // Signed mode
   //
   //LOG_UINT ( ">>adc<<  Offset calibration: ", 30, (((uint16_t)ADCA.CH0.OFFSETCORR1)<<8)+ADCA.CH0.OFFSETCORR0 );
//}



//****************************************************************************************
void adcStartChToGnd ( void )
{
   ADCA.CH0.CTRL = ADC_CH_INPUTMODE_SINGLEENDED_gc;
   ADCA.CH0.MUXCTRL = CFG_ADC_MUXPOS;  
   ADC_START();
}


//****************************************************************************************
void adcRegisterEndCb ( pfnAdcEnd_t cb )
{
   convEndCB = cb; 
}


//****************************************************************************************
ISR ( ADCA_CH0_vect )
{  
   DEB_3_CLR();

   if ( NULL != convEndCB )   
   {
      convEndCB ();      
   }   
}
/*
 * main.c
 *
 * Created: 2015-10-02 20:50:49
 *  Author: Chebu
 */ 


// TODO: _delay not working correctly
// TODO: clearing LCD
// TODO: Add watchdog
// TODO: add critical section macro
// TODO: Add 32kHz low timer when in ULP mode

// TODO: ADC offset !

#include "common.h"

volatile uint16_t measRes = 0;



void adcTest ( uint16_t val )
{
   measRes = (uint16_t)((((uint32_t)val)*1000)/65535);    // in mV
   
   DEB_1_TGL();
   //pdcUint ( measRes, 1, 0, 7 );
   LOG_UINT ( "Result:  ", 9, (uint32_t) measRes );
}


int main(void)
{ 
   boardInit();


   timerSingleUs( 65000 );

   while(1){;}








// ADC part:

   //adcRegisterEndCb( adcTest );
   
   
   //pdcUint ( 65530, 2, 0, 5 );
   /*
   while(1)
   {      
      _delay_ms (80);
      adcStartChToGnd( ADC_CH_MUXPOS_PIN0_gc );
    
   }   */
   

/*
    while(1)
    {        
       for ( uint32_t i = 0; i < 5000; i++ ){;}
       serialSendC ( test, sizeof(test));
       DEB_2_TGL();
    }
   */
   /*
    while(1)
    {        
       for ( uint32_t i = 0; i < 5000; i++ ){;}
       spiSend ( test, sizeof(test));
       DEB_2_TGL();
    }*/
   
   while(1){;}
}


